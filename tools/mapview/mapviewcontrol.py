#-------------------------------------------------------------------------------
# Name:		map view control
# Purpose:	 generic map viewer using pygame
#
# Author:	  ilteroi
#
# Created:	 14-08-2014
#-------------------------------------------------------------------------------

from __future__ import division
import pygame as pg
import math
import zlib
from operator import itemgetter
import os
import sys

RADIUS_BIG = 12
RADIUS_SMALL = 9
DISTANCEX = int( 2*RADIUS_BIG )
DISTANCEY = int( math.sqrt(3)*RADIUS_BIG )
ODDOFFSET = int( DISTANCEX/2 )

terrain_color = { 
	-1: pg.Color("#444444"), #none
	0: pg.Color("#00AA00"), #grass
	1: pg.Color("#558800"), #plains
	2: pg.Color("#AAAA00"), #desert
	3: pg.Color("#AAAAAA"), #tundra
	4: pg.Color("#FFFFFF"), #snow
	5: pg.Color("#8888FF"), #coast
	6: pg.Color("#4444FF"), #ocean
	7: pg.Color("#444400"), #mountain
	8: pg.Color("#888800"), #hill
}

terrain_string = { 
	-1: "none",
	0: "grass",
	1: "plains",
	2: "desert",
	3: "tundra",
	4: "snow",
	5: "coast",
	6: "ocean",
	7: "mountain",
	8: "hill",
}

class Plot():
	def __init__(self,x,y,visible,terrain,owner):
		self.idx_x = x
		self.idx_y = y
		self.x = x*DISTANCEX + (y%2)*ODDOFFSET
		self.y = y*DISTANCEY
		self.visible = visible
		self.terrain = terrain
		self.owner = owner
		self.color_value = None

		#draw functions don't take alpha, so a little workaround is needed
		self.surface = pg.Surface( (2*RADIUS_BIG,2*RADIUS_BIG) ).convert()
		if not self.visible:
			self.surface.set_alpha(128)
			
	def screen_dist(self,pos):
		return math.sqrt( (pos[0]-self.x)**2 + (pos[1]-self.y)**2 )

	def show_details(self):
		print("plot (%d,%d) - owner %d\n\tterrain %s" %  \
			(self.idx_x,self.idx_y,self.owner,terrain_string.get(self.terrain,"unknown")))

	def make_color(self,value):
		color = pg.Color("#FF00FF")
		color.hsva = ( zlib.adler32( "%s" % zlib.adler32( "%s" % value ) ) % 360, 100, 100, 100 )
		return color

	def draw(self, target, view_mode):
		background = pg.Color(0,0,0)
		self.surface.fill( background )
		self.surface.set_colorkey( background )

		pg.draw.circle(self.surface, terrain_color[self.terrain], (RADIUS_BIG,RADIUS_BIG), RADIUS_BIG, 0)
		if (self.color_value):
			pg.draw.circle(self.surface, self.color_value, (RADIUS_BIG,RADIUS_BIG), RADIUS_SMALL, 0)

		target.blit( self.surface, (self.x-RADIUS_BIG,self.y-RADIUS_BIG) )

class PlotWithScore(Plot):
	
	def __init__(self,x,y,visible,terrain,owner,values,scale):
		Plot.__init__(self,x,y,visible,terrain,owner);

		self.values = values
		self.scale = scale

	def draw_indexed_color(self, target, view_mode):
		if (view_mode<len(self.values)):
			self.color_value = self.make_color( "%08d" % self.values[view_mode] ) 
		else:
			self.color_value = None

		Plot.draw(self, target, view_mode)

	def draw_interpolated_color(self, target, view_mode):
		if (view_mode<len(self.values)):
			g = int(255*math.sqrt(self.values[view_mode]/self.scale[view_mode]))
			self.color_value = pg.Color(g,g,g)
		else:
			self.color_value = None

		Plot.draw(self, target, view_mode)

	def draw(self, target, view_mode):
		self.draw_interpolated_color(target,view_mode)

	def show_details(self):
		Plot.show_details(self)
		print(";".join( map( str, self.values) ) )

