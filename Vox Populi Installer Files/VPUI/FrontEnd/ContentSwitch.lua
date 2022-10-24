-- Table of Vox Populi tips
local g_VPTipTable = {
"Great Person yields are determined at the moment they are created.",
"Local Unhappiness from Needs cannot exceed a city's Population.",
"A large standing army costs less than losing half your empire to an opportunistic neighbor. The AI knows exactly how big your military is, so make sure it's never small.",
"Roads and Forts can help immensely in both offensive and defensive wars, but be wary of their maintenance cost.",
"To deal with a snowballing civilization, isolate it diplomatically and weaken it one step at a time. Light a thousand fires.",
"Against an aggressive warmonger, value defensive positions over good economic output for your frontier cities. A lost city will grant you no reward.",
"Tourism is more than a victory condition: it also helps your Trade Routes, preserves the buildings and population of conquered cities, and allows spies to set up more quickly.",
"Internal Trade Routes can be reliable ways to preserve your economy when Barbarians and aggressive neighbors roam around.",
"Never leave a Settler or Great Person unguarded.",
"Recon units are most vulnerable near coastal or mountainous tiles, where Barbarians can often surprise and trap them.",
"Founding a religion isn't crucial if you know you can take the Holy City of your neighbor.",
"Even if you aren't aiming for a Diplomatic Victory, City-State Embassies will prevent you from being left at the mercy of more influential civilizations after the World Congress is founded.",
"Offense is often the best defense, particularly against warmongers whose power spikes later in the game.",
"Combat Strength allows you to win battles. Healing allows you to win wars.",
"If you don't have any Gold and your income is negative, your Science output will be reduced and some Units will be disbanded.",
"The cost of a Technology is lowered for each other civilization that has researched it.",
"Always check your Faith and Gold stockpiles before clicking 'Next Turn.' It's easy to forget you were saving them for a unit.",
"For each 1 damage taken, Units lose 0.3% Combat Strength when attacking (or defending against melee attacks). Soften your enemy's units with ranged attacks before attacking with your melee units.",
"Melee Units garrisoning a city, Fort, or Citadel don't exit their fortification if they kill a Unit. You can use this to keep your defenses from overextending.",
"In each city, Unhappiness from Needs is increased with the number of non-puppeted cities you own, via the Empire Modifier. Don't expand your empire without sizable surplus happiness.",
"Cities, Forts, and Citadels which are adjacent to water create a canal for your Naval Units and sea Trade Routes to pass through.",
"Missionaries lose strength if they end their turn in another civilization's territory, unless you have an Open Borders treaty. Try to end their turn just outside your rival's borders.",
"When you expend a Great Writer, make sure you do it in a city where you want to grab new tiles from border growth.",
"Issue move orders with SHIFT+RCLICK to create waypoints.",
"CTRL+RCLICK issues a 'Move and Act' order. Workers repair tiles and improve resources, Missionaries spread their religion, Diplomatic Units spread influence, Archaeologists dig, and Combat Units go into Fortify or Alert mode.",
"When voting for World Congress resolutions, right-click to assign all delegates and middle-click or shift-click to assign ten.",
"SHIFT+LCLICK on the Next Turn button to issue a skip turn order to all idle Units. This can be used if you're experiencing the glitch that prevents progressing to the next turn.",
-- "CTRL+RCLICK on a notification to quickly clear all notifications.",
"Specialists can be locked in Vox Populi by clicking on an empty Specialist slot in the city screen.",
}

function getVPTip()
	math.randomseed(os.time()) -- random initialize
	math.random(); math.random(); math.random() -- warming up

	value = math.random(#g_VPTipTable)
	return g_VPTipTable[value]
end

-------------------------------------------------
-- ContentSwitch
-------------------------------------------------

----------------------------------------------------------------  
----------------------------------------------------------------        
function OnShowHide( isHide, isInit )
	
	if(not isHide) then
		
	end	
	Controls.Tips:SetText("Tip: "..getVPTip())
	local contentsize = Controls.Tips:GetSize().y
	if contentsize > 64 then
		Controls.ContentSwitchGrid:SetSizeY( 220 + contentsize )
	end
end
ContextPtr:SetShowHideHandler( OnShowHide );
