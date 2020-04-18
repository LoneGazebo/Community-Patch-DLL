------------------------------------------------
-- Unit Panel Screen 
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );

local g_PrimaryIM    = InstanceManager:new( "UnitAction",  "UnitActionButton", Controls.PrimaryStack );
local g_SecondaryIM  = InstanceManager:new( "UnitAction",  "UnitActionButton", Controls.SecondaryStack );
local g_BuildIM      = InstanceManager:new( "UnitAction",  "UnitActionButton", Controls.WorkerActionPanel );
local g_PromotionIM  = InstanceManager:new( "UnitAction",  "UnitActionButton", Controls.WorkerActionPanel );
local g_EarnedPromotionIM   = InstanceManager:new( "EarnedPromotionInstance", "UnitPromotionImage", Controls.EarnedPromotionStack );

    
local g_CurrentActions = {};    --CurrentActions associated with each button
local g_lastUnitID = -1;        -- Used to determine if a different unit has been selected.
local g_ActionButtons = {};
local g_PromotionsOpen = false;
local g_SecondaryOpen = false;
local g_WorkerActionPanelOpen = false;

local MaxDamage = GameDefines.MAX_HIT_POINTS;

local promotionsTexture = "Promotions512.dds";

local unitPortraitSize = Controls.UnitPortrait:GetSize().x;
local actionIconSize = 64;
if OptionsManager.GetSmallUIAssets() and not UI.IsTouchScreenEnabled() then
	actionIconSize = 45;
end

--------------------------------------------------------------------------------
-- this maps from the normal instance names to the build city control names
-- so we can use the same code to set it up
--------------------------------------------------------------------------------
local g_BuildCityControlMap = { 
    UnitActionButton    = Controls.BuildCityButton,
    --UnitActionMouseover = Controls.BuildCityMouseover,
    --UnitActionText      = Controls.BuildCityText,
    --UnitActionHotKey    = Controls.BuildCityHotKey,
    --UnitActionHelp      = Controls.BuildCityHelp,
};

local direction_types = {
	DirectionTypes.DIRECTION_NORTHEAST,
	DirectionTypes.DIRECTION_EAST,
	DirectionTypes.DIRECTION_SOUTHEAST,
	DirectionTypes.DIRECTION_SOUTHWEST,
	DirectionTypes.DIRECTION_WEST,
	DirectionTypes.DIRECTION_NORTHWEST
};
	
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
function OnPromotionButton()
    g_PromotionsOpen = not g_PromotionsOpen;
    
    if g_PromotionsOpen then
        Controls.PromotionStack:SetHide( false );
    else
        Controls.PromotionStack:SetHide( true );
    end
end
Controls.PromotionButton:RegisterCallback( Mouse.eLClick, OnPromotionButton );


--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
function OnSecondaryButton()
    g_SecondaryOpen = not g_SecondaryOpen;
    
    if g_SecondaryOpen then
        Controls.SecondaryStack:SetHide( false );
        Controls.SecondaryStretchy:SetHide( false );
        Controls.SecondaryImageOpen:SetHide( true );
        Controls.SecondaryImageClosed:SetHide( false );
        Controls.SecondaryButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_SECONDARY_O_TEXT" ));

    else
        Controls.SecondaryStack:SetHide( true );
        Controls.SecondaryStretchy:SetHide( true );
        Controls.SecondaryImageOpen:SetHide( false );
        Controls.SecondaryImageClosed:SetHide( true );
        Controls.SecondaryButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_SECONDARY_C_TEXT" ));
    end
end
Controls.SecondaryButton:RegisterCallback( Mouse.eLClick, OnSecondaryButton );


local GetActionIconIndexAndAtlas = {
	[ActionSubTypes.ACTIONSUBTYPE_PROMOTION] = function(action)
		local thisPromotion = GameInfo.UnitPromotions[action.CommandData];
		return thisPromotion.PortraitIndex, thisPromotion.IconAtlas;
	end,
	
	[ActionSubTypes.ACTIONSUBTYPE_INTERFACEMODE] = function(action)
		local info = GameInfo.InterfaceModes[action.Type];
		return info.IconIndex, info.IconAtlas;
	end,
	
	[ActionSubTypes.ACTIONSUBTYPE_MISSION] = function(action)
		local info = GameInfo.Missions[action.Type];
		return info.IconIndex, info.IconAtlas;
	end,
	
	[ActionSubTypes.ACTIONSUBTYPE_COMMAND] = function(action)
		local info = GameInfo.Commands[action.Type];
		return info.IconIndex, info.IconAtlas;
	end,
	
	[ActionSubTypes.ACTIONSUBTYPE_AUTOMATE] = function(action)
		local info = GameInfo.Automates[action.Type];
		return info.IconIndex, info.IconAtlas;
	end,
	
	[ActionSubTypes.ACTIONSUBTYPE_BUILD] = function(action)
		local info = GameInfo.Builds[action.Type];
		return info.IconIndex, info.IconAtlas;
	end,
	
	[ActionSubTypes.ACTIONSUBTYPE_CONTROL] = function(action)
		local info = GameInfo.Controls[action.Type];
		return info.IconIndex, info.IconAtlas;
	end,
};

function HookupActionIcon(action, actionIconSize, icon)

	local f = GetActionIconIndexAndAtlas[action.SubType];
	if(f ~= nil) then
		local iconIndex, iconAtlas = f(action);
		IconHookup(iconIndex, actionIconSize, iconAtlas, icon);
	else
		print(action.Type);
		print(action.SubType);
		error("Could not find method to obtain action icon.");
	end
