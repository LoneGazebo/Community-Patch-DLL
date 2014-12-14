#-------------------------------------------------------------------------------
# Name:		explora
# Purpose:	 visualize civ ai exploration progress
#
# Author:	  ilteroi
#
# Created:	 14-08-2014
#-------------------------------------------------------------------------------

from __future__ import division
import os
import sys
import pygame as pg
import math

from mapviewcontrol import Plot, PlotViewer, terrain_string

layer_string = {
	0: "owner          ",
	1: "score          ",
}

class PlotViewerAnimated(PlotViewer):
	"""A simple control class."""
	def __init__(self,caption,layer_names,plots,bounds,n_layers,fps):

		self.autorun = 0;
		self.framecounter = 0;

		PlotViewer.__init__(self,caption,layer_names,plots,bounds,n_layers,fps)

	def make_caption(self):
		return "%s (cursor: direction, enter: play/pause) - %s (space to change) - frame %d" % (self.caption, self.layer_names.get(self.view_mode,"unknown layer").strip(), self.framecounter )

	def handle_keys(self,event):
		if event.type == pg.KEYUP:
			if event.key == pg.K_RETURN:
				self.autorun = 0 if self.autorun else 1
			elif event.key in (pg.K_RIGHT,pg.K_UP):
				self.autorun = 0
				self.framecounter += 1
				self.set_caption()
			elif event.key in (pg.K_LEFT,pg.K_DOWN):
				self.autorun = 0
				self.framecounter -= 1
				self.set_caption()

		PlotViewer.handle_keys(self,event)

	def handle_click(self,plot_idx):
		self.plots[plot_idx].show_details(self.layer_names,self.framecounter)

	def next_frame(self):
		if self.autorun:
			self.framecounter += 1
			self.set_caption()

	def draw_plots(self):
		for p in self.plots:
			p.draw(self.image,self.view_mode,self.framecounter)

class PlotAnimated(Plot):
	def __init__(self,x,y,terrain):
		
		Plot.__init__(self,x,y,0,terrain,-1)

		self.owner = []
		self.revealed = []
		self.color_owner = []
		self.score = []
		self.scale = []

	def push_data(self, revealed, owner, score, scale):
		self.revealed.append(revealed)
		self.owner.append(owner)
		self.score.append(score)
		self.scale.append(scale)

		if owner!=-1:
			self.color_owner.append( self.make_color("%08d"%owner) )
		else:
			self.color_owner.append(None)

	def extrapolate_data(self, n_samples, scale):
		while len(self.revealed)<n_samples:
			self.revealed.append( self.revealed[-1] if len(self.revealed)>0 else 0 )
			self.owner.append( self.owner[-1] if len(self.owner)>0 else -1 )
			self.score.append( 0 )
			self.scale.append( scale )
			self.color_owner.append( self.color_owner[-1] if len(self.color_owner)>0 else None )

	def show_details(self,layer_description=None,frame_index=0):
		if len(self.score)>0:
			sane_index = frame_index % len(self.score)
			print("plot (%d,%d) - owner %d\n\tterrain %s\n\tscore %d" %  \
				(self.idx_x,self.idx_y,self.owner[sane_index],terrain_string.get(self.terrain,"unknown"),self.score[sane_index]))

	def draw(self, target, view_mode, frame_index):
		
		if len(self.score)>0:
		
			sane_index = frame_index % len(self.score)
			
			if view_mode==0:
				self.color_value = self.color_owner[sane_index]
			else:
				g = int(255*math.sqrt(self.score[sane_index]/self.scale[sane_index]))
				self.color_value = pg.Color(g,g,g)

			self.surface.set_alpha(255 if self.revealed[sane_index] else 128)

		Plot.draw(self, target, view_mode)

def findFiles(path,match):
	for f in os.listdir(path):
		fullpath = os.path.normpath( os.path.join(path,f) )
		if not os.path.isfile( fullpath ):
			continue
		if any( x in f for x in match ):
			yield fullpath

if __name__ == "__main__":
	
	expected_columns = 6

	files = sorted(findFiles(sys.argv[1],[sys.argv[2]]))
	
	if not files:
		print("usage: folder common-name")
		sys.exit(1)
	
	plots = []
	scale_per_turn = []

	lookup = {}
	for f in files:
		scale = []

		for line in open(f,"r"):
			if (line[0]=='#'):
				continue
			data_string = line.split(",")
			data = map(int,data_string[:expected_columns])
	
			while len(scale)<len(data):
				scale.append(1)
			for i in xrange(len(data)):
				scale[i] = max( scale[i], abs(data[i]) )

			x,y,revealed,terrain,owner,score = data
			if not (x,y) in lookup:
				lookup[ (x,y) ] = terrain

		scale_per_turn.append( scale[5] if len(scale)>0 else 1 )
	
	if len(lookup.keys())==0:
		print("no data")
		sys.exit()
	
	#do this before actually creating the plots (need to initialize pygame)
	run_it = PlotViewerAnimated("Animated Viewer", layer_string, plots, (scale[0]+1,scale[1]+1), 2, 5)

	for (x,y) in lookup.keys():
		plots.append( PlotAnimated(x,y,lookup[(x,y)]) )
		lookup[(x,y)] = len(plots)-1
	
	for i,f in enumerate(files):
		for line in open(f,"r"):
			if (line[0]=='#'):
				continue
			data_string = line.split(",")
			data = map(int,data_string[:expected_columns])

			x,y,revealed,terrain,owner,score = data
			plots[ lookup[(x,y)] ].push_data( revealed, owner, score, scale_per_turn[i] )
			
		#fill in data for the plots which were not present in the file
		for p in plots:
			p.extrapolate_data(i+1,scale_per_turn[i])
	
	run_it.main_loop()
	sys.exit()