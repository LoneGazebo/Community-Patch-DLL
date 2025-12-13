if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Plot Help Text
-------------------------------------------------

include( "PlotMouseoverInclude" );

function OnMouseOverHex( hexX, hexY )
	local TextString = "";

	local plot = Map.GetPlot( hexX, hexY );
	
	local iActiveTeam = Game.GetActiveTeam();

	local bIsDebug = Game.IsDebugMode();

	if (plot and plot:IsRevealed(iActiveTeam, bIsDebug)) then
		
		local strQuest = GetCivStateQuestString(plot, true);
		if (strQuest ~= "") then
			TextString = TextString .. strQuest;
			TextString = TextString .. "[NEWLINE]";
		end
		
		-- Resource
		local strResourceAndImprovement = GetResourceString(plot, false);
		if (strResourceAndImprovement == nil) then
			strResourceAndImprovement = "";
		end
		
		local strImprovement = GetImprovementString(plot);
		
		if (strResourceAndImprovement ~= "" and strImprovement ~= "") then
			strResourceAndImprovement = strResourceAndImprovement .. ", ";
		end
		
		-- Improvment, Route
		strResourceAndImprovement = strResourceAndImprovement .. strImprovement;
		if (strResourceAndImprovement ~= "") then
			
			TextString = TextString .. strResourceAndImprovement;
			TextString = TextString .. "[NEWLINE]";
		end
		
		-- Terrain, Feature, etc.
		local natureStr = GetNatureString(plot);
		if (natureStr ~= "") then
			TextString = TextString .. natureStr;
		end
		
		-- Yield (in advanced tooltip mode)
		local strYield = GetYieldString(plot);
		if (strYield ~= "") then
			TextString = TextString .. "[NEWLINE]" .. strYield;
		end
		
		-- International trade routes
		local strInternationalTradeRoutes = GetInternationalTradeRouteString(plot);
		--print("strStuff: " .. strInternationalTradeRoutes);
		if (strInternationalTradeRoutes ~= "") then
			TextString = TextString .. "[NEWLINE]" .. strInternationalTradeRoutes;
		end
		
		if (OptionsManager:IsDebugMode()) then
			-- X Y - in debug
			TextString = TextString .. "[NEWLINE]";
			TextString = TextString .. tostring(plot:GetX());
			TextString = TextString .. "x ";
			TextString = TextString .. tostring(plot:GetY());
			TextString = TextString .. "y    ";
			TextString = TextString .. tostring(hexX);
			TextString = TextString .. "i ";
			TextString = TextString .. tostring(hexY);
			TextString = TextString .. "j";
		end
	end
	
	if (TextString ~= "") then
		Controls.TextBox:SetHide(false);
		Controls.Text:SetText( TextString );
		Controls.TextBox:DoAutoSize();
		Controls.TextBox:ReprocessAnchoring();
	else
		Controls.TextBox:SetHide(true);
	end
	
end
Events.SerialEventMouseOverHex.Add( OnMouseOverHex );