end
--------------------------------------------------------------------------------
-- Refresh unit actions
--------------------------------------------------------------------------------
function UpdateUnitActions( unit )

    g_PrimaryIM:ResetInstances();
    g_SecondaryIM:ResetInstances();
    g_BuildIM:ResetInstances();
    g_PromotionIM:ResetInstances();
    Controls.BuildCityButton:SetHide( true );
    Controls.WorkerActionPanel:SetHide(true);
    local pActivePlayer = Players[Game.GetActivePlayer()];
    
    local bShowActionButton;
    local bUnitHasMovesLeft = unit:MovesLeft() > 0;
    
    -- Text that tells the player this Unit's out of moves this turn
    if (unit:IsTrade()) then
		Controls.UnitStatusInfo:SetHide(true);
		Controls.SecondaryButton:SetHide(true);
		Controls.UnitStatMovement:SetHide(true);
		Controls.UnitStatNameMovement:SetHide(true);
    elseif (not bUnitHasMovesLeft) then
		Controls.UnitStatusInfo:SetHide(false);
		Controls.SecondaryButton:SetHide(true);
		Controls.UnitStatMovement:SetHide(false);
		Controls.UnitStatNameMovement:SetHide(false);
	else
		Controls.UnitStatusInfo:SetHide(true);
		Controls.SecondaryButton:SetHide(false);
		Controls.UnitStatMovement:SetHide(false);
		Controls.UnitStatNameMovement:SetHide(false);
    end
    
    local hasPromotion = false;
	local bBuild = false;
	local bPromotion = false;
	local iBuildID;

    Controls.BackgroundCivFrame:SetHide( false );
   
	local numBuildActions = 0;
	local numPromotions = 0;
	local numPrimaryActions = 0;
	local numSecondaryActions = 0;
	
	local numberOfButtonsPerRow = 4;
	local buttonSize = 60;
	local buttonPadding = 8;
	local buttonOffsetX = 16;
	local buttonOffsetY = 40;
	local workerPanelSizeOffsetY = 104;
	if OptionsManager.GetSmallUIAssets() and not UI.IsTouchScreenEnabled() then
		numberOfButtonsPerRow = 6;
		buttonSize = 40;
		buttonPadding = 6;
		workerPanelSizeOffsetY = 86;
	end

    local recommendedBuild = nil;
    
    local buildCityButtonActive = false;
   
       -- loop over all the game actions
    for iAction = 0, #GameInfoActions, 1 do
        local action = GameInfoActions[iAction];
        
		-- test CanHandleAction w/ visible flag (ie COULD train if ... )
        if(action.Visible and Game.CanHandleAction( iAction, 0, 1 ) ) then
           	if( action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION ) then
                hasPromotion = true;                
			end
        end
    end


    -- loop over all the game actions
    for iAction = 0, #GameInfoActions, 1 
    do
        local action = GameInfoActions[iAction];
        
        local bBuild = false;
        local bPromotion = false;
        local bDisabled = false;
        
        -- We hide the Action buttons when Units are out of moves so new players aren't confused
        if (bUnitHasMovesLeft or action.Type == "COMMAND_CANCEL" or action.Type == "COMMAND_STOP_AUTOMATION" or action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION) then
			bShowActionButton = true;
		else
			bShowActionButton = false;
        end
        
		-- test CanHandleAction w/ visible flag (ie COULD train if ... )
        if( bShowActionButton and action.Visible and Game.CanHandleAction( iAction, 0, 1 ) ) 
        then
            local instance;
            local extraXOffset = 0;
			if (UI.IsTouchScreenEnabled()) then
				extraXOffset = 44;
			end
            if( action.Type == "MISSION_FOUND" ) then
                instance = g_BuildCityControlMap;
                Controls.BuildCityButton:SetHide( false );
                buildCityButtonActive = true;
                
            elseif( action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION ) then
				bPromotion = true;
                instance = g_PromotionIM:GetInstance();
                instance.UnitActionButton:SetAnchor( "L,B" );
				instance.UnitActionButton:SetOffsetVal( (numBuildActions % numberOfButtonsPerRow) * buttonSize + buttonPadding + buttonOffsetX + extraXOffset, math.floor(numBuildActions / numberOfButtonsPerRow) * buttonSize + buttonPadding + buttonOffsetY );				
                numBuildActions = numBuildActions + 1;
                
            elseif( (action.SubType == ActionSubTypes.ACTIONSUBTYPE_BUILD or action.Type == "INTERFACEMODE_ROUTE_TO") and hasPromotion == false) then
            
				bBuild = true;
				iBuildID = action.MissionData;
				instance = g_BuildIM:GetInstance();
				instance.UnitActionButton:SetAnchor( "L,B" );
				instance.UnitActionButton:SetOffsetVal( (numBuildActions % numberOfButtonsPerRow) * buttonSize + buttonPadding + buttonOffsetX + extraXOffset, math.floor(numBuildActions / numberOfButtonsPerRow) * buttonSize + buttonPadding + buttonOffsetY );				
				numBuildActions = numBuildActions + 1;
				if recommendedBuild == nil and unit:IsActionRecommended( iAction ) then
				
					recommendedBuild = iAction;
					
					local buildInfo = GameInfo.Builds[action.Type];				
					IconHookup( buildInfo.IconIndex, actionIconSize, buildInfo.IconAtlas, Controls.RecommendedActionImage );
					Controls.RecommendedActionButton:RegisterCallback( Mouse.eLClick, OnUnitActionClicked );
					Controls.RecommendedActionButton:SetVoid1( iAction );
					Controls.RecommendedActionButton:SetToolTipCallback( TipHandler );
					local text = action.TextKey or action.Type or "Action"..(buttonIndex - 1);
					local convertedKey = Locale.ConvertTextKey( text );
					
					local foo = Locale.Lookup("TXT_KEY_UPANEL_RECOMMENDED");
					
					Controls.RecommendedActionLabel:SetText( foo .. "[NEWLINE]" .. convertedKey );
				end
               
            elseif( action.OrderPriority > 100 ) then
                instance = g_PrimaryIM:GetInstance();
                numPrimaryActions = numPrimaryActions + 1;
                
            else
                instance = g_SecondaryIM:GetInstance();
                numSecondaryActions = numSecondaryActions + 1;
            end
            
			-- test w/o visible flag (ie can train right now)
			if not Game.CanHandleAction( iAction ) then
				bDisabled = true;
				instance.UnitActionButton:SetAlpha( 0.4 );                
				instance.UnitActionButton:SetDisabled( true );                
			else
				instance.UnitActionButton:SetAlpha( 1.0 );
				instance.UnitActionButton:SetDisabled( false );                
			end
			
            if(instance.UnitActionIcon ~= nil) then
				HookupActionIcon(action, actionIconSize, instance.UnitActionIcon);	
            end
            instance.UnitActionButton:RegisterCallback( Mouse.eLClick, OnUnitActionClicked );
            instance.UnitActionButton:SetVoid1( iAction );
			instance.UnitActionButton:SetToolTipCallback( TipHandler )
           
        end
    end

    --if hasPromotion == true then
        --Controls.PromotionButton:SetHide( false );
    --else
        --Controls.PromotionButton:SetHide( true );
    --end
    Controls.PromotionStack:SetHide( true );
    
    g_PromotionsOpen = false;
    
    Controls.PrimaryStack:CalculateSize();
    Controls.PrimaryStack:ReprocessAnchoring();
    
    local stackSize = Controls.PrimaryStack:GetSize();
    local stretchySize = Controls.PrimaryStretchy:GetSize();
    local buildCityButtonSize = 0;
    if buildCityButtonActive then
		if OptionsManager.GetSmallUIAssets() and not UI.IsTouchScreenEnabled() then
			buildCityButtonSize = 36;
		else
			buildCityButtonSize = 60;
		end
    end
    Controls.PrimaryStretchy:SetSizeVal( stretchySize.x, stackSize.y + buildCityButtonSize + 348 );
    
    if (numPrimaryActions > 0) then
        Controls.PrimaryStack:SetHide( false );
        Controls.PrimaryStretchy:SetHide( false );
        Controls.SecondaryButton:SetHide( false );
      	Controls.SecondaryButton:SetDisabled(numSecondaryActions == 0);
    else
        Controls.PrimaryStack:SetHide( true );
        Controls.PrimaryStretchy:SetHide( true );
        Controls.SecondaryButton:SetHide( true );
    end
    
    if(numSecondaryActions == 0) then
		Controls.SecondaryStack:SetHide(true);
		Controls.SecondaryStretchy:SetHide(true);
    end
    
    Controls.SecondaryStack:CalculateSize();
    Controls.SecondaryStack:ReprocessAnchoring();
    
    stackSize = Controls.SecondaryStack:GetSize();
    stretchySize = Controls.SecondaryStretchy:GetSize();
    Controls.SecondaryStretchy:SetSizeVal( stretchySize.x, stackSize.y + 290 );

    --Controls.BuildStack:CalculateSize();
    --Controls.BuildStack:ReprocessAnchoring();
    if numBuildActions > 0 or hasPromotion then
		Controls.WorkerActionPanel:SetHide( false );
		g_bWorkerActionPanelOpen = true;
		stackSize = Controls.WorkerActionPanel:GetSize();
		local rbOffset = 0;
		if recommendedBuild then
			rbOffset = 60;
			if OptionsManager.GetSmallUIAssets() and not UI.IsTouchScreenEnabled() then
				rbOffset = 60;
			end
			Controls.RecommendedActionDivider:SetHide( false );
			Controls.RecommendedActionButton:SetHide( false );
		else
			rbOffset = 0;
			Controls.RecommendedActionDivider:SetHide( true );
			Controls.RecommendedActionButton:SetHide( true );
		end
		if hasPromotion then
			Controls.WorkerText:SetHide(true);
			Controls.PromotionText:SetHide(false);
			Controls.PromotionAnimation:SetHide(false);
			Controls.EditButton:SetHide(false);
		else
			Controls.WorkerText:SetHide(false);
			Controls.PromotionText:SetHide(true);
			Controls.PromotionAnimation:SetHide(true);
			Controls.EditButton:SetHide(true);
		end
		Controls.WorkerActionPanel:SetSizeVal( stackSize.x, math.floor((numBuildActions-1) / numberOfButtonsPerRow) * buttonSize + buttonPadding + buttonOffsetY + rbOffset + workerPanelSizeOffsetY );
    else
		Controls.WorkerActionPanel:SetHide( true );
		g_bWorkerActionPanelOpen = false;
    end
    
    local buildType = unit:GetBuildType();
    if (buildType ~= -1) then -- this is a worker who is actively building something
		local thisBuild = GameInfo.Builds[buildType];
		--print("thisBuild.Type:"..tostring(thisBuild.Type));
		local civilianUnitStr = Locale.ConvertTextKey(thisBuild.Description);
		local iTurnsLeft = unit:GetPlot():GetBuildTurnsLeft(buildType, Game.GetActivePlayer(),  0, 0) + 1;	
		local iTurnsTotal = unit:GetPlot():GetBuildTurnsTotal(buildType);	
		if (iTurnsLeft < 4000 and iTurnsLeft > 0) then
			civilianUnitStr = civilianUnitStr.." ("..tostring(iTurnsLeft)..")";
		end
		IconHookup( thisBuild.IconIndex, 45, thisBuild.IconAtlas, Controls.WorkerProgressIcon ); 		
		Controls.WorkerProgressLabel:SetText( civilianUnitStr );
		Controls.WorkerProgressLabel:SetToolTipString(civilianUnitStr );
		local percent = (iTurnsTotal - (iTurnsLeft - 1)) / iTurnsTotal;
		Controls.WorkerProgressBar:SetPercent( percent );
		Controls.WorkerProgressIconFrame:SetHide( false );
		Controls.WorkerProgressFrame:SetHide( false );
	else
		Controls.WorkerProgressIconFrame:SetHide( true );
		Controls.WorkerProgressFrame:SetHide( true );
	end
    
    Controls.PromotionStack:CalculateSize();
    Controls.PromotionStack:ReprocessAnchoring();
end

local defaultErrorTextureSheet = "TechAtlasSmall.dds";
local nullOffset = Vector2( 0, 0 );