class PlotViewer(object):
	"""A simple control class."""
	def __init__(self,caption,layer_names,plots,bounds,n_layers,fps=60.0):

		self.caption = caption
		self.layer_names = layer_names
		self.plots = plots
		self.bounds = bounds
		self.render_size = ( bounds[0]*DISTANCEX, bounds[1]*DISTANCEY )
		self.n_layers = n_layers
		self.view_mode = 0

		pg.init()
		
		#we render twice as large as the screen so that zooming looks ok
		self.screen = pg.display.set_mode( tuple( int(i/2) for i in self.render_size), pg.RESIZABLE)
		self.screen_rect = self.screen.get_rect()
		self.image = pg.Surface(self.render_size, flags=pg.SRCALPHA).convert()
		self.image_rect = self.image.get_rect()

		#initially zoom factor is one
		self.zoom = 1
		self.visible_rect = self.image.get_rect()

		#animation stuff
		self.clock = pg.time.Clock()
		self.fps = fps
		self.done = False

		self.set_caption()

	def make_caption(self):
		return "%s - %s - space to change" % (self.caption, self.layer_names.get(self.view_mode,"unknown layer").strip())

	def set_caption(self):
		pg.display.set_caption( self.make_caption() )

	def map_window_to_texture_coords(self,pos):
		#scale coords and flip y axis!
		sx = self.render_size[0] / self.screen_rect.w
		sy = self.render_size[1] / self.screen_rect.h
		cx = pos[0] * sx
		cy = self.render_size[1] - pos[1] * sy
		return map( int, (cx,cy) )

		# TODO: update for zooming ...

	def handle_keys(self,event):
		if event.type == pg.QUIT:
			self.done = True
		elif event.type == pg.KEYUP and event.key == pg.K_SPACE:
			self.view_mode = (self.view_mode + 1) % self.n_layers
			self.set_caption()
		elif event.type == pg.VIDEORESIZE:
			#enfore aspect ratio
			img_aspect = self.image_rect[3]/self.image_rect[2]
			forced_size = map( int, (event.size[0],event.size[0]*img_aspect) )
			self.screen = pg.display.set_mode(forced_size, pg.RESIZABLE)
			self.screen_rect = self.screen.get_rect()
		elif event.type == pg.MOUSEBUTTONDOWN and event.button == 1:
			cx,cy = self.map_window_to_texture_coords( event.pos )
			#find closest plot for the click
			distances = [ p.screen_dist( (cx,cy) ) for p in self.plots ]
			idx,mindist = min(enumerate(distances), key=itemgetter(1))
			#print(event.pos, sx, sy, cx, cy, mindist)
			if mindist<RADIUS_BIG:
				self.handle_click(idx)
		elif event.type == pg.MOUSEBUTTONDOWN and event.button == 4:
			self.zoom_in(event.pos)
		elif event.type == pg.MOUSEBUTTONDOWN and event.button == 5:
			self.zoom_out(event.pos)

	def zoom_in(self,center):
		
		targeted = self.map_window_to_texture_coords( center )
		
		self.zoom += 0.25
		vx = (self.image_rect[2]-self.image_rect[0]) / self.zoom
		vy = (self.image_rect[3]-self.image_rect[1]) / self.zoom

		#now make it so that the targeted position stays at the same coords
		# TODO

	def zoom_out(self,center):
		self.zoom = max( 1, self.zoom - 0.25 )
		
		# TODO

	def handle_click(self,plot_idx):
		self.plots[plot_idx].show_details(self.layer_names)

	def draw_plots(self):
		for p in self.plots:
			p.draw(self.image,self.view_mode)
					
	def update(self):

		self.image.fill(pg.Color("black"))

		self.draw_plots()
		
		#flip to adjust for the way it's displayed in civ
		self.image = pg.transform.flip( self.image, False, True )
		
		screen_size = self.screen_rect.size
		if screen_size != self.render_size:
			pg.transform.smoothscale(self.image, screen_size, self.screen)
		else:
			self.screen.blit(self.image, (0,0))

	def next_frame(self):
		pass

	def main_loop(self):
		"""Loop-dee-loop."""
		while not self.done:
			for event in pg.event.get():
				self.handle_keys(event)
			self.update()
			pg.display.update()
			self.clock.tick(self.fps)
			self.next_frame()
			
		pg.quit()

layer_string = {
	0: "score",
}

#default viewer
if __name__ == "__main__":
	
	expected_columns = 6
	
	if not os.path.isfile(sys.argv[1]):
		sys.exit(1)
	
	plots = []
	scale = []
	for line in open(sys.argv[1],"r"):
		if (line[0]=='#'):
			continue
		data = map(int, line.split(","))
		while len(scale)<len(data):
			scale.append(1)
		for i in xrange(len(data)):
			scale[i] = max( scale[i], abs(data[i]) )
	
	#do this before actually creating the plots (need to initialize pygame)
	run_it = PlotViewer( "Tactical Cells", layer_string, plots, (scale[0]+1,scale[1]+1), expected_columns-5 )

	for line in open(sys.argv[1],"r"):
		if (line[0]=='#'):
			continue
		data = map(int, line.split(","))
		#pad with zeros
		while len(data)<expected_columns:
			data.append(0)
		plots.append( PlotWithScore(data[0],data[1],data[2],data[3],data[4],data[5:],scale[5:]) )
	
	run_it.main_loop()
	sys.exit()