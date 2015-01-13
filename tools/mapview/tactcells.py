#-------------------------------------------------------------------------------
# Name:		tactcells
# Purpose:	 visualize civ tactical map
#
# Author:	  ilteroi
#
# Created:	 06-12-2014
#-------------------------------------------------------------------------------

import os
import sys

from mapviewcontrol import Plot, PlotWithScore, PlotViewer

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

ai_TacticalTargetType = (
	"AI_TACTICAL_TARGET_NONE",
	"CITY",
	"BARBARIAN_CAMP",
	"IMPROVEMENT",
	"BLOCKADE_RESOURCE_POINT",
	"LOW_PRIORITY_UNIT",    # Can't attack one of our cities
	"MEDIUM_PRIORITY_UNIT", # Can damage one of our cities
	"HIGH_PRIORITY_UNIT",   # Can contribute to capturing one of our cities
	"CITY_TO_DEFEND",
	"IMPROVEMENT_TO_DEFEND",
	"DEFENSIVE_BASTION",
	"ANCIENT_RUINS",
	"BOMBARDMENT_ZONE",     # Used for naval bombardment operation
	"EMBARKED_MILITARY_UNIT",
	"EMBARKED_CIVILIAN",
	"VERY_HIGH_PRIORITY_CIVILIAN",
	"HIGH_PRIORITY_CIVILIAN",
	"MEDIUM_PRIORITY_CIVILIAN",
	"LOW_PRIORITY_CIVILIAN",
	"AI_TACTICAL_TARGET_TRADE_UNIT_SEA",
	"TRADE_UNIT_LAND",
	"TRADE_UNIT_SEA_PLOT", # Used for idle unit moves to plunder trade routes that go through our territory
	"TRADE_UNIT_LAND_PLOT", 
	"CITADEL",
	"IMPROVEMENT_RESOURCE",
)

layer_string = {
	0: "defense 	",
	1: "target type	",
	2: "deployment	",
	3: "safe		",
	4: "attacked	", 
	5: "in range	",
	6: "flank bonus	",
}

class TactCell(PlotWithScore):

	def show_details(self):
		Plot.show_details(self)

		for pair in zip( self.values, layer_string.values() ):
			print("%s:\t%d" % (pair[1],pair[0]) )
			
		print("target type\t: %s" % ai_TacticalTargetType[self.values[1]] )

	def draw(self, target, view_mode):
		
		if view_mode==1 and self.values[1]>0: #special for target type
			self.draw_indexed_color(target,view_mode)
		else:
			self.draw_interpolated_color(target,view_mode)

if __name__ == "__main__":
	
	expected_columns = 10
	
	if not os.path.isfile(sys.argv[1]):
		print("usage: filename")
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
		plots.append( TactCell(data[0],data[1],data[2],data[3],data[4],data[5:],scale[5:]) )
	
	run_it.main_loop()
	sys.exit()