--------------------------------------------------------------------------------
-- Refresh unit portrait and name
--------------------------------------------------------------------------------
function UpdateUnitPortrait( unit )
  
	local name;
	if(unit:IsGreatPerson()) then
		name = unit:GetNameNoDesc();
		if(name == nil or #name == 0) then
			name = unit:GetName();			
		end
	else
		name = unit:GetName();
	end
	
	name = Locale.ToUpper(name);
  
    --local name = unit:GetNameKey();
    local convertedKey = Locale.ConvertTextKey(name);
    convertedKey = Locale.ToUpper(convertedKey);

    Controls.UnitName:SetText(convertedKey);    
    Controls.UnitName:SetFontByName("TwCenMT24");   
    
    local name_length = Controls.UnitName:GetSizeVal();
    local box_length = Controls.UnitNameButton:GetSizeVal();
    
    if (name_length > (box_length - 50)) then
	    Controls.UnitName:SetFontByName("TwCenMT20");   
	end
	
	name_length = Controls.UnitName:GetSizeVal();
	
	if(name_length > (box_length - 50)) then
		Controls.UnitName:SetFontByName("TwCenMT14");
	end
    
    -- Tool tip
    local strToolTip = Locale.ConvertTextKey("TXT_KEY_CURRENTLY_SELECTED_UNIT");
    
	local cityName = unit:GetCityName();
	if(cityName ~= "") then
		strToolTip = strToolTip .. "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_UNIT_ORIGIN_CITY", cityName )
	end

    if unit:IsCombatUnit() or unit:GetDomainType() == DomainTypes.DOMAIN_AIR then
		local iExperience = unit:GetExperience();
	    
		local iLevel = unit:GetLevel();
		local iExperienceNeeded = unit:ExperienceNeeded();
		local xpString = Locale.ConvertTextKey("TXT_KEY_UNIT_EXPERIENCE_INFO", iLevel, iExperience, iExperienceNeeded);
		Controls.XPMeter:SetToolTipString( xpString );
		Controls.XPMeter:SetPercent( iExperience / iExperienceNeeded );
			
		if (iExperience > 0) then
			strToolTip = strToolTip .. "[NEWLINE][NEWLINE]" .. xpString;
		end
		Controls.XPFrame:SetHide( false );
	else
 		Controls.XPFrame:SetHide( true );
   end
	
	Controls.UnitPortrait:SetToolTipString(strToolTip);
    
    local thisUnitInfo = GameInfo.Units[unit:GetUnitType()];

	local flagOffset, flagAtlas = UI.GetUnitFlagIcon(unit);	    
    local textureOffset, textureAtlas = IconLookup( flagOffset, 32, flagAtlas );
    Controls.UnitIcon:SetTexture(textureAtlas);
    Controls.UnitIconShadow:SetTexture(textureAtlas);
    Controls.UnitIcon:SetTextureOffset(textureOffset);
    Controls.UnitIconShadow:SetTextureOffset(textureOffset);
    
    local pPlayer = Players[ unit:GetOwner() ];
    if (pPlayer ~= nil) then
		local iconColor, flagColor = pPlayer:GetPlayerColors();
	        
		if( pPlayer:IsMinorCiv() ) then
			flagColor, iconColor = flagColor, iconColor;
		end

		Controls.UnitIcon:SetColor( iconColor );
		Controls.UnitIconBackground:SetColor( flagColor );
	end    
    
	local portraitOffset, portraitAtlas = UI.GetUnitPortraitIcon(unit);
    textureOffset, textureAtlas = IconLookup( portraitOffset, unitPortraitSize, portraitAtlas );
    if textureOffset == nil then
		textureOffset = nullOffset;
		textureAtlas = defaultErrorTextureSheet;
    end
    Controls.UnitPortrait:SetTexture(textureAtlas);
    Controls.UnitPortrait:SetTextureOffset(textureOffset);
    
    --These controls are potentially hidden if the previous selection was a city.
	Controls.UnitTypeFrame:SetHide(false);
	Controls.CycleLeft:SetHide(false);
	Controls.CycleRight:SetHide(false);
    Controls.UnitMovementBox:SetHide(false);
  
end

function UpdateCityPortrait(city)
    local name = city:GetName();
    name = Locale.ToUpper(name);
    --local name = unit:GetNameKey();
    local convertedKey = Locale.ConvertTextKey(name);
    convertedKey = Locale.ToUpper(convertedKey);

    Controls.UnitName:SetText(convertedKey);    
    Controls.UnitName:SetFontByName("TwCenMT24");   
    
    local name_length = Controls.UnitName:GetSizeVal();
    local box_length = Controls.UnitNameButton:GetSizeVal();
    
    if (name_length > (box_length - 50)) then
	    Controls.UnitName:SetFontByName("TwCenMT20");   
	end
	
	name_length = Controls.UnitName:GetSizeVal();
	
	if(name_length > (box_length - 50)) then
		Controls.UnitName:SetFontByName("TwCenMT14");
	end
    
	Controls.UnitPortrait:SetToolTipString(nil);
    
    local textureOffset, textureAtlas = IconLookup( 0, unitPortraitSize, "CITY_ATLAS" );
    if textureOffset == nil then
		textureOffset = nullOffset;
		textureAtlas = defaultErrorTextureSheet;
    end
    Controls.UnitPortrait:SetTexture(textureAtlas);
    Controls.UnitPortrait:SetTextureOffset(textureOffset);
    
        
    --Hide various aspects of Unit Panel since they don't apply to the city.
    --Clear promotions
    g_EarnedPromotionIM:ResetInstances();
    
    Controls.UnitTypeFrame:SetHide(true);
    Controls.CycleLeft:SetHide(true);
    Controls.CycleRight:SetHide(true);
    Controls.XPFrame:SetHide( true );
    Controls.SecondaryButton:SetHide(true);
    Controls.UnitMovementBox:SetHide(true);
    Controls.UnitStrengthBox:SetHide(true);
    Controls.UnitRangedAttackBox:SetHide(true);
    Controls.WorkerActionPanel:SetHide(true);
	Controls.PrimaryStack:SetHide( true );
	Controls.PrimaryStretchy:SetHide( true );
	Controls.SecondaryStack:SetHide( true );
	Controls.SecondaryStretchy:SetHide( true );
	Controls.SecondaryImageOpen:SetHide( false );
	Controls.SecondaryImageClosed:SetHide( true );
	Controls.WorkerProgressIconFrame:SetHide( true );
	Controls.WorkerProgressFrame:SetHide( true );
	g_bWorkerActionPanelOpen = false;
	g_PromotionsOpen = false;
	g_SecondaryOpen = false;
end


--------------------------------------------------------------------------------
-- Refresh unit promotions
--------------------------------------------------------------------------------
function UpdateUnitPromotions(unit)
    
    g_EarnedPromotionIM:ResetInstances();
    local controlTable;
    
    --For each avail promotion, display the icon
    for unitPromotion in GameInfo.UnitPromotions() do
        local unitPromotionID = unitPromotion.ID;
        
        if (unit:IsHasPromotion(unitPromotionID) and not unit:IsTrade()) then
            
            controlTable = g_EarnedPromotionIM:GetInstance();
			IconHookup( unitPromotion.PortraitIndex, 32, unitPromotion.IconAtlas, controlTable.UnitPromotionImage );

            -- Tooltip
			local sDurationTip = ""
			if unit:GetPromotionDuration(unitPromotionID) > 0 then
				sDurationTip = " (" .. Locale.ConvertTextKey("TXT_KEY_STR_TURNS", unit:GetPromotionDuration(unitPromotionID) - (Game.GetGameTurn() - unit:GetTurnPromotionGained(unitPromotionID))) .. ")"
			end
            local strToolTip = Locale.ConvertTextKey(unitPromotion.Description);
            strToolTip = strToolTip .. sDurationTip .. "[NEWLINE][NEWLINE]" .. Locale.ConvertTextKey(unitPromotion.Help)
            controlTable.UnitPromotionImage:SetToolTipString(strToolTip);
            
        end
    end
end

---------------------------------------------------
---- Promotion Help
---------------------------------------------------
--function PromotionHelpOpen(iPromotionID)
    --local pPromotionInfo = GameInfo.UnitPromotions[iPromotionID];
    --local promotionHelp = Locale.ConvertTextKey(pPromotionInfo.Description);
    --Controls.HelpText:SetText(promotionHelp);
--end

