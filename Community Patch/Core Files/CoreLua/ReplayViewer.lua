include("InstanceManager");
include("IconSupport");
include("GameCalendarUtilities.lua");

-------------------------------------------------------------------
-- Color Utilities
-------------------------------------------------------------------
local function ColorIsWhite(color)
	return color.Red == 1 and color.Blue == 1 and color.Green == 1;
end

local function ColorIsBlack(color)
	return color.Red == 0 and color.Blue == 0 and color.Green == 0;
end

local function GetBrightness(c)
	local r = c.Red;
	if(r <= 0.03928) then
		r = r / 12.92;
	else
		r = ((r + 0.055) / 1.055)^2.4;
	end
	
	r = r * 0.2126;
	
	local g = c.Green
	if(g <= 0.03928) then
		g = g / 12.92;
	else
		g = ((g + 0.055) / 1.055)^2.4;
	end
	
	g = g * 0.7152;
	
	local b = c.Blue;
	if(b <= 0.03928) then
		b = b / 12.92;
	else
		b = ((b + 0.055) / 1.055)^2.4;
	end
	
	b = b * 0.0722;
	
	return r + g + b;

end

local function HSLFromColor(r,g,b)
		
	local h,s,l;
	
	local minV = math.min(r,g,b);
	local maxV = math.max(r,g,b);

	l = (maxV + minV) / 2;

	if(minV == maxV) then
		h = 0;
		s = 0;
	else
		if(l < 0.5) then
			s = (maxV - minV) / (maxV + minV);
		else
			s = (maxV - minV) / (2 - maxV - minV); 
		end
		
		if(r == maxV) then
			h = (g - b)/(maxV - minV);
		elseif(g == maxV) then
			h = 2 + (b - r)/(maxV - minV);
		elseif(b == maxV) then
			h = 4 + (r - g)/(maxV - minV);
		end
	end
	
	return h,s,l;
end

local function ColorFromHSL(hue, sat, lum)
	local function Clamp(value, minValue, maxValue)
		if (value < minValue) then return minValue;
		elseif(value > maxValue) then return maxValue;
		else return value;
		end
	end

	hue = Clamp(hue, 0, 1.0);
	sat = Clamp(sat, 0, 1.0);
	lum = Clamp(lum, 0, 1.0);

    local r = 0;
    local g = 0
    local b = 0;
  
  
    if (lum == 0) then
        r = 0;
        g = 0;
        b = 0;
        
    elseif (sat == 0) then
        r = lum;
        g = lum;
        b = lum;
        
	else
        local temp1;
        if(lum < 0.5) then 
			temp1 = lum * (1 + sat) 
		else 
			temp1 = lum + sat - lum * sat; 
		end
        
        local temp2 = 2.0 * lum - temp1;

        local t3 = { hue + 1 / 3, hue, hue - 1 / 3 };
        local clr = {0, 0, 0};
        for i, t in ipairs(t3) do
        
            if (t < 0) then
                t = t + 1;
            end
            
            if (t > 1) then
                t = t - 1;
            end

            if (6 * t < 1) then
                clr[i] = temp2 + (temp1 - temp2) * t * 6;
            elseif (2 * t < 1) then
                clr[i] = temp1;
            elseif (3 * t < 2) then
                clr[i] = (temp2 + (temp1 - temp2) * ((2 / 3) - t) * 6);
            else
                clr[i] = temp2;
            end
        end
        
        r = clr[1];
        g = clr[2];
        b = clr[3];
    end
    
    return {Red = r, Green = g, Blue = b, Alpha = 1};
end

---------------------------------------------------------------
-- Globals
----------------------------------------------------------------
g_bIsEndGame = (ContextPtr:GetID() == "EndGameReplay"); 

g_GraphVerticalMarkers = {
	Controls.VerticalLabel1,
	Controls.VerticalLabel2,
	Controls.VerticalLabel3,
	Controls.VerticalLabel4,
	Controls.VerticalLabel5
};

g_GraphHorizontalMarkers = {
	Controls.HorizontalLabel1,
	Controls.HorizontalLabel2,
	Controls.HorizontalLabel3,
	Controls.HorizontalLabel4,
	Controls.HorizontalLabel5
};

g_ReplayMessageInstanceManager = InstanceManager:new("ReplayMessageInstance", "Base", Controls.ReplayMessageStack);
g_GraphLegendInstanceManager = InstanceManager:new("GraphLegendInstance", "GraphLegend", Controls.GraphLegendStack);
g_LineSegmentInstanceManager = InstanceManager:new("GraphLineInstance","LineSegment", Controls.GraphCanvas);

g_ReplayInfo = {};

