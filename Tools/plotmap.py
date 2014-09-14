#-------------------------------------------------------------------------------
# Name:		module1
# Purpose:	 
#
# Author:	  Christoph
#
# Created:	 14-08-2014
# Copyright:   (c) Christoph 2014
# Licence:	 <your licence>
#-------------------------------------------------------------------------------

import os
import sys
import pygame as pg
import math
import zlib
from operator import itemgetter

#todo: danger map

CAPTION = "Fake Map Viewer (space to change view)"

RADIUS_BIG = 12
RADIUS_SMALL = 9
DISTANCEX = int( 2*RADIUS_BIG )
DISTANCEY = int( math.sqrt(3)*RADIUS_BIG )
ODDOFFSET = int( DISTANCEX/2 )

ai_unit_type = {
	1: "UNITAI_SETTLE",					# these are Settlers
	2: "UNITAI_WORKER",					# builder
	3: "UNITAI_ATTACK",					# use these to attack other units
	4: "UNITAI_CITY_BOMBARD",			# use these to attack cities
	5: "UNITAI_FAST_ATTACK",			# use these to pillage enemy improvements and attack barbarians
	6: "UNITAI_DEFENSE",				# these are units that are mainly in the floating defense force
	7: "UNITAI_COUNTER",				# these are counter-units to specific other units - these will likely need more logic in building and using
	8: "UNITAI_RANGED",					# ranged attacks
}

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

feature_string = { 
	-1: "none",
	0: "ice",
	1: "jungle",
	2: "marsh",
	3: "oasis",
	4: "floodplain",
	5: "forest",
	6: "fallout",
	7: "naturalwonder",
}

ptype_string = { 
	-1: "none",
	0: "mountain",
	1: "hills",
	2: "land",
	3: "ocean",
}

layer_string = {
	0: "terrain/owner  ",
	1: "area           ",
	2: "total value    ",
	3: "aggregate yields",
	4: "value modifier  ", 
	5: "strategic modifier",
	6: "civ modifier   ",
	7: "danger         ",
	8: "fertility      ",
	9: "distance factor",
}

blocked_reason = {
	-1: "cannot found",
	-2: "enemy land",
	-3: "already targeted",
	-4: "enemy unit",
	-5: "too far",
	-6: "not in target area",
}

class Control(object):
	"""A simple control class."""
	def __init__(self,plots,bounds,n_layers):
		"""
		Initialize all of the usual suspects. If the os.environ line is
		included, then the screen will recenter after it is resized.
		"""

		self.plots = plots
		self.bounds = bounds
		self.native_screen_size = ( bounds[0]*DISTANCEX, bounds[1]*DISTANCEY )
		self.n_layers = n_layers
		self.view_mode = 0

		pg.init()
		self.screen = pg.display.set_mode( tuple(i/2 for i in self.native_screen_size), pg.RESIZABLE)
		self.screen_rect = self.screen.get_rect()
		self.image = pg.Surface(self.native_screen_size, flags=pg.SRCALPHA).convert()
		self.image_rect = self.image.get_rect()
		self.clock = pg.time.Clock()
		self.fps = 60.0
		self.done = False
		self.keys = pg.key.get_pressed()

		self.set_caption(self.view_mode)

	def set_caption(self,view_mode):
		pg.display.set_caption( "Fake Map Viewer - %s - space to change" % layer_string.get(view_mode,"unknown layer").strip() )

	def event_loop(self):
		"""
		We are going to catch pygame.VIDEORESIZE events when the user
		changes the size of the window.
		"""
		for event in pg.event.get():
			self.keys = pg.key.get_pressed()
			if event.type == pg.QUIT or self.keys[pg.K_ESCAPE]:
				self.done = True
			elif self.keys[pg.K_SPACE]:
				self.view_mode = (self.view_mode + 1) % self.n_layers
				self.set_caption(self.view_mode)
			elif event.type == pg.VIDEORESIZE:
				self.screen = pg.display.set_mode(event.size, pg.RESIZABLE)
				self.screen_rect = self.screen.get_rect()
			elif event.type == event.type == pg.MOUSEBUTTONDOWN and event.button == 1:
				#scale coords and flip y axis!
				sx = self.native_screen_size[0] / float(self.screen_rect.w)
				sy = self.native_screen_size[1] / float(self.screen_rect.h)
				cx = event.pos[0] * sx
				cy = self.native_screen_size[1] - event.pos[1] * sy
				#find closest plot for the click
				distances = [ p.screen_dist( (cx,cy) ) for p in self.plots ]
				idx,mindist = min(enumerate(distances), key=itemgetter(1))
				#print(event.pos, sx, sy, cx, cy, mindist)
				if mindist<RADIUS_BIG:
					plots[idx].show_details()

	def update(self):
		"""
		Draw all objects on a surface the size of the start window just as
		we would normally draw them on the display surface. Check if the
		current resolution is the same as the original resolution. If so blit
		the image directly to the display; if not, resize the image first.
		"""
		self.image.fill(pg.Color("black"))
		for p in self.plots:
			p.draw(self.image,self.view_mode)
		
		#flip to adjust for the way it's displayed in civ
		self.image = pg.transform.flip( self.image, False, True )
		
		screen_size = self.screen_rect.size
		if screen_size != self.native_screen_size:
			pg.transform.smoothscale(self.image, screen_size, self.screen)
		else:
			self.screen.blit(self.image, (0,0))

	def main_loop(self):
		"""Loop-dee-loop."""
		while not self.done:
			self.event_loop()
			self.update()
			pg.display.update()
			self.clock.tick(self.fps)