--------------------------------------------------------------------------------
-- Refresh unit stats
--------------------------------------------------------------------------------
function UpdateUnitStats(unit)
    
    -- update the background image (update this if we get icons for the minors)
    local civType = unit:GetCivilizationType();
	local civInfo = GameInfo.Civilizations[civType];
	local civPortraitIndex = civInfo.PortraitIndex;
    if civPortraitIndex < 0 or civPortraitIndex > 21 then
        civPortraitIndex = 22;
    end
    
    IconHookup( civPortraitIndex, 128, civInfo.IconAtlas, Controls.BackgroundCivSymbol );
       
    -- Movement
    if unit:GetDomainType() == DomainTypes.DOMAIN_AIR then
		local iRange = unit:Range();
		local szMoveStr = iRange .. " [ICON_MOVES]";	    
		Controls.UnitStatMovement:SetText(szMoveStr);
		Controls.UnitStatNameMovement:LocalizeAndSetText("TXT_KEY_UPANEL_RANGEMOVEMENT");
		
		local rebaseRange = iRange * GameDefines.AIR_UNIT_REBASE_RANGE_MULTIPLIER;
		rebaseRange = rebaseRange / 100;
		
		szMoveStr = Locale.ConvertTextKey( "TXT_KEY_UPANEL_UNIT_MAY_STRIKE_REBASE", iRange, rebaseRange );
		Controls.UnitStatNameMovement:SetToolTipString(szMoveStr);
		Controls.UnitStatMovement:SetToolTipString(szMoveStr);

    else
		local move_denominator = GameDefines["MOVE_DENOMINATOR"];
		local moves_left = unit:MovesLeft() / move_denominator;
		local max_moves = unit:MaxMoves() / move_denominator;
		local szMoveStr = math.floor(moves_left) .. "/" .. math.floor(max_moves) .. " [ICON_MOVES]";
	    
		Controls.UnitStatMovement:SetText(szMoveStr);
	    
		szMoveStr = Locale.ConvertTextKey( "TXT_KEY_UPANEL_UNIT_MAY_MOVE", moves_left );
		Controls.UnitStatNameMovement:LocalizeAndSetText("TXT_KEY_UPANEL_MOVEMENT");
		Controls.UnitStatNameMovement:SetToolTipString(szMoveStr);
		Controls.UnitStatMovement:SetToolTipString(szMoveStr);
    end
    
    -- Strength
    local strength = 0;
    if(unit:GetDomainType() == DomainTypes.DOMAIN_AIR) then
        strength = unit:GetBaseRangedCombatStrength();
    elseif (not unit:IsEmbarked()) then
        strength = unit:GetBaseCombatStrength();
    end
    if(strength > 0) then
        strength = strength .. " [ICON_STRENGTH]";
        Controls.UnitStrengthBox:SetHide(false);
        Controls.UnitStatStrength:SetText(strength);
        local strengthTT = Locale.ConvertTextKey( "TXT_KEY_UPANEL_STRENGTH_TT" );
        Controls.UnitStatStrength:SetToolTipString(strengthTT);
        Controls.UnitStatNameStrength:SetToolTipString(strengthTT);
   -- Religious units
    elseif (unit:GetSpreadsLeft() > 0) then
        strength = math.floor(unit:GetConversionStrength()/GameDefines["RELIGION_MISSIONARY_PRESSURE_MULTIPLIER"]) .. " [ICON_PEACE]";
        Controls.UnitStrengthBox:SetHide(false);
        Controls.UnitStatStrength:SetText(strength);    
        local strengthTT = Locale.ConvertTextKey( "TXT_KEY_UPANEL_RELIGIOUS_STRENGTH_TT" );
        Controls.UnitStatStrength:SetToolTipString(strengthTT);
        Controls.UnitStatNameStrength:SetToolTipString(strengthTT);
    elseif (unit:GetTourismBlastStrength() > 0) then
        strength = unit:GetTourismBlastStrength() .. " [ICON_TOURISM]";
        Controls.UnitStrengthBox:SetHide(false);
        Controls.UnitStatStrength:SetText(strength);    
        local strengthTT = Locale.ConvertTextKey( "TXT_KEY_UPANEL_TOURISM_STRENGTH_TT" );
        Controls.UnitStatStrength:SetToolTipString(strengthTT);
        Controls.UnitStatNameStrength:SetToolTipString(strengthTT);    
    else
        Controls.UnitStrengthBox:SetHide(true);
    end        
    
    -- Ranged Strength
    local iRangedStrength = 0;
    if(unit:GetDomainType() ~= DomainTypes.DOMAIN_AIR) then
        iRangedStrength = unit:GetBaseRangedCombatStrength();
    else
        iRangedStrength = 0;
    end
    if(iRangedStrength > 0) then
        local szRangedStrength = iRangedStrength .. " [ICON_RANGE_STRENGTH]";
        Controls.UnitRangedAttackBox:SetHide(false);
        local rangeStrengthStr = Locale.ConvertTextKey( "TXT_KEY_UPANEL_RANGED_ATTACK" );
        Controls.UnitStatNameRangedAttack:SetText(rangeStrengthStr);
        Controls.UnitStatRangedAttack:SetText(szRangedStrength);
        local rangeStrengthTT = Locale.ConvertTextKey( "TXT_KEY_UPANEL_RANGED_ATTACK_TT" );
        Controls.UnitStatRangedAttack:SetToolTipString(rangeStrengthTT);
        Controls.UnitStatNameRangedAttack:SetToolTipString(rangeStrengthTT);
    -- Religious unit
    elseif (unit:GetSpreadsLeft() > 0) then
        iRangedStrength = unit:GetSpreadsLeft() .. "      ";
        Controls.UnitRangedAttackBox:SetHide(false);
        local rangeStrengthStr = Locale.ConvertTextKey( "TXT_KEY_UPANEL_SPREAD_RELIGION_USES" );
        Controls.UnitStatNameRangedAttack:SetText(rangeStrengthStr);
        Controls.UnitStatRangedAttack:SetText(iRangedStrength);    
        local rangeStrengthTT = Locale.ConvertTextKey( "TXT_KEY_UPANEL_SPREAD_RELIGION_USES_TT" );
        Controls.UnitStatRangedAttack:SetToolTipString(rangeStrengthTT);
        Controls.UnitStatNameRangedAttack:SetToolTipString(rangeStrengthTT);
	elseif (unit:GetChargesLeft() > 0) then
        iRangedStrength = unit:GetChargesLeft() .. "      ";
        Controls.UnitRangedAttackBox:SetHide(false);
        local rangeStrengthStr = Locale.ConvertTextKey( "TXT_KEY_UPANEL_REPAIR_CHARGES" );
        Controls.UnitStatNameRangedAttack:SetText(rangeStrengthStr);
        Controls.UnitStatRangedAttack:SetText(iRangedStrength);    
        local rangeStrengthTT = Locale.ConvertTextKey( "TXT_KEY_UPANEL_REPAIR_CHARGES_TT" );
        Controls.UnitStatRangedAttack:SetToolTipString(rangeStrengthTT);
        Controls.UnitStatNameRangedAttack:SetToolTipString(rangeStrengthTT);
    elseif (GameInfo.Units[unit:GetUnitType()].RemoveHeresy) then
        iRangedStrength = 1;
        Controls.UnitRangedAttackBox:SetHide(false);
        local rangeStrengthStr = Locale.ConvertTextKey( "TXT_KEY_UPANEL_REMOVE_HERESY_USES" );
        Controls.UnitStatNameRangedAttack:SetText(rangeStrengthStr);
        Controls.UnitStatRangedAttack:SetText(iRangedStrength);    
        local rangeStrengthTT = Locale.ConvertTextKey( "TXT_KEY_UPANEL_REMOVE_HERESY_USES_TT" );
        Controls.UnitStatRangedAttack:SetToolTipString(rangeStrengthTT);
        Controls.UnitStatNameRangedAttack:SetToolTipString(rangeStrengthTT);
    elseif unit:GetBuilderStrength() > 0 then
		iRangedStrength = unit:GetBuilderStrength() .. "      ";
		Controls.UnitRangedAttackBox:SetHide(false);
		local rangeStrengthStr = Locale.ConvertTextKey( "TXT_KEY_UPANEL_BUILDER_STRENGTH" );
		Controls.UnitStatNameRangedAttack:SetText(rangeStrengthStr);
        Controls.UnitStatRangedAttack:SetText(iRangedStrength);
		local rangeStrengthTT = Locale.ConvertTextKey( "TXT_KEY_UPANEL_BUILDER_STRENGTH_TT" );
        Controls.UnitStatRangedAttack:SetToolTipString(rangeStrengthTT);
        Controls.UnitStatNameRangedAttack:SetToolTipString(rangeStrengthTT);
	else
        Controls.UnitRangedAttackBox:SetHide(true);
    end        
    
end

--------------------------------------------------------------------------------
-- Refresh unit health bar
--------------------------------------------------------------------------------
function UpdateUnitHealthBar(unit)
	-- note that this doesn't use the bar type
	local damage = unit:GetDamage();
	if damage == 0 then
		Controls.HealthBar:SetHide(true);	
	else
		MaxDamage = unit:GetMaxHitPoints();
		local healthPercent = 1.0 - (damage / MaxDamage);
		local healthTimes100 =  math.floor(100 * healthPercent + 0.5);
		local barSize = { x = 9, y = math.floor(123 * healthPercent) };
		if healthTimes100 <= 33 then
			Controls.RedBar:SetSize(barSize);
			Controls.RedAnim:SetSize(barSize);
			Controls.GreenBar:SetHide(true);
			Controls.YellowBar:SetHide(true);
			Controls.RedBar:SetHide(false);
		elseif healthTimes100 <= 66 then
			Controls.YellowBar:SetSize(barSize);
			Controls.GreenBar:SetHide(true);
			Controls.YellowBar:SetHide(false);
			Controls.RedBar:SetHide(true);
		else
			Controls.GreenBar:SetSize(barSize);
			Controls.GreenBar:SetHide(false);
			Controls.YellowBar:SetHide(true);
			Controls.RedBar:SetHide(true);
		end
		
		Controls.HealthBar:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_UPANEL_SET_HITPOINTS_TT",(MaxDamage-damage), MaxDamage ) );
		--Controls.HealthBar:SetToolTipString(healthPercent.." Hit Points");
		
		Controls.HealthBar:SetHide(false);
	end
