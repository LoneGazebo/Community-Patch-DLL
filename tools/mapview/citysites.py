#-------------------------------------------------------------------------------
# Name:		plotmap
# Purpose:	 visualize civ settle plot selection
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

from mapviewcontrol import Plot, PlotViewer, terrain_string

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
	0: "owner          ",
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
	0: "not revealed",
	-1: "cannot found",
	-2: "enemy land",
	-3: "already targeted",
	-4: "enemy unit",
	-5: "too far",
	-6: "not in target area",
}

class CityPlot(Plot):
	def __init__(self,x,y,terrain,ptype,feature,owner,area,visible,values,hints,scale):
		Plot.__init__(self,x,y,visible,terrain,owner);

		self.ptype = ptype
		self.feature = feature
		self.area = area
		self.values = values
		self.hints = hints

		if (self.owner!=-1):
			color_owner = pg.Color("#FF00FF")
			color_owner.hsva = ( zlib.adler32("%08d"%self.owner) % 360, 100, 100, 100 )
		else:
			color_owner = None

		color_area = pg.Color("#FF00FF")
		color_area.hsva = ( zlib.adler32("%08d"%self.area) % 360, 100, 100, 100 )

		self.color_values = [color_owner,color_area]

		assert( len(self.values)==len(scale) )
		gray = [int(255.*v/s) for v,s in zip(self.values,scale)]
		for g in gray:
			if g<0:
				if self.values[0]<0:
					self.color_values.append( pg.Color(128,0,128) ) #blocked
				else:
					self.color_values.append( pg.Color(-g,0,0) )
			else:
				self.color_values.append( pg.Color(0,+g,0) )
			
	def show_details(self,layer_description=None):
		print("plot (%d,%d) - owner %d\n\tterrain %s, plot %s, feature %s, area %08d\n" %  \
			(self.idx_x,self.idx_y,self.owner,
			terrain_string.get(self.terrain,"unknown"),
			ptype_string.get(self.ptype,"unknown"),
			feature_string.get(self.feature,"unknown"),
			self.area))

		if (layer_description):
			for pair in zip( self.values, layer_description.values() ):
				print("%s:\t%d" % (pair[1],pair[0]) )

		#special treatment for negative total
		total = self.values[0]
		if total<=0:
			print( "\nblocked because of:  %s\n" % blocked_reason.get(total,"unknown") )
		#show hints
		print( "\n".join( ( ("\t%s\t" % s) for s in self.hints) ) )

	def draw(self, target, view_mode):

		self.color_value = self.color_values[view_mode]
		Plot.draw(self, target, view_mode)

if __name__ == "__main__":
	
	n_properties = 8
	n_values = 8
	expected_columns = n_properties+n_values
	
	if not os.path.isfile(sys.argv[1]):
		print("usage: filename")
		sys.exit(1)
	
	scale = []
	plots = []
	for line in open(sys.argv[1],"r"):
		if (line[0]=='#'):
			continue
		data_string = line.split(",")
		data = map(int,data_string[:expected_columns])

		while len(scale)<len(data):
			scale.append(1)
		for i in xrange(len(data)):
			scale[i] = max( scale[i], abs(data[i]) )

	#do this before actually creating the plots (need to initialize pygame)
	#+2 for owner and area view
	run_it = PlotViewer( "City Sites", layer_string, plots, (scale[0]+1,scale[1]+1), n_values+2 )

	scale = scale[n_properties:n_properties+n_values]
	scale = scale[3:] + scale[:3]

	for line in open(sys.argv[1],"r"):
		data_string = line.split(",")
		data = map(int,data_string[:expected_columns])
		#pad with zeros
		while len(data)<expected_columns:
			data.append(0)

		x,y,terrain,ptype,feature,owner,area,visible = data[:n_properties]
		values = data[n_properties:n_properties+n_values]
		#little hack so total value comes first
		values = values[3:] + values[:3]
		hints = data[n_properties+n_values:]

		plots.append( CityPlot(x,y,terrain,ptype,feature,owner,area,visible,values,hints,scale) )
	
	run_it.main_loop()
	sys.exit()