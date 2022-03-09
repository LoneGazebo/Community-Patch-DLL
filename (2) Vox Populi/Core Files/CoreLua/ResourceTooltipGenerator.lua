print("This is the modded ResourceTooltipGenerator from CBP")
-------------------------------------------------
-- Resource Tooltip Generator
-------------------------------------------------

function GenerateResourceToolTip(plot)
	-- Resource
    local iResourceID = plot:GetResourceType(Game.GetActiveTeam());
    local pResourceInfo = GameInfo.Resources[iResourceID];
    if (pResourceInfo == nil) then
		return nil;
	end
    
    -- Quantity
    local strQuantity = "";
    if (plot:GetNumResource() > 1) then
		strQuantity = plot:GetNumResource() .. " ";
    end
    
    -- Name
    local strToolTip = "[COLOR_POSITIVE_TEXT]" .. strQuantity .. Locale.ToUpper(Locale.ConvertTextKey(pResourceInfo.Description)) .. "[ENDCOLOR]";
	
	-- Extra Help Text (e.g. for strategic resources)
	if (pResourceInfo.Help) then
		-- Basic tooltips get extra info
		if (not OptionsManager.IsNoBasicHelp()) then
			strToolTip = strToolTip .. "[NEWLINE]";
			strToolTip = strToolTip .. "[ICON_BULLET]" .. Locale.ConvertTextKey(pResourceInfo.Help);
			strToolTip = strToolTip .. "[NEWLINE]---------------------";
		end
	end
	
	-- Happiness
	if (pResourceInfo.Happiness) then
		if (pResourceInfo.Happiness ~= 0) then
			local strHappinessToolTip = "";

			-- Basic tooltips get extra info
			if (not OptionsManager.IsNoBasicHelp()) then
				strHappinessToolTip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_TOOLTIP_IMPROVED");
				strHappinessToolTip = strHappinessToolTip .. "[NEWLINE]";
			end
			-- COMMUNITY PATCH CHANGE
			local pPlayer = Players[Game.GetActivePlayer()];
			local iHappiness = pPlayer:GetHappinessFromLuxury(iResourceID);
			if (iHappiness > 0) then
				strHappinessToolTip = strHappinessToolTip .. "[ICON_BULLET]+" .. iHappiness .. " [ICON_HAPPINESS_1]";
			end
			--END CHANGE
			--strHappinessToolTip = strHappinessToolTip .. "[ICON_BULLET]+" .. pResourceInfo.Happiness .. " [ICON_HAPPINESS_1]";
			-- Basic tooltips get extra info
			if (not OptionsManager.IsNoBasicHelp()) then
				strHappinessToolTip = strHappinessToolTip .. " " .. Locale.ConvertTextKey("TXT_KEY_GAME_CONCEPT_SECTION_10");	-- This is the text key for Happiness... don't ask
			end

			strToolTip = strToolTip .. "[NEWLINE]";
			strToolTip = strToolTip .. strHappinessToolTip;
		end
	end
	
	-- Yield
	local strYieldToolTip = "";
	local condition = "ResourceType = '" .. pResourceInfo.Type .. "'";
	local pYieldInfo;
	local bFirst = true;
	for row in GameInfo.Resource_YieldChanges( condition ) do
		pYieldInfo = GameInfo.Yields[row.YieldType];
		
		-- Add bullet in front of first entry, space in front of all others
		if (bFirst) then
			bFirst = false;
			strYieldToolTip = strYieldToolTip .. "[ICON_BULLET]";
		else
			strYieldToolTip = strYieldToolTip .. " ";
		end
		
		if row.Yield > 0 then
			strYieldToolTip = strYieldToolTip.."+";
		end
		
		strYieldToolTip = strYieldToolTip .. tostring(row.Yield) .. pYieldInfo.IconString;
		
		-- Basic tooltips get extra info
		if (not OptionsManager.IsNoBasicHelp()) then
			strYieldToolTip = strYieldToolTip .. " " .. Locale.ConvertTextKey(pYieldInfo.Description);
		end
	end

	-- Something in the yield tooltip?
	if (strYieldToolTip ~= "") then
		
		-- Basic tooltips get extra info
		if (not OptionsManager.IsNoBasicHelp()) then
			strYieldToolTip = Locale.ConvertTextKey("TXT_KEY_RESOURCE_TOOLTIP_IMPROVED_WORKED") .. "[NEWLINE]" .. strYieldToolTip;
		end
		
		strToolTip = strToolTip .. "[NEWLINE]";
		strToolTip = strToolTip .. strYieldToolTip;
	end
	
	return strToolTip;
end