class Plot():
	def __init__(self,x,y,terrain,ptype,feature,owner,area,visible,values,hints):
		self.idx_x = x
		self.idx_y = y
		self.x = x*DISTANCEX + (y%2)*ODDOFFSET
		self.y = y*DISTANCEY
		self.terrain = terrain
		self.ptype = ptype
		self.feature = feature
		self.owner = owner
		self.area = area
		self.visible = visible
		self.values = values
		self.hints = hints

	def init(self,scale):
		#determine colors
		assert( len(self.values)==len(scale) )
		gray = [int(255.*v/s) for v,s in zip(self.values,scale)]
		self.color_values = []

		for g in gray:
			if g<=0:
				if self.values[0]<0:
					self.color_values.append( pg.Color(255,0,255) ) #blocked
				else:
					self.color_values.append( pg.Color(-g,0,0) )
			else:
				self.color_values.append( pg.Color(0,+g,0) )

		self.color_owner = pg.Color("#FF00FF")
		self.color_owner.hsva = ( zlib.adler32("%08d"%self.owner) % 360, 100, 100, 100 )

		self.color_area = pg.Color("#FF00FF")
		self.color_area.hsva = ( zlib.adler32("%08d"%self.area) % 360, 100, 100, 100 )

		#draw functions don't take alpha, so a little workaround is needed
		self.surface = pg.Surface( (2*RADIUS_BIG,2*RADIUS_BIG) ).convert()
		if not self.visible:
			self.surface.set_alpha(128)
			
	def screen_dist(self,pos):
		return math.sqrt( (pos[0]-self.x)**2 + (pos[1]-self.y)**2 )

	def show_details(self):
		print("plot (%d,%d) - owner %d\n\tterrain %s, plot %s, feature %s\n\tarea %03d\n" %  \
			(self.idx_x,self.idx_y,self.owner,
			terrain_string.get(self.terrain,"unknown"),
			ptype_string.get(self.ptype,"unknown"),
			feature_string.get(self.feature,"unknown"),
			self.area))
		#special treatment for total
		total = self.values[0]
		if total<0:
			print( "\tblocked because of:  %s\n" % blocked_reason.get(total,"unknown") )
		else:
			print( "\t%s:\t%08d\n" % (layer_string.get(2),total) )
		#rest is automatic			
		print( "\n".join( ("\t%s:\t%08d" % (layer_string.get(i+3,"unknown layer"),v) ) for i,v in enumerate(self.values[1:]) ) ) 
		print( "\n".join( ( ("\t%s\t" % s) for s in self.hints) ) )

	def draw(self, target, view_mode):

		background = pg.Color(0,0,0)
		self.surface.fill( background )
		self.surface.set_colorkey( background )

		if self.ptype==0: #mountain
			sane_terrain = 7 #force mountain
		else: #for land,hill,ocean it's fine as is
			sane_terrain = self.terrain

		pg.draw.circle(self.surface, terrain_color[sane_terrain], (RADIUS_BIG,RADIUS_BIG), RADIUS_BIG, 0)
		
		if view_mode==0:
			if (self.owner!=-1):
				pg.draw.circle(self.surface, self.color_owner, (RADIUS_BIG,RADIUS_BIG), RADIUS_SMALL, 0)
		elif view_mode==1:
			pg.draw.circle(self.surface, self.color_area, (RADIUS_BIG,RADIUS_BIG), RADIUS_SMALL, 0)
		else:
			pg.draw.circle(self.surface, self.color_values[view_mode-2], (RADIUS_BIG,RADIUS_BIG), RADIUS_SMALL, 0)
			
		target.blit( self.surface, (self.x-RADIUS_BIG,self.y-RADIUS_BIG) )

if __name__ == "__main__":
	
	n_properties = 8
	n_values = 8
	
	scale = [1]*n_values
	sizex = 0
	sizey = 0
	
	if not os.path.isfile(sys.argv[1]):
		sys.exit(1)
	
	plots = []
	for line in open(sys.argv[1],"r"):
		data = map(int, line.split(",")[:n_properties+n_values])
		sizex = max(sizex,data[0])
		sizey = max(sizey,data[1])
		data = data[n_properties:n_properties+n_values]
		for i in range(len(data)):
			scale[i] = max( scale[i], abs(data[i]) )
	
	#do this before actually creating the plots (need to initialize pygame)
	run_it = Control(plots,(sizex+1,sizey+1),n_values+2)

	for line in open(sys.argv[1],"r"):
		data = line.split(",")
		x,y,terrain,ptype,feature,owner,area,visible = map(int, data[:n_properties])
		values = map(int, data[n_properties:n_properties+n_values])
		#pad with zeros
		while len(values)<n_values:
			values.append(0)
		#little hack so total value comes first
		values = values[3:] + values[:3]
		hints = data[n_properties+n_values:]
		plots.append( Plot(x,y,terrain,ptype,feature,owner,area,visible,values,hints) )

	#also rearrange the scale
	scale = scale[3:] + scale[:3]

	#set the scale factors	
	for p in plots:
		p.init(scale)
	
	run_it.main_loop()
	pg.quit()
	sys.exit()