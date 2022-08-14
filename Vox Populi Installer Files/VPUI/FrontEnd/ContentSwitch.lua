-- Table of Vox Populi tips
local g_VPTipTable = {
"Great Person yields are determined at the moment they are created.", 
"Unhappiness from needs caps at city population.", 
"A large standing army costs less than losing half your empire to an opportunistic neighbor. The AI knows exactly how big your military is, so make sure it's never small.", 
"Roads and forts can help immensely in both offensive and defensive wars, but be wary of the maintenance cost.",
"To deal with a snowballing civilization, isolate it diplomatically and weaken it one step at a time. Light a thousand fires.",
"Against an aggressive warmonger, value defensive positions for your frontier cities instead of good economic output: a lost city will grant you no reward.",
"Tourism is more than a victory condition: it also helps your trade routes and preserves the buildings and population of conquered cities.",
"Internal trade routes can be reliable ways to preserve your economy when barbarians and aggressive neighbors roam around.",
"Never leave a settler or great person unguarded.",
"Your recon units are most vulnerable near coastal or mountainous tiles, where barbarians can often surprise and trap them.",
"Founding a religion isn't crucial if you know you can take the Holy City of your neighbor.",
"Even if you aren't aiming for a diplomatic victory, Great Diplomats are critical to not to be left at the mercy of more influential civilizations after the World Congress is founded.",
"Offense is often the best defense, particularly against warmongers whose power spikes later in the game.",
"Combat strength allows you to win battles, healing allows you to win wars.",
 "If you don't have any gold and have negative income, your science will be reduced, and some units will be disbanded.",
 "Technologies have their cost lowered when other players have researched them.",
 "Always check your faith and gold stockpile before clicking 'Next Turn.' It's easy to forget you were saving them for a unit.",
 "Units suffer a 0.3% combat strength penalty in melee combat and when attacking for every 1 damage taken. Soften your enemy's units with ranged attacks before attacking with your melee units.",
 "Melee units garrisoning a city, fort, or citadel don't move to the tile of the unit they kill when ordered to attack. You can use those to keep your units from overextending.", 
"Unhappiness from needs grows rather exponentially with the number of non-puppeted cities you own. Don't expand your empire without sizable surplus happiness.",
 "Cities, forts, and citadels on a coastal tile count as a canal for your naval units and sea trade routes.",
 "Missionaries lose strength if they end a turn on a tile belonging to another civilization that you do not have Open Borders. Try to end their turn just outside their borders.",
 "When you expend a Great Writer, make sure you do it in a city where you want to grab new tiles from border growth.",
"Issue move orders with SHIFT+RCLICK to create waypoints.",
"CTRL+RCLICK issues a 'Move and Act' order. 'Act' depends on the unit: Workers repair tiles and improve resources, missionaries spread religion, diplomatic units spread influence, archaeologists dig, and combat units go into fortify or alert mode.",
"When voting for resolutions, right-click to assign all delegates and middle click to assign ten.",
"SHIFT+LCLICK on the Next Turn button to issue a skip turn order on all idle units. This can be used if you're experiencing the glitch that prevents progressing to the next turn.",
"CTRL+RCLICK on a notification to quickly clear all notifications.",
"Specialists can be locked in Vox Populi by clicking on an empty specialist slot.",
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
