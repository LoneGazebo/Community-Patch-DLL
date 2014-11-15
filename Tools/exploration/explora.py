#-------------------------------------------------------------------------------
# Name:		explora
# Purpose:	 visualize civ ai exploration progress
#
# Author:	  ilteroi
#
# Created:	 14-08-2014
#-------------------------------------------------------------------------------

import os
import sys
import pygame as pg
import math
import zlib
from operator import itemgetter

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

class Control(object):
	"""A simple control class."""
	def __init__(self,plots,bounds):
		"""
		Initialize all of the usual suspects. If the os.environ line is
		included, then the screen will recenter after it is resized.
		"""

		self.plots = plots
		self.bounds = bounds
		self.native_screen_size = ( bounds[0]*DISTANCEX, bounds[1]*DISTANCEY )
		self.view_index = 0
		self.direction = 1;

		pg.init()
		self.screen = pg.display.set_mode( tuple(i/2 for i in self.native_screen_size), pg.RESIZABLE)
		self.screen_rect = self.screen.get_rect()
		self.image = pg.Surface(self.native_screen_size, flags=pg.SRCALPHA).convert()
		self.image_rect = self.image.get_rect()
		self.clock = pg.time.Clock()
		self.fps = 60.0
		self.done = False
		self.keys = pg.key.get_pressed()

		self.set_caption()

	def set_caption(self):
		pg.display.set_caption( "Exploration Viewer - %03d - cursor: direction, space: play/pause" % self.view_index )

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
				self.direction = abs( self.direction - 1)
			elif any( self.keys[k] for k in (pg.K_RIGHT,pg.K_LEFT,pg.K_UP,pg.K_DOWN) ):
				self.direction *= -1
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
			p.draw(self.image,self.view_index)
		
		#flip to adjust for the way it's displayed in civ
		self.image = pg.transform.flip( self.image, False, True )
		
		screen_size = self.screen_rect.size
		if screen_size != self.native_screen_size:
			pg.transform.smoothscale(self.image, screen_size, self.screen)
		else:
			self.screen.blit(self.image, (0,0))

	def main_loop(self):
		"""Loop-dee-loop."""
		counter = 0
		while not self.done:
			self.event_loop()
			self.update()
			pg.display.update()
			self.clock.tick(self.fps)
			counter += 1
			if counter%int(5) == 0:
				counter = 0
				self.view_index += 1
				self.set_caption()

class Plot():
	def __init__(self,x,y,terrain,ptype,feature,area):
		self.idx_x = x
		self.idx_y = y
		self.x = x*DISTANCEX + (y%2)*ODDOFFSET
		self.y = y*DISTANCEY
		self.terrain = terrain
		self.ptype = ptype
		self.feature = feature
		self.owner = owner
		self.area = area
		self.owner = []
		self.revealed = []
		self.color_owner = []

		self.color_area = pg.Color("#FF00FF")
		self.color_area.hsva = ( zlib.adler32("%08d"%self.area) % 360, 100, 100, 100 )

		#draw functions don't take alpha, so a little workaround is needed
		self.surface = pg.Surface( (2*RADIUS_BIG,2*RADIUS_BIG) ).convert()

	def push_data(self,revealed,owner):
		self.revealed.append(revealed)
		self.owner.append(owner)

		self.color_owner.append(pg.Color("#FF00FF"))
		self.color_owner[-1].hsva = ( zlib.adler32("%08d"%self.owner[-1]) % 360, 100, 100, 100 )
			
	def screen_dist(self,pos):
		return math.sqrt( (pos[0]-self.x)**2 + (pos[1]-self.y)**2 )

	def show_details(self):
		sane_index = index % len(self.owner)
		owner = self.owner[sane_index]

		print("plot (%d,%d) - owner %d\n\tterrain %s, plot %s, feature %s, area %08d\n" %  \
			(self.idx_x,self.idx_y,owner,
			terrain_string.get(self.terrain,"unknown"),
			ptype_string.get(self.ptype,"unknown"),
			feature_string.get(self.feature,"unknown"),
			self.area))

	def draw(self, target, index):

		sane_index = index % len(self.revealed)

		background = pg.Color(0,0,0)
		self.surface.fill( background )
		self.surface.set_colorkey( background )

		if self.ptype==0: #mountain
			sane_terrain = 7 #force mountain
		else: #for land,hill,ocean it's fine as is
			sane_terrain = self.terrain

		pg.draw.circle(self.surface, terrain_color[sane_terrain], (RADIUS_BIG,RADIUS_BIG), RADIUS_BIG, 0)
		
		if (self.owner[sane_index] !=-1 ):
			pg.draw.circle(self.surface, self.color_owner[sane_index], (RADIUS_BIG,RADIUS_BIG), RADIUS_SMALL, 0)

		if self.revealed[sane_index]:
			self.surface.set_alpha(255)
		else:
			self.surface.set_alpha(64)
			
		target.blit( self.surface, (self.x-RADIUS_BIG,self.y-RADIUS_BIG) )

def findFiles(path,match):
	for f in os.listdir(path):
		if not os.path.isfile(f):
			continue
		if any( x in f for x in match ):
			yield f

if __name__ == "__main__":
	
	n_properties = 8
	
	sizex = 0
	sizey = 0

	files = sorted(findFiles(".",[ sys.argv[1]]))
	
	if not files:
		sys.exit(1)
	
	plots = []
	for line in open(files[0],"r"):
		data = map(int, line.split(",")[:n_properties])
		sizex = max(sizex,data[0])
		sizey = max(sizey,data[1])
	
	#do this before actually creating the plots (need to initialize pygame)
	run_it = Control(plots,(sizex+1,sizey+1))

	lookup = {}
	for line in open(files[0],"r"):
		data = map(int, line.split(",")[:n_properties])
		x,y,terrain,ptype,feature,owner,area,revealed = map(int, data[:n_properties])
		plots.append( Plot(x,y,terrain,ptype,feature,area) )
		lookup[ (x,y) ] = len(plots)-1
	
	for f in files:
		for line in open(f,"r"):
			data = line.split(",")
			x,y,terrain,ptype,feature,owner,area,revealed = map(int, data[:n_properties])
			plots[ lookup[(x,y)] ].push_data(revealed,owner)
	
	run_it.main_loop()
	pg.quit()
	sys.exit()