Panels = {
	{
		Title = Locale.Lookup("TXT_KEY_REPLAY_VIEWER_MESSAGES_TITLE"),
		Tooltip = Locale.Lookup("TXT_KEY_REPLAY_VIEWER_MESSAGES_TT"),
		Panel = Controls.MessagesPanel,
		Refresh = function(panel)
		
			-- First, determine the best player colors to use.
			-- Certain players use dark player colors and as such we can't use that as the text color on 
			-- a black background :(				
			local function MakeColorVector(c) 
				return {
					x = c.Red,
					y = c.Green,
					z = c.Blue,
					w = 1.0
				};
			end
			
			local defaultColor = {
				Red = 1.0,
				Green = 1.0,
				Blue = 200/255,
			};
			
			local defaultColorVector = MakeColorVector(defaultColor);
					
			local function DetermineBestMessageColor(primaryColor, secondaryColor)
						
				local usedColor = secondaryColor;
				if(ColorIsWhite(usedColor) or ColorIsBlack(usedColor)) then
					usedColor = primaryColor;
				end
				
				local h,s,l = HSLFromColor(usedColor.Red, usedColor.Green, usedColor.Blue);
				
				print(l);
				if(l < 0.4) then
					--usedColor = ColorFromHSL(h, s, 0.5);
					--print("CONVERT");
					--print(usedColor.Red);
					--print(usedColor.Blue);
					--print(usedColor.Green);
				end
				
				
				return usedColor;
			end		
					
			local playerColorVectors = {};
			for i, player in ipairs(g_ReplayInfo.PlayerInfo) do
				local playerColor = GameInfo.PlayerColors[player.PlayerColor];
				if(playerColor ~= nil) then
					
					local primaryColor = GameInfo.Colors[playerColor.PrimaryColor];
					local secondaryColor = GameInfo.Colors[playerColor.SecondaryColor];
				
					local color = DetermineBestMessageColor(primaryColor, secondaryColor);
					playerColorVectors[i] = MakeColorVector(color);
					
				end	
			end		

			g_ReplayMessageInstanceManager:ResetInstances();
			for i,message in ipairs(g_ReplayInfo.Messages) do
				if(message ~= nil and message.Text ~= nil and #message.Text > 0) then
					local messageInstance = g_ReplayMessageInstanceManager:GetInstance();
				
					messageInstance.MessageText2:SetHide(true);
				
					local text = tostring(message.Turn) .. " - " .. message.Text;
										
					if(message.Player ~= nil and message.Player > -1) then					
						local playerInfo = g_ReplayInfo.PlayerInfo[message.Player];
						if(playerInfo ~= nil) then
							local colorVector = playerColorVectors[message.Player];							
							
								local h,s,l = HSLFromColor(colorVector.x, colorVector.y, colorVector.z);
				
							--	print(l);
								if(l < 0.4) then
									--usedColor = ColorFromHSL(h, s, 0.5);
									--print("CONVERT");
									--print(usedColor.Red);
									--print(usedColor.Blue);
									--print(usedColor.Green);
									messageInstance.MessageText2:SetHide(false);
								end
											
							
							messageInstance.MessageText:SetColor(colorVector, 0);						
						else
							messageInstance.MessageText:SetColor(defaultColorVector, 0);
						
						end
					end
					
					messageInstance.MessageText:SetText(text);
					messageInstance.MessageText2:SetText(text);
					
					local baseWidth, baseHeight = messageInstance.Base:GetSizeVal();
					local msgWidth, msgHeight = messageInstance.MessageText:GetSizeVal();
					
					messageInstance.LineLeft:SetHide(false);
					messageInstance.LineRight:SetHide(false);
			
					
					local newHeight = msgHeight + 10;	
					messageInstance.Base:SetSizeVal(baseWidth, newHeight);
				end				
			end
			
			-- Add empty text to padd the bottom.
			local bottomPadding = g_ReplayMessageInstanceManager:GetInstance();
			bottomPadding.MessageText:SetText(" ");
			
			bottomPadding.LineLeft:SetHide(true);
			bottomPadding.LineRight:SetHide(true);
			
			Controls.ReplayMessageStack:CalculateSize();
			Controls.ReplayMessageStack:ReprocessAnchoring();
			Controls.ReplayMessageScrollPanel:CalculateInternalSize();
		end
	},
	
	{
		Title = Locale.Lookup("TXT_KEY_REPLAY_VIEWER_GRAPHS_TITLE"),
		Tooltip = Locale.Lookup("TXT_KEY_REPLAY_VIEWER_GRAPHS_TT"),
		Panel = Controls.GraphsPanel,
		CurrentGraphDataSetIndex = nil,		
		SegmentsByPlayer = {},
		GraphLegendsByPlayer = {},
		PlayerGraphColors = {},
		
		
		-- This method will pad the horizontal values if they are too small so that they show up correctly
		PadHorizontalValues = function(minTurn, maxTurn)
			local range = maxTurn - minTurn;
			local numHorizontalMarkers = #g_GraphHorizontalMarkers - 1;
			if(range < numHorizontalMarkers) then
				return (minTurn - (numHorizontalMarkers - range)), maxTurn;
			else
				return minTurn, maxTurn;
			end
		end,
		
		CreateUpdateHandler = function(panel)

			--Calculate the graph's bounding rect in absolute coords (kinda a hack since this sasumes the con)
			local screenX, screenY = UIManager:GetScreenSizeVal();
			
			local xAbsoluteOffset = (screenX * 0.5) - (Controls.MainPanel:GetSizeX() * 0.5);
			xAbsoluteOffset = xAbsoluteOffset + Controls.GraphsPanel:GetOffsetX() + Controls.GraphDisplay:GetOffsetX();
			
			local yAbsoluteOffset = (screenY * 0.5) - (Controls.MainPanel:GetSizeY() * 0.5);
			yAbsoluteOffset = yAbsoluteOffset + Controls.GraphsPanel:GetOffsetY() + Controls.GraphDisplay:GetOffsetY();

			local graphsPanel = Controls.GraphsPanel;

			local graphDisplayWidth = Controls.GraphDisplay:GetSizeX();
			local graphDisplayHeight = Controls.GraphDisplay:GetSizeY();
			
			local horizontalMouseGuideEnd = graphDisplayWidth + 8;
			local verticalMouseGuideEnd = graphDisplayHeight + 8;
			
			local horizontalMouseCrosshair = Controls.HorizontalMouseCrosshair;
			local verticalMouseCrosshair = Controls.VerticalMouseCrosshair;
			
			local verticalMouseGuide = Controls.VerticalMouseGuide;
			local horizontalMouseGuide = Controls.HorizontalMouseGuide;
			
			local uiManager = UIManager;
			
			local GetMousePos = function()
				return uiManager:GetMousePos();
			end
			
			local DrawCursor = function(x, y) 
				horizontalMouseCrosshair:SetStartVal(0, y); 
				horizontalMouseCrosshair:SetEndVal(graphDisplayWidth, y);
		 
				verticalMouseCrosshair:SetStartVal(x, 0);
				verticalMouseCrosshair:SetEndVal(x, graphDisplayHeight);
			
				horizontalMouseGuide:SetStartVal(graphDisplayWidth, y); 
				horizontalMouseGuide:SetEndVal(horizontalMouseGuideEnd, y);
		 
				verticalMouseGuide:SetStartVal(x, graphDisplayHeight);
				verticalMouseGuide:SetEndVal(x, verticalMouseGuideEnd);
			end	
			
			local ToggleHideLines = function(bHide)
				horizontalMouseCrosshair:SetHide(bHide);
				verticalMouseCrosshair:SetHide(bHide);
				
				horizontalMouseGuide:SetHide(bHide);
				verticalMouseGuide:SetHide(bHide);
			end
			
			return function()
				if(not graphsPanel:IsHidden()) then
					local x,y = GetMousePos();
					
					local xRelative = x - xAbsoluteOffset;
					local yRelative = y - yAbsoluteOffset;
					
					if(xRelative > 0 and xRelative < graphDisplayWidth and
					   yRelative > 0 and yRelative < graphDisplayHeight) then
						DrawCursor(xRelative, yRelative);
						ToggleHideLines(false);
					else
						ToggleHideLines(true);
					end
				else
					ToggleHideLines(true);
				end
			end
		end,
		
		DrawGraph = function(panel)
				
			local graphWidth, graphHeight = Controls.GraphCanvas:GetSizeVal();
			local initialTurn = g_ReplayInfo.InitialTurn;
			local finalTurn = g_ReplayInfo.FinalTurn;
			
			local playerInfos = g_ReplayInfo.PlayerInfo;
			local indexName = panel.CurrentGraphDataSetIndex;
			
			g_LineSegmentInstanceManager:ResetInstances();
			panel.SegmentsByPlayer = {};
		
			local function RefreshVerticalScales(minValue, maxValue)
				
				local numVerticalMarkers = #g_GraphVerticalMarkers;
				
				local increments = (maxValue - minValue) / (numVerticalMarkers - 1);
							
				for i,v in ipairs(g_GraphVerticalMarkers) do
					if(i == 1) then
						v:SetText(minValue);
					elseif(i == numVerticalMarkers) then
						v:SetText(maxValue);
					else
						local scoreIncrement = math.floor((i - 1) * increments) + minValue;
						v:SetText(scoreIncrement);
					end
				end	
			end
						
			local numSegments = 100;
			local numVerts = numSegments + 1;
			
			function DrawGraph(playerInfo, color, scoreType, graphWidth, minX, maxX, yScale, yOffset)
									
				local xScale = graphWidth / (maxX - minX);
				
				local lineSegments = {};
				
				function CreateLineSegment(startX, startY)
					local lineSegment = g_LineSegmentInstanceManager:GetInstance();
					table.insert(lineSegments, lineSegment);
					
					lineSegment.LineSegment:SetStartVal(startX, startY);
					lineSegment.LineSegment:SetColorVal(color.Red, color.Green, color.Blue, color.Alpha);
					lineSegment.LineSegment:SetHide(false);	
					
					return lineSegment;
				end
				
				-- First, gather only the data we require and adjust the turn.
				local turnData = {};
				for i, v in pairs(playerInfo.Scores) do
					local value = v[scoreType];
					if(value ~= nil and i <= maxX and i >= minX) then
						turnData[i - minX] = value;			
					end
				end
								
				-- Second, reduce set down to fixed increments by lerping
				function lerp(y1,y2,x)
					return y1 + (y2-y1)*x;
				end
				
					
				local incrementData = {}
				incrementData[0] = turnData[0];
				
				for i = 1, numVerts, 1 do
					local increment = (i - 1)/(numVerts - 1);
					
					local turn = (increment * (maxX - minX));
					local beforeTurn = math.floor(turn);
					local afterTurn = math.floor(turn);
					
					local beforeValue = turnData[beforeTurn];
					local afterValue = turnData[afterTurn];
					
					if(beforeValue ~= nil and afterValue ~= nil) then
						incrementData[i] = lerp(beforeValue, afterValue, turn - beforeTurn);
					end	
				end
				
				-- Third, draw line segments!				
				for i = 1, numSegments, 1 do
					local y1 = incrementData[i];
					local y2 = incrementData[i+1];
					
					if(y1 ~= nil and y2 ~= nil) then
					
						local startX = math.floor(((i - 1)/(numVerts-1)) * graphWidth);
						local startY = math.floor((y1 - yOffset) * yScale)
					
						startY = graphHeight - startY;
					
						local lineSegment = CreateLineSegment(startX, startY);
						
						local endX = math.floor((i/(numVerts-1)) * graphWidth);
						local endY = math.floor((y2 - yOffset) * yScale);		
					
						-- inverse Y
						endY = graphHeight - endY;
					
						lineSegment.LineSegment:SetEndVal(endX, endY);
					end
				end
								
				return lineSegments;
			end	
			
			-- Determine the maximum score for all players
			local maxScore = 0;
			local minScore = nil;
			for i,v in ipairs(playerInfos) do
				for turn, score in pairs(v.Scores) do
					local s = score[indexName];
					if(s) then
						if(s > maxScore) then
							maxScore = s;
						end
					
						if(minScore == nil or s < minScore) then
							minScore = s;
						end
					end
				end
			end
			
			if(minScore ~= nil) then	-- this usually means that there were no values for that dataset.
				
				Controls.NoGraphData:SetHide(true);
				
				--We want a value that is nicely divisible by the number of vertical markers
				local numSegments = #g_GraphVerticalMarkers - 1;
				local range = maxScore - minScore;
				local newRange = range + numSegments - range % numSegments;		
								
				maxScore = newRange + minScore;
				
				local YScale = graphHeight / (maxScore - minScore);
				
				RefreshVerticalScales(minScore, maxScore);
				
				local minTurn, maxTurn = panel.PadHorizontalValues(initialTurn, finalTurn);
				
				for i,v in ipairs(playerInfos) do
					local graphLegend = panel.GraphLegendsByPlayer[i];
					local isHidden = not graphLegend.ShowHide:IsChecked();
				
					panel.SegmentsByPlayer[i] = DrawGraph(v, panel.PlayerGraphColors[i], indexName, graphWidth, minTurn, maxTurn, YScale, minScore); 
					if(isHidden == true) then
						for _, instance in ipairs(panel.SegmentsByPlayer[i]) do
							instance.LineSegment:SetHide(true);					
						end
					end
				end
				
				
			else
				for i,v in ipairs(g_GraphVerticalMarkers) do
					Controls.NoGraphData:SetHide(false);
					v:SetText("");
				end
			end
		end,
		
		Refresh = function(panel) 
		
			local graphWidth, graphHeight = Controls.GraphCanvas:GetSizeVal();
			local initialTurn = g_ReplayInfo.InitialTurn;
			local finalTurn = g_ReplayInfo.FinalTurn;
			local startYear = g_ReplayInfo.StartYear;
			local calendarType = GameInfo.Calendars[g_ReplayInfo.Calendar].Type;
			local gameSpeedType = GameInfo.GameSpeeds[g_ReplayInfo.GameSpeed].Type;
			
			function RefreshHorizontalScales()
				
				local minTurn, maxTurn = panel.PadHorizontalValues(initialTurn, finalTurn);
				
				local turnIncrements = (maxTurn - minTurn) / (#g_GraphHorizontalMarkers - 1);
								
				for i,v in ipairs(g_GraphHorizontalMarkers) do
					if(i == #g_GraphHorizontalMarkers) then
						v:SetText(GetShortDateString(finalTurn, calendarType, gameSpeedType, startYear));
					else
						local turnIncrement = math.floor((i - 1) * turnIncrements) + minTurn;
						if(turnIncrement < initialTurn) then
							v:SetText("");	
						else
							v:SetText(GetShortDateString(turnIncrement, calendarType, gameSpeedType, startYear));
						end
					end
				end
			end
			
			function RefreshCivilizations()
			
				local colorDistances = {};
				local pow = math.pow;
				function ColorDistance(color1, color2)
					local distance = nil;
					local colorDistanceTable = colorDistances[color1.Type];
					if(colorDistanceTable == nil) then
						colorDistanceTable = {
							[color1.Type] = 0;	--We can assume the distance of a color to itself is 0.
						};
						
						colorDistances[color1.Type] = colorDistanceTable
					end
					
					local distance = colorDistanceTable[color2.Type];
					if(distance == nil) then
						local r2 = pow((color1.Red - color2.Red)*255, 2);
						local g2 = pow((color1.Green - color2.Green)*255, 2);
						local b2 = pow((color1.Blue - color2.Blue)*255, 2);	
						
						distance = r2 + g2 + b2;
						colorDistanceTable[color2.Type] = distance;
					end
					
					return distance;
				end
				
				local colorNotUnique = {};
				local colorBlack = {Type = "COLOR_BLACK", Red = 0, Green = 0, Blue = 0, Alpha = 1,};
				function IsUniqueColor(color)
					if(colorNotUnique[color.Type]) then
						return false;
					end
					
					local blackThreshold = 5000;
					local differenceThreshold = 3000;
					
					local distanceAgainstBlack = ColorDistance(color, colorBlack);
					if(distanceAgainstBlack > blackThreshold) then
						for i, v in pairs(panel.PlayerGraphColors) do
							local distanceAgainstOtherPlayer = ColorDistance(color, v);
							if(distanceAgainstOtherPlayer < differenceThreshold) then
								colorNotUnique[color.Type] = true;
								return false;
							end
						end
						
						return true;
					end
					
					colorNotUnique[color.Type] = true;
					return false;
				end
					
				function DetermineGraphColor(playerColor)
					if(IsUniqueColor(GameInfo.Colors[playerColor.PrimaryColor])) then
						return GameInfo.Colors[playerColor.PrimaryColor];
					elseif(IsUniqueColor(GameInfo.Colors[playerColor.SecondaryColor])) then
						return GameInfo.Colors[playerColor.SecondaryColor];
					else
						for color in GameInfo.Colors() do
							if(IsUniqueColor(color)) then
								return color;
							end
						end
					end
										
					--error("Could not find a unique color");
					return GameInfo.Colors[playerColor.PrimaryColor];
				end

				panel.PlayerGraphColors = {};
				g_GraphLegendInstanceManager:ResetInstances();
				for i, player in ipairs(g_ReplayInfo.PlayerInfo) do
					
					local civ = GameInfo.Civilizations[player.Civilization];
					
					local graphLegendInstance = g_GraphLegendInstanceManager:GetInstance();
					
					IconHookup( civ.PortraitIndex, 32, civ.IconAtlas, graphLegendInstance.LegendIcon );
					
					local color = DetermineGraphColor(GameInfo.PlayerColors[player.PlayerColor]);
					panel.PlayerGraphColors[i] = color;
									
					graphLegendInstance.LegendLine:SetColorVal(color.Red, color.Green, color.Blue, color.Alpha);
					
					if(player.CivShortDescription ~= nil) then
						graphLegendInstance.LegendName:LocalizeAndSetText(player.CivShortDescription);
					else
						graphLegendInstance.LegendName:LocalizeAndSetText(civ.ShortDescription);
					end	
					
					-- Default city states to be unchecked.
					local checked = civ.Type ~= "CIVILIZATION_MINOR";
					graphLegendInstance.ShowHide:SetCheck(checked);
					
					graphLegendInstance.ShowHide:RegisterCheckHandler( function(bCheck)
						local segments = panel.SegmentsByPlayer[i];
						if(segments) then
							for i,v in ipairs(segments) do
								v.LineSegment:SetHide(not bCheck);
							end
						end
					end);
					
					panel.GraphLegendsByPlayer[i] = graphLegendInstance;
				end
				
				panel.OnUpdate = panel:CreateUpdateHandler();
			end
			
			RefreshCivilizations();
			RefreshHorizontalScales();
			
			Controls.GraphLegendStack:CalculateSize();
			Controls.GraphLegendStack:ReprocessAnchoring();
			Controls.GraphLegendScrollPanel:CalculateInternalSize();

		end,
	},
	
	{
		Title = Locale.Lookup("TXT_KEY_REPLAY_VIEWER_MAP_TITLE"),
		Tooltip = Locale.Lookup("TXT_KEY_REPLAY_VIEWER_MAP_TT"),
		Panel = Controls.MapPanel,
		IsAnimating = false,
		CurrentTurn = nil,
		CreateSetCurrentTurnFunction = function(panel, replayInfo)
		
			local mapWidth = replayInfo.MapWidth;
			local mapHeight = replayInfo.MapHeight;
			local initialTurn = replayInfo.InitialTurn;
			local finalTurn = replayInfo.FinalTurn;
			
			local defaultColor = {
				Red = 0, Green = 0, Blue = 0, Alpha = 0
			};
			
			-- Cache the player colors in a more usable structure.
			local playerColors = {};
			local minorCiv = GameInfo.Civilizations["CIVILIZATION_MINOR"];
			
			for i, player in ipairs(replayInfo.PlayerInfo) do
				local playerColor = GameInfo.PlayerColors[player.PlayerColor];
				
				local primaryColor = GameInfo.Colors[playerColor.PrimaryColor];
				local secondaryColor = GameInfo.Colors[playerColor.SecondaryColor];
				
				-- Reverse colors for minor civs.
				local civ = GameInfo.Civilizations[player.Civilization];
				if(civ.Type == minorCiv.Type) then
					local temp = primaryColor;
					primaryColor = secondaryColor;
					secondaryColor = temp;
				end
				
				
				playerColors[i] = {
					Primary = {
						Red = primaryColor.Red,
						Green = primaryColor.Green,
						Blue = primaryColor.Blue,
						Alpha = 0.8
					},
					Secondary = {
						Red = secondaryColor.Red,
						Green = secondaryColor.Green,
						Blue = secondaryColor.Blue,
						Alpha = 1.0
					},
				}
			end
			
			local IceColor = {
				Red = 189/255,
				Green = 242/255,
				Blue = 1,
				Alpha = 0.7,
			};
			
			local ClearColor = {
				Red = 0,
				Green = 0,
				Blue = 0,
				Alpha = 0
			};
			
			local IceFeatureID = GameInfo.Features["FEATURE_ICE"].ID
			
			-- We've got 3 tables here, terrain, culture, and cities.
			-- Each one is populated with values that will ultimately
			-- determine the map tile color.
			local featureColors = {};
			local cultureColors = {};
			local cityColors = {};
			
			-- Gather and organize how plot colors are stored so that they can be quickly accessed.
			local plotColors = {};
			for turn = initialTurn, finalTurn, 1 do
				plotColors[turn] = {};
				cultureColors[turn] = {};
				cityColors[turn] = {};
			end
					
			-- Create a single table of just the TerrainType.
			-- The data structure supports the plotstate changing per turn but
			-- for now we only care about the first found state.
			
			-- Also assign a ice blue color overlay for tiles w/ ICE on them.
			local plotTerrains = {};
			for i, plot in pairs(replayInfo.Plots) do
				for turn, plotStateAtTurn in pairs(plot) do
					plotTerrains[i - 1] = plotStateAtTurn.TerrainType;
			
					if(plotStateAtTurn.FeatureType == IceFeatureID) then
						featureColors[i - 1] = IceColor;
					end
					
					break;
				end
			end

			local REPLAY_MESSAGE_CITY_FOUNDED = 1;
			local REPLAY_MESSAGE_PLOT_OWNER_CHANGE = 2;
			local REPLAY_MESSAGE_CITY_CAPTURED = 3;
			local REPLAY_MESSAGE_CITY_DESTROYED = 4;

			for i, message in ipairs(replayInfo.Messages) do
				
				if(	message.Type == REPLAY_MESSAGE_CITY_FOUNDED or 
				    message.Type == REPLAY_MESSAGE_CITY_CAPTURED) then
										
					local cityColorsForTurn = cityColors[message.Turn];
					if(cityColorsForTurn == nil) then
						cityColorsForTurn = {};
						cityColors[message.Turn] = cityColorsForTurn;
					end
			
					for _, plot in ipairs(message.Plots) do
						local playerColor = playerColors[message.Player];
						if(playerColor ~= nil) then					
							local idx = plot.Y * mapWidth + plot.X;
							cityColorsForTurn[idx] = playerColor.Primary;
						end
					end	
											
				elseif(message.Type == REPLAY_MESSAGE_CITY_DESTROYED) then
					
					local cityColorsForTurn = cityColors[message.Turn];
					if(cityColorsForTurn == nil) then
						cityColorsForTurn = {};
						cityColors[message.Turn] = cityColorsForTurn;
					end
					
					for _, plot in ipairs(message.Plots) do
						local idx = plot.Y * mapWidth + plot.X;
						cityColorsForTurn[idx] = ClearColor;
					end	
					
				elseif(message.Type == REPLAY_MESSAGE_PLOT_OWNER_CHANGE) then
					
					local cultureColorsForTurn = cultureColors[message.Turn];
					if(cultureColorsForTurn == nil) then
						cultureColorsForTurn = {};
						cultureColors[message.Turn] = cultureColorsForTurn;
					end
				
					for _, plot in ipairs(message.Plots) do
					
						local playerColor = playerColors[message.Player];
						local idx = plot.Y * mapWidth + plot.X;
						if(playerColor ~= nil) then
							cultureColorsForTurn[idx] = playerColor.Secondary;
						else
							cultureColorsForTurn[idx] = ClearColor;
						end
					end		
				end				
			end
			
			-- These new datasets are nice but still not as fast as we'd like since we must
			-- iterate backwards to obtain the plot colors for all plots.
			-- Let's populate the list further so that no iteration is necessary later.
			for y = 0, mapHeight - 1, 1 do	
				for x = 0, mapWidth - 1, 1 do
					local idx = x + (y * mapWidth);
					
					local cityColor;
					local cultureColor;
					
					for turn = initialTurn, finalTurn, 1 do
						local cultureColorsAtTurn = cultureColors[turn];
						local cityColorsAtTurn = cityColors[turn];
				
						local cityColorAtTurn = cityColorsAtTurn[idx];
						if(cityColorAtTurn ~= nil) then
							cityColor = cityColorAtTurn;
						else
							cityColorsAtTurn[idx] = cityColor;
						end
						
						local cultureColorAtTurn = cultureColorsAtTurn[idx];
						if(cultureColorAtTurn ~= nil) then
							cultureColor = cultureColorAtTurn;
						else
							cultureColorsAtTurn[idx] = cultureColor;
						end
					end		
				end
			end
			
			-- Just a nice utility function to get the color based on a priority.
			local function GetPlotColor(turn, plotIdx) 
				local cultureColorsAtTurn = cultureColors[turn];
				local cityColorsAtTurn = cityColors[turn];
				
				local cityColor = (cityColorsAtTurn ~= nil) and cityColorsAtTurn[plotIdx] or nil;
				local cultureColor = (cultureColorsAtTurn ~= nil) and cultureColorsAtTurn[plotIdx] or nil;
				local featureColor = featureColors[plotIdx];
				
				if(cityColor ~= nil and cityColor.Alpha > 0) then
					return cityColor;
				elseif(cultureColor ~= nil and cultureColor.Alpha > 0) then
					return cultureColor;
				elseif(featureColor ~= nil) then
					return featureColor;
				else
					return ClearColor;
				end
			end
			
			-- Populate the plotColors table
			for y = 0, mapHeight - 1, 1 do	
				for x = 0, mapWidth - 1, 1 do
					local idx = x + (y * mapWidth);
					
					for turn = initialTurn, finalTurn, 1 do
						local plotColorsAtTurn = plotColors[turn];
						plotColorsAtTurn[idx] = GetPlotColor(turn, idx);
					end
				end
			end
			
			local turnLabel = Controls.TurnLabel;
			local turnSlider = Controls.TurnSlider;
		
			return function(panel, currentTurn)
				-- Up Values
				-- defaultColor
				-- plotTerrains
				-- plotColors
				-- mapWidth
				-- mapHeight
				-- turnLabel
				
				print("Drawing Map at Turn " .. currentTurn);
				
				turnLabel:LocalizeAndSetText("TXT_KEY_TP_TURN_COUNTER", currentTurn);
				
				local sliderValue = (currentTurn - initialTurn)/(finalTurn - initialTurn);
				
				turnSlider:SetValue(sliderValue);
				
				panel.CurrentTurn = currentTurn;
				
				local replayMapControl = Controls.ReplayMap;
				
				local plotColorsAtTurn = plotColors[currentTurn];
				
				for y = 0, mapHeight - 1, 1 do	
					for x = 0, mapWidth - 1, 1 do
						local idx = x + (y * mapWidth);
						local terrainType = plotTerrains[idx];
						
						local plotColor = defaultColor;
						if(plotColorsAtTurn ~= nil) then
							local plotColorAtTurn = plotColorsAtTurn[idx];
							if(plotColorAtTurn ~= nil) then
								plotColor = plotColorAtTurn;
							end
						end
						
						replayMapControl:SetPlot(x, y, terrainType, plotColor.Red, plotColor.Green, plotColor.Blue, plotColor.Alpha);
					end
				end
			end
		end,
		
		CreateUpdateFunction = function(panel, replayInfo)
			local turnDelay = 0.25; -- seconds per turn.
			local currentDelay = turnDelay;
			
			return function(panel, dTime)
				if(panel.IsAnimating == true) then
					currentDelay = currentDelay - dTime;
					if(currentDelay <= 0) then
						currentDelay = turnDelay;
						local newTurn = panel.CurrentTurn + 1;
						if(newTurn > replayInfo.FinalTurn) then
							newTurn = replayInfo.InitialTurn;
						end
						panel:SetCurrentTurn(newTurn);
					end
				end
			end
		end,
		
		Refresh = function(panel) 
			local mapWidth = g_ReplayInfo.MapWidth;
			local mapHeight = g_ReplayInfo.MapHeight;
			
			Controls.ReplayMap:SetMapSize(mapWidth, mapHeight, 0, -1);
			
			-- Generate a new SetCurrentTurn function that has many items pre-cached
			panel.SetCurrentTurn = panel:CreateSetCurrentTurnFunction(g_ReplayInfo);
			panel.OnUpdate = panel:CreateUpdateFunction(g_ReplayInfo);
			
			if(panel.CurrentTurn == nil) then
				panel.CurrentTurn = g_ReplayInfo.InitialTurn;
			end
			
			panel:SetCurrentTurn(panel.CurrentTurn);
		end,
	},
};

CurrentPanelIndex = 1;

----------------------------------------------------------------
----------------------------------------------------------------
function OnBack()
	UIManager:SetUICursor(1);
	Modding.DeactivateMods();
	UIManager:SetUICursor(0);
    UIManager:DequeuePopup( ContextPtr );
end
Controls.BackButton:RegisterCallback(Mouse.eLClick, OnBack);

----------------------------------------------------------------
function InputHandler(uiMsg, wParam, lParam)
    if(uiMsg == KeyEvents.KeyDown) then
        if(wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN) then
			OnBack();
        end
		return true;
    end
   
end

if(not g_bIsEndGame) then
	ContextPtr:SetInputHandler(InputHandler);
end

----------------------------------------------------------------
function SetCurrentPanel(panelIndex)
	for i,v in pairs(Panels) do
		v.Panel:SetHide(true);
	end
	
	local p = Panels[panelIndex];
	p.Panel:SetHide(false);
	
	local replayInfoPulldownButton = Controls.ReplayInfoPulldown:GetButton();
	replayInfoPulldownButton:SetText(p.Title);
	replayInfoPulldownButton:SetToolTipString(p.ToolTip);
	
	CurrentPanelIndex = panelIndex;
end

function SetCurrentGraphDataSet(dataSetIndex)
	
	local ds = GameInfo.ReplayDataSets[dataSetIndex];
	local graphDataSetPulldownButton = Controls.GraphDataSetPulldown:GetButton();
	graphDataSetPulldownButton:LocalizeAndSetText(ds.Description);
	
	local graphPanel = Panels[2];
	graphPanel.CurrentGraphDataSetIndex = dataSetIndex;
	graphPanel:DrawGraph();
end

function Refresh()
	print("Refreshing Replay Viewer");
	
	for i,v in ipairs(Panels) do
		v:Refresh();
	end
	
	SetCurrentPanel(1);

	-- Get First Dataset Type
	local firstDataSet;
	for row in GameInfo.ReplayDataSets() do
		firstDataSet = row.Type;
		break;
	end
	
	SetCurrentGraphDataSet(firstDataSet);
end

function OnUpdate(dTime)
	local panel = Panels[CurrentPanelIndex];
	if(panel.OnUpdate ~= nil) then
		panel:OnUpdate(dTime);
	end
end
ContextPtr:SetUpdate(OnUpdate);

-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide )
	if(not bIsHide) then
		if(g_bIsEndGame) then
			GenerateReplayInfoFromCurrentGame();
			Refresh();
		end
	else
		-- Dispose of temporary data.
		-- NOTE: This is all hard-coded here but could probably be better architected.
		-- Free Instances
		g_ReplayMessageInstanceManager:ResetInstances();
		g_GraphLegendInstanceManager:ResetInstances();
		g_LineSegmentInstanceManager:ResetInstances();
		
		-- Graph Panel
		local graphPanel = Panels[2];
		graphPanel.CurrentGraphDataSetIndex = nil;		
		graphPanel.SegmentsByPlayer = {};
		graphPanel.PlayerGraphColors = {};	
		graphPanel.GraphLegendsByPlayer = {};			
		graphPanel.OnUpdate = nil;
		
		-- Map Panel
		local mapPanel = Panels[3];
		mapPanel.IsAnimating = false;
		mapPanel.CurrentTurn = nil;
		mapPanel.SetCurrentTurn = nil;
		mapPanel.OnUpdate = nil;
		
		-- Replay Info
		g_ReplayInfo = nil;
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

function OnPausePlay()
	local mapPanel = Panels[3];
	mapPanel.IsAnimating = not mapPanel.IsAnimating;
end
Controls.PlayPauseButton:RegisterCallback(Mouse.eLClick, OnPausePlay);

function OnTurnSliderValueChanged(newValue)
	local initialTurn = g_ReplayInfo.InitialTurn;
	local finalTurn = g_ReplayInfo.FinalTurn;
	local currentTurn = math.floor(newValue * (finalTurn - initialTurn)) + initialTurn;

	local mapPanel = Panels[3];
	mapPanel:SetCurrentTurn(currentTurn);
	
end
Controls.TurnSlider:RegisterSliderCallback(OnTurnSliderValueChanged);

----------------------------------------------------------------
-- Static Initialization
----------------------------------------------------------------
local replayInfoPulldown = Controls.ReplayInfoPulldown;
replayInfoPulldown:ClearEntries();
for i, v in ipairs(Panels) do
	local controlTable = {};
	replayInfoPulldown:BuildEntry( "InstanceOne", controlTable );
	controlTable.Button:SetText(v.Title);
	controlTable.Button:SetToolTipString(v.ToolTip);
	
	controlTable.Button:RegisterCallback(Mouse.eLClick, function()
		SetCurrentPanel(i);
	end);
end
replayInfoPulldown:CalculateInternals();

function RefreshGraphDataSets()		
	
	local graphDataSetPulldown = Controls.GraphDataSetPulldown;
	graphDataSetPulldown:ClearEntries();
	
	local graphEntries = {};
	for row in GameInfo.ReplayDataSets() do
		table.insert(graphEntries, {Locale.Lookup(row.Description), row.Type});
	end	
	
	table.sort(graphEntries, function(a,b) return Locale.Compare(a[1], b[1]) == -1; end);
	
	for i,v in ipairs(graphEntries) do
		local controlTable = {};
		graphDataSetPulldown:BuildEntry( "InstanceOne", controlTable );
		controlTable.Button:LocalizeAndSetText(v[1]);

		controlTable.Button:RegisterCallback(Mouse.eLClick, function()
			SetCurrentGraphDataSet(v[2]);
		end);
	end
	graphDataSetPulldown:CalculateInternals();
	
end

RefreshGraphDataSets();
	
LuaEvents.ReplayViewer_LoadReplay.Add(function(file)
	print("Loading Replay - " .. file);
	g_ReplayInfo = UI.GetReplayInfo(file);
	Refresh();
end);


function GenerateReplayInfoFromCurrentGame()
	
	local mapWidth, mapHeight = Map.GetGridSize();
	local finalTurn = Game.GetGameTurn();
	g_ReplayInfo = {
		--Populate Generic Info
		
		--MapScriptName =
		--WorldSize = 
		--Climate =
		--SeaLevel = 
		Era = Game.GetStartEra(),
		GameSpeed = Game.GetGameSpeedType(),
		--VictoryType = 
		--NormalizedScore =
		
		Calendar = Game.GetCalendar(),
		InitialTurn = Game.GetStartTurn(),
		FinalTurn = finalTurn,
		StartYear = Game.GetStartYear(),
		--GameType = nil,
		--FinalDate = nil,
		
		MapWidth = mapWidth,
		MapHeight= mapHeight,
	};
	
	-- Populate Player Info
	local playerInfos = {};	
	local playerMap = { [-1] = -1};
	local playerInfosIndex = 1;
	for player_num = 0, GameDefines.MAX_CIV_PLAYERS - 1 do
		local player = Players[player_num];
		if(player and player:IsEverAlive()) then
		
			local playerInfo = {
				Civilization = GameInfo.Civilizations[player:GetCivilizationType()].Type,
				Leader = GameInfo.Leaders[player:GetLeaderType()].Type,
				PlayerColor = GameInfo.PlayerColors[player:GetPlayerColor()].Type,
				Difficulty = GameInfo.HandicapInfos[player:GetHandicapType()].Type,
				LeaderName = player:GetName(),
				CivDescription = player:GetCivilizationDescription(),
				CivShortDescription = player:GetCivilizationShortDescription(),
				CivAdjective = player:GetCivilizationAdjective(),
				Scores = {},	
			};
			
			-- Player:GetReplayData returns table[dataset][turn] = value
			-- We need to convert this to scores[turn][dataset] = value
			local scores = {};
			local playerScores = player:GetReplayData();	
			for ds,v in pairs(playerScores) do
				for turn, value in pairs(v) do
					if(scores[turn] == nil) then
						scores[turn] = {};
					end
					
					local turnData = scores[turn];
					turnData[ds] = value;
				end
			end
			
			playerInfo.Scores = scores;
			
			playerInfos[playerInfosIndex] = playerInfo;
			playerMap[player_num] = playerInfosIndex;
			
			playerInfosIndex = playerInfosIndex + 1;
		end	
	end
	
	g_ReplayInfo.PlayerInfo = playerInfos;
	g_ReplayInfo.ActivePlayer = playerMap[Game.GetActivePlayer()];
	if(g_ReplayInfo.ActivePlayer == nil) then
		g_ReplayInfo.ActivePlayer = playerMap[0];
	end
	
	-- Populate Messages	
	-- Grab messages and sort by their turn.
	local replayMessages = {};
	local messages = Game.GetReplayMessages();
	-- table.sort(messages, function(a, b) return a.Turn < b.Turn end);
	for i,message in ipairs(messages) do
		table.insert(replayMessages, {Turn = message.Turn, Text = message.Text, Type = message.Type, Player = playerMap[message.Player], Plots = message.Plots});
	end
	g_ReplayInfo.Messages = replayMessages;
	
	-- Populate Plots
	local plots = {};
	for i = 0, Map.GetNumPlots() - 1, 1 do
		local plot = Map.GetPlotByIndex(i);
		
		plots[i + 1] = {
			-- Only record final turn for now
			[finalTurn] = {
				TerrainType = plot:GetTerrainType(),
				FeatureType = plot:GetFeatureType(),
			}
		}
	end
	g_ReplayInfo.Plots = plots;
	
end

if(g_bIsEndGame) then
	Controls.FrontEndReplayViewer:SetHide(true);
end