end

--------------------------------------------------------------------------------
-- Event Handlers
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
-- CycleLeft clicked event handler
--------------------------------------------------------------------------------
function OnCycleLeftClicked()
    -- Cycle to next selection.
    Game.CycleUnits(true, true, false);
end
Controls.CycleLeft:RegisterCallback( Mouse.eLClick, OnCycleLeftClicked );


--------------------------------------------------------------------------------
-- CycleRight clicked event handler
--------------------------------------------------------------------------------
function OnCycleRightClicked()
    -- Cycle to previous selection.
    Game.CycleUnits(true, false, false);
end
Controls.CycleRight:RegisterCallback( Mouse.eLClick, OnCycleRightClicked );

--------------------------------------------------------------------------------
-- Unit Name clicked event handler
--------------------------------------------------------------------------------
function OnUnitNameClicked()
    -- go to this unit
    UI.LookAtSelectionPlot(0);
end
Controls.UnitNameButton:RegisterCallback( Mouse.eLClick, OnUnitNameClicked );
Controls.UnitPortraitButton:RegisterCallback( Mouse.eLClick, OnUnitNameClicked );



function OnUnitRClicked()
	local unit = UI.GetHeadSelectedUnit();
	if unit then
	
		-- search by name
		local searchString = Locale.ConvertTextKey( unit:GetNameKey() );
		Events.SearchForPediaEntry( searchString );
	end
end
Controls.UnitPortraitButton:RegisterCallback( Mouse.eRClick, OnUnitRClicked );

--------------------------------------------------------------------------------
-- InfoPane is now dirty.
--------------------------------------------------------------------------------
function OnInfoPaneDirty()
    
    -- Retrieve the currently selected unit.
    local unit = UI.GetHeadSelectedUnit();
    local name = unit and unit:GetNameKey() or "unit is nil";
    local convertedKey = Locale.ConvertTextKey(name);
    
    local unitID = unit and unit:GetID() or -1;
    -- Unit is different than last unit.
    if(unitID ~= g_lastUnitID) then
        local playerID = Game.GetActivePlayer();
        local unitPosition = {
            x = unit and unit:GetX() or 0,
            y = unit and unit:GetY() or 0,
        };
        local hexPosition = ToHexFromGrid(unitPosition);
        
        if(g_lastUnitID ~= -1) then
            Events.UnitSelectionChanged(playerID, g_lastUnitID, 0, 0, 0, false, false);
        end
        
        if(unitID ~= -1) then
            Events.UnitSelectionChanged(playerID, unitID, hexPosition.x, hexPosition.y, 0, true, false);
        end
        
        g_SecondaryOpen = false;
        Controls.PrimaryStack:SetHide( true );
        Controls.PrimaryStretchy:SetHide( true );
        Controls.SecondaryStack:SetHide( true );
        Controls.SecondaryStretchy:SetHide( true );
        Controls.SecondaryImageOpen:SetHide( false );
        Controls.SecondaryImageClosed:SetHide( true );
        Controls.SecondaryButton:SetToolTipString( Locale.ConvertTextKey( "TXT_KEY_SECONDARY_C_TEXT" ));
    end
    g_lastUnitID = unitID;
    
    if (unit ~= nil) then
        UpdateUnitActions(unit);
        UpdateUnitPortrait(unit);
        UpdateUnitPromotions(unit);
        UpdateUnitStats(unit);
        UpdateUnitHealthBar(unit);
        ContextPtr:SetHide( false );
    else
		-- Attempt to show currently selected city.
		local city = UI.GetHeadSelectedCity();
		if(city ~= nil) then
	
			UpdateCityPortrait(city);
				
		    ContextPtr:SetHide( false );
    	else
		    ContextPtr:SetHide( true );
    	end
    
    end
    
end
Events.SerialEventUnitInfoDirty.Add(OnInfoPaneDirty);


local g_iPortraitSize = 256;
local bOkayToProcess = true;

--------------------------------------------------------------------------------
-- UnitAction<idx> was clicked.
--------------------------------------------------------------------------------
function OnUnitActionClicked( action )
	if bOkayToProcess then
		if (GameInfoActions[action].SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION) then
			Events.AudioPlay2DSound("AS2D_INTERFACE_UNIT_PROMOTION");	
		end
		Game.HandleAction( action );
    end
end

function OnActivePlayerTurnEnd()
	bOkayToProcess = false;
end
Events.ActivePlayerTurnEnd.Add( OnActivePlayerTurnEnd );

function OnActivePlayerTurnStart()
	bOkayToProcess = true;
end
Events.ActivePlayerTurnStart.Add( OnActivePlayerTurnStart );

-------------------------------------------------
-------------------------------------------------
function OnEditNameClick()
	
	if UI.GetHeadSelectedUnit() then
		local popupInfo = {
				Type = ButtonPopupTypes.BUTTONPOPUP_RENAME_UNIT,
				Data1 = UI.GetHeadSelectedUnit():GetID(),
				Data2 = -1,
				Data3 = -1,
				Option1 = false,
				Option2 = false;
			}
		Events.SerialEventGameMessagePopup(popupInfo);
	end
end
Controls.EditButton:RegisterCallback( Mouse.eLClick, OnEditNameClick );

------------------------------------------------------
------------------------------------------------------
local tipControlTable = {};
TTManager:GetTypeControlTable( "TypeUnitAction", tipControlTable );
function TipHandler( control )
	
	local unit = UI.GetHeadSelectedUnit();
	if not unit then
		return
	end
	
	local iAction = control:GetVoid1();
    local action = GameInfoActions[iAction];
    
    local iActivePlayer = Game.GetActivePlayer();
    local pActivePlayer = Players[iActivePlayer];
    local iActiveTeam = Game.GetActiveTeam();
    local pActiveTeam = Teams[iActiveTeam];
    
    local pPlot = unit:GetPlot();
    
    local bBuild = false;

    local bDisabled = false;
	
	-- Build data
	local iBuildID = action.MissionData;
	local pBuild = GameInfo.Builds[iBuildID];
	local strBuildType= "";
	if (pBuild) then
		strBuildType = pBuild.Type;
	end
	
	-- Improvement data
	local iImprovement = -1;
	local pImprovement;
	
	if (pBuild) then
		iImprovement = pBuild.ImprovementType;
		
		if (iImprovement and iImprovement ~= "NONE") then
			pImprovement = GameInfo.Improvements[iImprovement];
			iImprovement = pImprovement.ID;
		end
	end
    
    -- Feature data
	local iFeature = unit:GetPlot():GetFeatureType();
	local pFeature = GameInfo.Features[iFeature];
	local strFeatureType;
	if (pFeature) then
		strFeatureType = pFeature.Type;
	end
	
	-- Route data
	local iRoute = -1;
	local pRoute;
	
	if (pBuild) then
		iRoute = pBuild.RouteType
		
		if (iRoute and iRoute ~= "NONE") then
			pRoute = GameInfo.Routes[iRoute];
			iRoute = pRoute.ID;
		end
	end
	
	local strBuildTurnsString = "";
	local strBuildResourceConnectionString = "";
	local strClearFeatureString = "";
	local strBuildYieldString = "";
	
	local bFirstEntry = true;
	
	local strToolTip = "";
	
    local strDisabledString = "";
    
    local strActionHelp = "";
    
    -- Not able to perform action
    if not Game.CanHandleAction( iAction ) then
		bDisabled = true;
	end
    
    -- Upgrade has special help text
    if (action.Type == "COMMAND_UPGRADE") then
		
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strActionHelp = strActionHelp .. "[NEWLINE]";
		end
		
		strActionHelp = strActionHelp .. "[NEWLINE]";
		
		local iUnitType = unit:GetUpgradeUnitType();
		local iGoldToUpgrade = unit:UpgradePrice(iUnitType);
		strActionHelp = strActionHelp .. Locale.ConvertTextKey("TXT_KEY_UPGRADE_HELP", GameInfo.Units[iUnitType].Description, iGoldToUpgrade);
		
        strToolTip = strToolTip .. strActionHelp;
        
		if bDisabled then
			
			local pActivePlayer = Players[Game.GetActivePlayer()];
			
			-- Can't upgrade because we're outside our territory
			if (pPlot:GetOwner() ~= unit:GetOwner()) then
				
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_UPGRADE_HELP_DISABLED_TERRITORY");
			end
			
			-- Can't upgrade because we're outside of a city
			if (unit:GetDomainType() == DomainTypes.DOMAIN_AIR and not pPlot:IsCity()) then
				
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_UPGRADE_HELP_DISABLED_CITY");
			end
			
			-- Can't upgrade because we lack the Gold
			if (iGoldToUpgrade > pActivePlayer:GetGold()) then
				
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_UPGRADE_HELP_DISABLED_GOLD");
			end
			
			-- Can't upgrade because we lack the Resources
			local strResourcesNeeded = "";
			
			local iNumResourceNeededToUpgrade;
			local iResourceLoop;
			
			-- Loop through all resources to see how many we need. If it's > 0 then add to the string
			for pResource in GameInfo.Resources() do
				iResourceLoop = pResource.ID;
				
				iNumResourceNeededToUpgrade = unit:GetNumResourceNeededToUpgrade(iResourceLoop);
				
				if (iNumResourceNeededToUpgrade > 0 and iNumResourceNeededToUpgrade > pActivePlayer:GetNumResourceAvailable(iResourceLoop)) then
					-- Add separator for non-initial entries
					if (strResourcesNeeded ~= "") then
						strResourcesNeeded = strResourcesNeeded .. ", ";
					end
					
					strResourcesNeeded = strResourcesNeeded .. iNumResourceNeededToUpgrade .. " " .. pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description);
				end

				if Game.IsCustomModOption("UNITS_RESOURCE_QUANTITY_TOTALS") then
					local iNumResourcesTotalNeeded = unit:GetNumResourceTotalNeededToUpgrade(iResourceLoop)
					
					if iNumResourcesTotalNeeded > 0 and iNumResourcesTotalNeeded > pActivePlayer:GetNumResourceTotal(iResourceLoop) then
						-- Add separator for non-initial entries
						if (strResourcesTotalNeeded ~= "") then
							strResourcesTotalNeeded = strResourcesTotalNeeded .. ", ";
						end
						strResourcesTotalNeeded = strResourcesTotalNeeded .. iNumResourcesTotalNeeded .. " " .. pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description);
					elseif iNumResourcesTotalNeeded > 0 and pActivePlayer:GetNumResourceAvailable(iResourceLoop) < 0 then
						-- Add separator for non-initial entries
						if (strResourcesNetPositiveNeeded ~= "") then
							strResourcesNetPositiveNeeded = strResourcesNetPositiveNeeded .. ", ";
						end
						strResourcesNetPositiveNeeded = strResourcesNetPositiveNeeded .. pResource.IconString .. " " .. Locale.ConvertTextKey(pResource.Description);
					end
				end
			end
			
			-- Build resources required string
			if (strResourcesNeeded ~= "") then
				
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_UPGRADE_HELP_DISABLED_RESOURCES", strResourcesNeeded);
			end

			if Game.IsCustomModOption("UNITS_RESOURCE_QUANTITY_TOTALS") and (strResourcesTotalNeeded ~= "") then
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_UPGRADE_HELP_DISABLED_GROSS_RESOURCES", strResourcesTotalNeeded);
			end

			if Game.IsCustomModOption("UNITS_RESOURCE_QUANTITY_TOTALS") and (strResourcesNetPositiveNeeded ~= "") then
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_UPGRADE_HELP_DISABLED_RESOURCES_NET_NEGATIVE", strResourcesNetPositiveNeeded);
			end
    
    	        -- if we can't upgrade due to stacking
	        if (pPlot:GetNumFriendlyUnitsOfType(unit) > 1) then
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_UPGRADE_HELP_DISABLED_STACKING");

	        end
    
	        strDisabledString = "[COLOR_WARNING_TEXT]" .. strDisabledString .. "[ENDCOLOR]";	        
		end
    end
    
    if (action.Type == "MISSION_ALERT" and not unit:IsEverFortifyable()) then
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strActionHelp = strActionHelp .. "[NEWLINE]";
		end

		strActionHelp = "[NEWLINE]" .. Locale.ConvertTextKey("TXT_KEY_MISSION_ALERT_NO_FORTIFY_HELP");
		strToolTip = strToolTip .. strActionHelp;

    -- Golden Age has special help text
    elseif (action.Type == "MISSION_GOLDEN_AGE") then
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strActionHelp = strActionHelp .. "[NEWLINE]";
		end
		
		local iGALength = unit:GetGoldenAgeTurns();
		strActionHelp = "[NEWLINE]" .. Locale.ConvertTextKey( "TXT_KEY_MISSION_START_GOLDENAGE_HELP", iGALength );
        strToolTip = strToolTip .. strActionHelp;
		
    -- Spread Religion has special help text
    elseif (action.Type == "MISSION_SPREAD_RELIGION") then
    
		local iNumFollowers = unit:GetNumFollowersAfterSpread();
		local religionName = Game.GetReligionName(unit:GetReligion());
		
        strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_MISSION_SPREAD_RELIGION_HELP");
        strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]";
        strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_MISSION_SPREAD_RELIGION_RESULT", religionName, iNumFollowers);
		strToolTip = strToolTip .. " ";  
		    
 		local eMajorityReligion = unit:GetMajorityReligionAfterSpread();
        if (eMajorityReligion < ReligionTypes.RELIGION_PANTHEON) then
			strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_MISSION_MAJORITY_RELIGION_NONE");
		else
			local majorityReligionName = Locale.Lookup(Game.GetReligionName(eMajorityReligion));
			strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_MISSION_MAJORITY_RELIGION", majorityReligionName);		
		end

    -- Create Great Work has special help text
	elseif (action.Type == "MISSION_CREATE_GREAT_WORK") then

		strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_MISSION_CREATE_GREAT_WORK_HELP");
		strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]";
	    
		if (not bDisabled) then
			local pActivePlayer = Players[Game.GetActivePlayer()];
			local eGreatWorkSlotType = unit:GetGreatWorkSlotType();
			local iBuilding = pActivePlayer:GetBuildingOfClosestGreatWorkSlot(unit:GetX(), unit:GetY(), eGreatWorkSlotType);
			local pCity = pActivePlayer:GetCityOfClosestGreatWorkSlot(unit:GetX(), unit:GetY(), eGreatWorkSlotType);	
			strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_MISSION_CREATE_GREAT_WORK_RESULT", GameInfo.Buildings[iBuilding].Description, pCity:GetNameKey());
		end
	
	-- Paradrop
	elseif (action.Type == "INTERFACEMODE_PARADROP") then
		strToolTip = Locale.Lookup("TXT_KEY_INTERFACEMODE_PARADROP_HELP_WITH_RANGE", unit:GetDropRange());

	-- Sell Exotic Goods
	elseif (action.Type == "MISSION_SELL_EXOTIC_GOODS") then
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strToolTip = strToolTip .. "[NEWLINE]";
		end
		
		strToolTip = strToolTip .. "[NEWLINE]" .. Locale.Lookup("TXT_KEY_MISSION_SELL_EXOTIC_GOODS_HELP");
		
		if (not bDisabled) then
			strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]";
			strToolTip = strToolTip .. "+" .. unit:GetExoticGoodsGoldAmount() .. "[ICON_GOLD]";
			strToolTip = strToolTip .. "[NEWLINE]";
			strToolTip = strToolTip .. Locale.ConvertTextKey("TXT_KEY_EXPERIENCE_POPUP", unit:GetExoticGoodsXPAmount());
		end
	
	-- Great Scientist
	elseif (action.Type == "MISSION_DISCOVER") then
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strToolTip = strToolTip .. "[NEWLINE]";
		end
		
		strToolTip = strToolTip .. "[NEWLINE]" .. Locale.Lookup("TXT_KEY_MISSION_DISCOVER_TECH_HELP");
		
		if (not bDisabled) then
			strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]";
			strToolTip = strToolTip .. "+" .. unit:GetDiscoverAmount() .. "[ICON_RESEARCH]";
		end
		
	-- Great Engineer
	elseif (action.Type == "MISSION_HURRY") then
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strToolTip = strToolTip .. "[NEWLINE]";
		end
		
		strToolTip = strToolTip .. "[NEWLINE]" .. Locale.Lookup("TXT_KEY_MISSION_HURRY_PRODUCTION_HELP");
		
		if (not bDisabled) then
			strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]";
			strToolTip = strToolTip .. "+" .. unit:GetHurryProduction(unit:GetPlot()) .. "[ICON_PRODUCTION]";
		end
	
	-- Great Merchant
	elseif (action.Type == "MISSION_TRADE") then
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strToolTip = strToolTip .. "[NEWLINE]";
		end
		
		strToolTip = strToolTip .. "[NEWLINE]" .. Locale.Lookup("TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP");
		
		if (not bDisabled) then
			strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]";
			strToolTip = strToolTip .. "+" .. unit:GetTradeInfluence(unit:GetPlot()) .. " [ICON_INFLUENCE]";
			strToolTip = strToolTip .. "[NEWLINE]";
			strToolTip = strToolTip .. "+" .. unit:GetTradeGold(unit:GetPlot()) .. "[ICON_GOLD]";
		end
		
	-- Great Writer
	elseif (action.Type == "MISSION_GIVE_POLICIES") then
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strToolTip = strToolTip .. "[NEWLINE]";
		end
		
		strToolTip = strToolTip .. "[NEWLINE]" .. Locale.Lookup("TXT_KEY_MISSION_GIVE_POLICIES_HELP");
		
		if (not bDisabled) then
			strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]";
			strToolTip = strToolTip .. "+" .. unit:GetGivePoliciesCulture() .. "[ICON_CULTURE]";
		end
	
	-- Great Musician
	elseif (action.Type == "MISSION_ONE_SHOT_TOURISM") then
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strToolTip = strToolTip .. "[NEWLINE]";
		end
		
		strToolTip = strToolTip .. "[NEWLINE]" .. Locale.Lookup("TXT_KEY_MISSION_ONE_SHOT_TOURISM_HELP");
		
		if (not bDisabled) then
			strToolTip = strToolTip .. "[NEWLINE]----------------[NEWLINE]";
			strToolTip = strToolTip .. "+" .. unit:GetBlastTourism() .. "[ICON_TOURISM]";
		end
		
    -- Help text
    elseif (action.Help and action.Help ~= "") then
		
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strActionHelp = strActionHelp .. "[NEWLINE]";
		end
		
		strActionHelp = "[NEWLINE]" .. Locale.ConvertTextKey( action.Help );
        strToolTip = strToolTip .. strActionHelp;
	end
    
    -- Delete has special help text
    if (action.Type == "COMMAND_DELETE") then
		
		strActionHelp = "";
		
		-- Add spacing for all entries after the first
		if (bFirstEntry) then
			bFirstEntry = false;
		elseif (not bFirstEntry) then
			strActionHelp = strActionHelp .. "[NEWLINE]";
		end
		
		strActionHelp = strActionHelp .. "[NEWLINE]";
		
		local iGoldToScrap = unit:GetScrapGold();
		
		strActionHelp = strActionHelp .. Locale.ConvertTextKey("TXT_KEY_SCRAP_HELP", iGoldToScrap);
		
        strToolTip = strToolTip .. strActionHelp;
	end
	
	-- Build?
    if (action.SubType == ActionSubTypes.ACTIONSUBTYPE_BUILD) then
		bBuild = true;
	end
	
    -- Not able to perform action
    if (bDisabled) then
		
		-- Worker build
		if (bBuild) then
			
			-- Figure out what the name of the thing is that we're looking at
			local strImpRouteKey = "";
			if (pImprovement) then
				strImpRouteKey = pImprovement.Description;
			elseif (pRoute) then
				strImpRouteKey = pRoute.Description;
			end
			
			-- Don't have Tech for Build?
			if (pBuild.PrereqTech ~= nil) then
				local pPrereqTech = GameInfo.Technologies[pBuild.PrereqTech];
				local iPrereqTech = pPrereqTech.ID;
				if (iPrereqTech ~= -1 and not pActiveTeam:GetTeamTechs():HasTech(iPrereqTech)) then
					
					-- Must not be a build which constructs something
					if (pImprovement or pRoute) then
						
						-- Add spacing for all entries after the first
						if (bFirstEntry) then
							bFirstEntry = false;
						elseif (not bFirstEntry) then
							strDisabledString = strDisabledString .. "[NEWLINE]";
						end
						
						strDisabledString = strDisabledString .. "[NEWLINE]";
						strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_BUILD_BLOCKED_PREREQ_TECH", pPrereqTech.Description, strImpRouteKey);
					end
				end
			end
			
			-- Trying to build something and are not adjacent to our territory?
			if (pImprovement and pImprovement.InAdjacentFriendly) then
				if (pPlot:GetTeam() ~= unit:GetTeam()) then
					if (not pPlot:IsAdjacentTeam(unit:GetTeam(), true)) then

					
						-- Add spacing for all entries after the first
						if (bFirstEntry) then
							bFirstEntry = false;
						elseif (not bFirstEntry) then
							strDisabledString = strDisabledString .. "[NEWLINE]";
						end
					
						strDisabledString = strDisabledString .. "[NEWLINE]";
						strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_BUILD_BLOCKED_NOT_IN_ADJACENT_TERRITORY", strImpRouteKey);
					end
				end
				
			-- Trying to build something in a City-State's territory?
			elseif (pImprovement and pImprovement.OnlyCityStateTerritory) then
				local bCityStateTerritory = false;
				if (pPlot:IsOwned()) then
					local pPlotOwner = Players[pPlot:GetOwner()];
					if (pPlotOwner and pPlotOwner:IsMinorCiv()) then
						bCityStateTerritory = true;
					end
				end
				
				if (not bCityStateTerritory) then
					-- Add spacing for all entries after the first
					if (bFirstEntry) then
						bFirstEntry = false;
					elseif (not bFirstEntry) then
						strDisabledString = strDisabledString .. "[NEWLINE]";
					end
					
					strDisabledString = strDisabledString .. "[NEWLINE]";
					strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_BUILD_BLOCKED_NOT_IN_CITY_STATE_TERRITORY", strImpRouteKey);
				end	

			-- Trying to build something outside of our territory?
			elseif (pImprovement and not pImprovement.OutsideBorders) then
				if (pPlot:GetTeam() ~= unit:GetTeam()) then
				
					
					-- Add spacing for all entries after the first
					if (bFirstEntry) then
						bFirstEntry = false;
					elseif (not bFirstEntry) then
						strDisabledString = strDisabledString .. "[NEWLINE]";
					end
					
					strDisabledString = strDisabledString .. "[NEWLINE]";
					strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_BUILD_BLOCKED_OUTSIDE_TERRITORY", strImpRouteKey);
				end
			end
			
			-- Trying to build something that requires an adjacent luxury?
			if (pImprovement and pImprovement.AdjacentLuxury) then
				local bAdjacentLuxury = false;

				for loop, direction in ipairs(direction_types) do
					local adjacentPlot = Map.PlotDirection(pPlot:GetX(), pPlot:GetY(), direction);
					if (adjacentPlot ~= nil) then
						local eResourceType = adjacentPlot:GetResourceType();
						if (eResourceType ~= -1) then
							if (Game.GetResourceUsageType(eResourceType) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY) then
								bAdjacentLuxury = true;
							end
						end
					end
				end
				
				if (not bAdjacentLuxury) then
					-- Add spacing for all entries after the first
					if (bFirstEntry) then
						bFirstEntry = false;
					elseif (not bFirstEntry) then
						strDisabledString = strDisabledString .. "[NEWLINE]";
					end
					
					strDisabledString = strDisabledString .. "[NEWLINE]";
					strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_BUILD_BLOCKED_NO_ADJACENT_LUXURY", strImpRouteKey);
				end
			end
			
			-- Trying to build something where we can't have two adjacent?
			if (pImprovement and pImprovement.NoTwoAdjacent) then
				local bTwoAdjacent = false;

				 for loop, direction in ipairs(direction_types) do
					local adjacentPlot = Map.PlotDirection(pPlot:GetX(), pPlot:GetY(), direction);
					if (adjacentPlot ~= nil) then
						local eImprovementType = adjacentPlot:GetImprovementType();
						if (eImprovementType ~= -1) then
							if (eImprovementType == iImprovement) then
								bTwoAdjacent = true;
							end
						end
						local iBuildProgress = adjacentPlot:GetBuildProgress(iBuildID);
						if (iBuildProgress > 0) then
							bTwoAdjacent = true;
						end
					end
				end
				
				if (bTwoAdjacent) then
					-- Add spacing for all entries after the first
					if (bFirstEntry) then
						bFirstEntry = false;
					elseif (not bFirstEntry) then
						strDisabledString = strDisabledString .. "[NEWLINE]";
					end
					
					strDisabledString = strDisabledString .. "[NEWLINE]";
					strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_BUILD_BLOCKED_CANNOT_BE_ADJACENT", strImpRouteKey);
				end
			end
			
			-- Build blocked by a feature here?
			if (pActivePlayer:IsBuildBlockedByFeature(iBuildID, iFeature)) then
				local iFeatureTech;
				
				local filter = "BuildType = '" .. strBuildType .. "' and FeatureType = '" .. strFeatureType .. "'";
				for row in GameInfo.BuildFeatures(filter) do
					iFeatureTech = GameInfo.Technologies[row.PrereqTech].ID;
				end
				
				local pFeatureTech = GameInfo.Technologies[iFeatureTech];
				
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. "[NEWLINE]";
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_BUILD_BLOCKED_BY_FEATURE", pFeatureTech.Description, pFeature.Description);
			end
			
		-- Not a Worker build, use normal disabled help from XML
		else
			
            if (action.Type == "MISSION_FOUND" and pActivePlayer:IsEmpireVeryUnhappy()) then
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_MISSION_BUILD_CITY_DISABLED_UNHAPPY");
			
            elseif (action.Type == "MISSION_CULTURE_BOMB" and pActivePlayer:GetCultureBombTimer() > 0) then
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey("TXT_KEY_MISSION_CULTURE_BOMB_DISABLED_COOLDOWN", pActivePlayer:GetCultureBombTimer());
				
			elseif (action.DisabledHelp and action.DisabledHelp ~= "") then
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strDisabledString = strDisabledString .. "[NEWLINE][NEWLINE]";
				end
				
				strDisabledString = strDisabledString .. Locale.ConvertTextKey(action.DisabledHelp);
			end
		end
		
        strDisabledString = "[COLOR_WARNING_TEXT]" .. strDisabledString .. "[ENDCOLOR]";
        strToolTip = strToolTip .. strDisabledString;
        
    end
    
	-- Is this a Worker build?
	if (bBuild) then
		
		local iExtraBuildRate = 0;
		
		-- Are we building anything right now?
		local iCurrentBuildID = unit:GetBuildType();
		if (iCurrentBuildID == -1 or iBuildID ~= iCurrentBuildID) then
			iExtraBuildRate = unit:WorkRate(true, iBuildID);
		end
		
		local iBuildTurns = pPlot:GetBuildTurnsLeft(iBuildID, Game.GetActivePlayer(), iExtraBuildRate, iExtraBuildRate);
		--print("iBuildTurns: " .. iBuildTurns);
		if (iBuildTurns > 1) then
			strBuildTurnsString = " ... " .. Locale.ConvertTextKey("TXT_KEY_BUILD_NUM_TURNS", iBuildTurns);
		end
		
		-- Extra Yield from this build
		local iYieldChange;
		
		local bFirstYield = true;
		
		for iYield = 0, YieldTypes.NUM_YIELD_TYPES-1, 1 
		do
			iYieldChange = pPlot:GetYieldWithBuild(iBuildID, iYield, false, iActivePlayer);
			iYieldChange = iYieldChange - pPlot:CalculateYield(iYield);
			
			if (iYieldChange ~= 0) then
				
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					--strBuildYieldString = strBuildYieldString .. "[NEWLINE]";
					bFirstEntry = false;
				elseif (not bFirstEntry and bFirstYield) then
					strBuildYieldString = strBuildYieldString .. "[NEWLINE]";
				end
				
				strBuildYieldString = strBuildYieldString .. "[NEWLINE]";
				
				-- Positive or negative change?
				if (iYieldChange > -1) then
					strBuildYieldString = strBuildYieldString .. "[COLOR_POSITIVE_TEXT]+";
				else
					strBuildYieldString = strBuildYieldString .. "[COLOR_NEGATIVE_TEXT]";
				end
				
				if (iYield == YieldTypes.YIELD_FOOD) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_FOOD_STRING", iYieldChange);
				elseif (iYield == YieldTypes.YIELD_PRODUCTION) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_PRODUCTION_STRING", iYieldChange);
				elseif (iYield == YieldTypes.YIELD_GOLD) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_GOLD_STRING", iYieldChange);
				elseif (iYield == YieldTypes.YIELD_SCIENCE) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_SCIENCE_STRING", iYieldChange);
				elseif (iYield == YieldTypes.YIELD_CULTURE) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_CULTURE_STRING", iYieldChange);
				elseif (iYield == YieldTypes.YIELD_FAITH) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_FAITH_STRING", iYieldChange);
				--CBP
				elseif (iYield == YieldTypes.YIELD_TOURISM) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_TOURISM_STRING", iYieldChange);
				elseif (iYield == YieldTypes.YIELD_GOLDEN_AGE_POINTS) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_GAP_STRING", iYieldChange);
				elseif (iYield == YieldTypes.YIELD_GREAT_GENERAL_POINTS) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_GGP_STRING", iYieldChange);
				elseif (iYield == YieldTypes.YIELD_GREAT_ADMIRAL_POINTS) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_GAP2_STRING", iYieldChange);
				elseif (iYield == YieldTypes.YIELD_CULTURE_LOCAL) then
					strBuildYieldString = strBuildYieldString .. Locale.ConvertTextKey("TXT_KEY_BUILD_CULTURE_LOCAL_STRING", iYieldChange);
				end
				
				bFirstYield = false;
			end
		end
		
        strToolTip = strToolTip .. strBuildYieldString;
		
		-- Resource connection
		if (pImprovement) then 
			local iResourceID = pPlot:GetResourceType(iActiveTeam);
			if (iResourceID ~= -1) then
				if (pPlot:IsResourceConnectedByImprovement(iImprovement)) then
					if (Game.GetResourceUsageType(iResourceID) ~= ResourceUsageTypes.RESOURCEUSAGE_BONUS) then
						local pResource = GameInfo.Resources[pPlot:GetResourceType(iActiveTeam)];
						local strResourceString = pResource.Description;
						
						-- Add spacing for all entries after the first
						if (bFirstEntry) then
							bFirstEntry = false;
						elseif (not bFirstEntry) then
							strBuildResourceConnectionString = strBuildResourceConnectionString .. "[NEWLINE]";
						end
						
						strBuildResourceConnectionString = strBuildResourceConnectionString .. "[NEWLINE]";
						strBuildResourceConnectionString = strBuildResourceConnectionString .. Locale.ConvertTextKey("TXT_KEY_BUILD_CONNECTS_RESOURCE", pResource.IconString, strResourceString);
						
						strToolTip = strToolTip .. strBuildResourceConnectionString;
					end
				end
			end
		end
		
		-- Production for clearing a feature
		if (pFeature) then
			local bFeatureRemoved = pPlot:IsBuildRemovesFeature(iBuildID);
			if (bFeatureRemoved) then
				
				-- Add spacing for all entries after the first
				if (bFirstEntry) then
					bFirstEntry = false;
				elseif (not bFirstEntry) then
					strClearFeatureString = strClearFeatureString .. "[NEWLINE]";
				end
				
				strClearFeatureString = strClearFeatureString .. "[NEWLINE]";
				strClearFeatureString = strClearFeatureString .. Locale.ConvertTextKey("TXT_KEY_BUILD_FEATURE_CLEARED", pFeature.Description);
			end
			
			local iFeatureProduction = pPlot:GetFeatureProduction(iBuildID, iActiveTeam);
			if (iFeatureProduction > 0) then
				strClearFeatureString = strClearFeatureString .. Locale.ConvertTextKey("TXT_KEY_BUILD_FEATURE_PRODUCTION", iFeatureProduction);
				
			-- Add period to end if we're not going to append info about feature production
			elseif (bFeatureRemoved) then
				strClearFeatureString = strClearFeatureString .. ".";
			end
			
			strToolTip = strToolTip .. strClearFeatureString;
		end
	end
    
    -- Tooltip
    if (strToolTip and strToolTip ~= "") then
        tipControlTable.UnitActionHelp:SetText( strToolTip );
    end
	
	-- Title
    local text = action.TextKey or action.Type or "Action"..(buttonIndex - 1);
    local strTitleString = "[COLOR_POSITIVE_TEXT]" .. Locale.ConvertTextKey( text ) .. "[ENDCOLOR]".. strBuildTurnsString;
    tipControlTable.UnitActionText:SetText( strTitleString );
    
    -- HotKey
    if action.SubType == ActionSubTypes.ACTIONSUBTYPE_PROMOTION then
        tipControlTable.UnitActionHotKey:SetText( "" );
    elseif action.HotKey and action.HotKey ~= "" then
        tipControlTable.UnitActionHotKey:SetText( "("..tostring(action.HotKey)..")" );
    else
        tipControlTable.UnitActionHotKey:SetText( "" );
    end
    
    -- Autosize tooltip
    tipControlTable.UnitActionMouseover:DoAutoSize();
    local mouseoverSize = tipControlTable.UnitActionMouseover:GetSize();
    if mouseoverSize.x < 350 then
		tipControlTable.UnitActionMouseover:SetSizeVal( 350, mouseoverSize.y );
    end

end


function ShowHideHandler( bIshide, bIsInit )
    if( bIshide ) then
        local EnemyUnitPanel = ContextPtr:LookUpControl( "/InGame/WorldView/EnemyUnitPanel" );
        if( EnemyUnitPanel ~= nil ) then
            EnemyUnitPanel:SetHide( true );
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnActivePlayerChanged(iActivePlayer, iPrevActivePlayer)
	g_lastUnitID = -1;
	OnInfoPaneDirty();
end
Events.GameplaySetActivePlayer.Add(OnActivePlayerChanged);

function OnEnemyPanelHide( bIsEnemyPanelHide )
    if( g_bWorkerActionPanelOpen ) then
        Controls.WorkerActionPanel:SetHide( not bIsEnemyPanelHide );
    end
end
LuaEvents.EnemyPanelHide.Add( OnEnemyPanelHide );

OnInfoPaneDirty();
