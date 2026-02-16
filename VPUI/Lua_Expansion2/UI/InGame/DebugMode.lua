if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
include( "FLuaVector" );

-- Functions for backwards iteration of Lua Tables
function last(t)
	local lastIndex,lastValue;
	for i,v in pairs(t) do
		lastIndex = i;
		lastValue = v;
	end
	return lastIndex,lastValue;
end

function prev(t, index)
	local nextI,nextV;
	
	-- We are at the beginning of the table
	local firstIndex = next(t);
	if( index == firstIndex ) then
		return last(t);
	end
	
	-- Iterate over the whole table until we find an element whose next element is the one we look for
	for i,v in pairs(t) do
		nextI,nextV = next(t,i);
		if( nextI == index ) then
			return i,v;
		end
	end
	return nil,nil;
end

function HexDeSelected( iHexX, iHexY )
	-- Only deal with highlighting in city edit mode
	print( "HexDeSelected:g_iControl " .. tostring(g_iControl) );
	if( g_iControl == 3 ) then
	
		if( iHexX == g_iHexX and iHexY == g_iHexY ) then
    		Events.SerialEventHexHighlight( Vector2( g_iHexX, g_iHexY ), false, Vector4( 0.5, 0.5, 0.5, 1.0 ) );
		end
	end
end
Events.SerialEventHexDeSelected.Add( HexDeSelected )

function ModeClicked( void1, Control ) 
	
	--if leaving city edit mode, deselect the hex
	HexDeSelected( g_iHexX, g_iHexY );
	
	if Control == 100 then
		ContextPtr:SetHide( true );
		--ContextPtr:LookUpControl( "../DebugMenu"):SetHide(false);
	end
	
	Controls.EditCity_Panel:SetHide(		not ( Control == 3  ) );
    Controls.EditImprovement_Panel:SetHide(	not ( Control == 4 ) );
    Controls.EditResource_Panel:SetHide(	not ( Control == 5 ) );
    Controls.CreateRoad_Panel:SetHide(		not ( Control == 7  ) );
    Controls.CreateUnit_Panel:SetHide(		not ( Control == 10  ) );
    Controls.EditUnit_Panel:SetHide(		not ( Control == 11 ) );
    Controls.ReloadAssets_Panel:SetHide(	not ( Control == 15 ) );
    
    --Store the last state change
    g_iControl = Control;
end

Controls.Exit_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.DefaultMode_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.CreateCity_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.EditCity_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.EditImprovement_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.EditResource_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.CreateRoad_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.CreateFarm_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.ChangeBorder_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.CreateFarm_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.CreateUnit_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.EditUnit_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.MoveUnit_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.RotateUnit_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.AttackUnit_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.ReloadAssets_Button:RegisterCallback( Mouse.eLClick, ModeClicked );
Controls.SpawnArrow_Button:RegisterCallback( Mouse.eLClick, ModeClicked );


-------------------------------------------------
-- City Edit Mode buttons
-------------------------------------------------

--Era select
g_iCityEra = 1;
function ChangeCurrentCityEra( Inc )
	g_iCityEra = g_iCityEra + Inc;
	if( g_iCityEra > 4 ) then
		g_iCityEra = 4;
	end
	if( g_iCityEra < 1 ) then
		g_iCityEra = 1;
	end	
	Controls.CityEra_Label:SetText(" " .. g_iCityEra .. " " );
end
Controls.CityEra_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeCurrentCityEra );
Controls.CityEra_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeCurrentCityEra );


--Culture select
g_iCityCulture = 1;
function ChangeCityCulture( Inc )
	g_iCityCulture = g_iCityCulture + Inc;
	if( g_iCityCulture > 5 ) then
		g_iCityCulture = 5;
	end
	if( g_iCityCulture < 1 ) then
		g_iCityCulture = 1;
	end	
	Controls.CityCulture_Label:SetText(" " .. g_iCityCulture .. " ");
end
Controls.CityCulture_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeCityCulture );
Controls.CityCulture_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeCityCulture );


--Size select
g_iPopulation = 1;
function ChangeCitySize( Inc )
	g_iPopulation = g_iPopulation + Inc;
	if( g_iPopulation < 1 ) then
		g_iPopulation = 1;
	end
	if( g_iPopulation > 20 ) then
		g_iPopulation = 20;
	end
	Controls.CitySize_Label:SetText( " " .. g_iPopulation .. "/ 20 ");
end
Controls.CitySize_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeCitySize );
Controls.CitySize_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeCitySize );

--Continent select
g_iCityContinent = 1;
function ChangeCityContinent( Inc )
	g_iCityContinent = g_iCityContinent + Inc;
	if( g_iCityContinent < 1 ) then
		g_iCityContinent = 1;
	end
	if( g_iCityContinent > 5 ) then
		g_iCityContinent = 5;
	end
	Controls.CityContinent_Label:SetText( " " .. g_iCityContinent .. "/ 5 ");
end
Controls.CityContinent_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeCityContinent );
Controls.CityContinent_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeCityContinent );

--Set the current era for the current player
function SetCityEra()
	Events.SerialEventEraChanged( g_iCityEra-1, g_iCurrPlayer );
end
Controls.CityEra_Button:RegisterCallback( Mouse.eLClick, SetCityEra );


--Set the culture
function SetCityCulture()
	Events.SerialEventCityCultureChanged( g_iHexX, g_iHexY, g_iCityCulture-1 );
end
Controls.CityCulture_ButtonApply:RegisterCallback( Mouse.eLClick, SetCityCulture );


--Set the size
function SetCitySize()
	local iCitySize = Game.GetVariableCitySizeFromPopulation( g_iPopulation );
	Events.SerialEventCityPopulationChanged( g_iHexX, g_iHexY, g_iPopulation, iCitySize );
end
Controls.CitySize_ButtonApply:RegisterCallback( Mouse.eLClick, SetCitySize );


--Set the continent
function SetCityContinent()
	Events.SerialEventCityContinentChanged( g_iHexX, g_iHexY, (g_iCityContinent - 1) );
end
Controls.CityContinent_ButtonApply:RegisterCallback( Mouse.eLClick, SetCityContinent );

--Building size control
g_nBuildingSize = 100;
function ChangeBuildingSize( nInc )
	g_nBuildingSize = g_nBuildingSize + nInc;
	Controls.CityBuildingSize_Label:SetText( string.format( "%1.2f", g_nBuildingSize/100 ) );
	Events.SerialEventBuildingSizeChanged( g_nBuildingSize/100 );
end
Controls.CityBuildingSize_ButtonGrtrDown:RegisterCallback( Mouse.eLClick, ChangeBuildingSize );
Controls.CityBuildingSize_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeBuildingSize );
Controls.CityBuildingSize_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeBuildingSize );
Controls.CityBuildingSize_ButtonGrtrUp:RegisterCallback( Mouse.eLClick, ChangeBuildingSize );

-------------------------------------------------
-- Create Unit Mode Buttons
-------------------------------------------------

--Unit Type select
g_iCreateCommonGreatUnitType = -1;
g_iCreateUniqueUnitType = table.maxn(ArtInfo.Units);
function ChangeUnitType( Inc )
    if g_iCreateCommonGreatUnitType + Inc > table.maxn(ArtInfo.Units) then
        g_iCreateCommonGreatUnitType = 0;
    elseif g_iCreateCommonGreatUnitType + Inc < 0  then
        g_iCreateCommonGreatUnitType = table.maxn(ArtInfo.Units);
    else
        g_iCreateCommonGreatUnitType = g_iCreateCommonGreatUnitType + Inc;
    end
    
    Controls.CreateUnitType_Label:SetText(ArtInfo.Units[g_iCreateCommonGreatUnitType].Type);
    Events.UnitTypeChanged("ART_DEF_UNIT_" .. ArtInfo.Units[g_iCreateCommonGreatUnitType].Type);
end
Controls.CreateUnitType_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeUnitType );
Controls.CreateUnitType_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeUnitType );

-------------------------------------------------
function ChangeUniqueUnitType( Inc )
    if g_iCreateUniqueUnitType + Inc > table.maxn(ArtInfo.UniqueUnits) then
        g_iCreateUniqueUnitType = 0; --table.count(ArtInfo.Units);
    elseif g_iCreateUniqueUnitType + Inc < 0 then --table.count(ArtInfo.Units) then
        g_iCreateUniqueUnitType = table.maxn(ArtInfo.UniqueUnits);
    else
        g_iCreateUniqueUnitType = g_iCreateUniqueUnitType + Inc;
    end
    
    Controls.CreateUniqueUnitType_Label:SetText(ArtInfo.UniqueUnits[g_iCreateUniqueUnitType].Type);
    Events.UnitTypeChanged("ART_DEF_UNIT_U_" .. ArtInfo.UniqueUnits[g_iCreateUniqueUnitType].Type);
end
Controls.CreateUniqueUnitType_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeUniqueUnitType );
Controls.CreateUniqueUnitType_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeUniqueUnitType );

-------------------------------------------------
-- Universal Asset Iteration Function
-------------------------------------------------
function ChangeAssetType( iIncrement, iIndex, sTable )
	local i,v;
    
    -- Handle the previous button, since Lua has just a next() function
    if( iIncrement < 0 ) then
		-- We are at the starting "nothing selected" state
		if( iIndex == 10000 ) then
			i,v = last(sTable);
		else
			i,v = prev(sTable, iIndex);
		end
    else    
		-- We are at the starting "nothing selected" state
		if( iIndex == 10000 ) then
			i,v = next(sTable);
		else
			i,v = next(sTable, iIndex);
		end
	end
	
	-- We are at the end of the table or prev() didn't find a match
	if( i == nil and v == nil ) then
		i,v = next(sTable);
	end
	
    return i,v;
end

-------------------------------------------------
-- Improvement Edit Mode Buttons
-------------------------------------------------
g_iCreateImprovementType = 10000;
g_iCreateImprovementRRType = 10000;
g_iCreateImprovementEra = 10000;
g_iCreateImprovementState = 10000;
-------------------------------------------------
function ChangeImprovementType( Inc )
	local i,v = ChangeAssetType( Inc, g_iCreateImprovementType, ArtInfo.Improvements);
	
	-- Reset globals
	g_iCreateImprovementRRType = 10000;
	Controls.ImprovementRRType_Label:SetText("Choose Resource");
	
    g_iCreateImprovementEra = 10000;
    Controls.ImprovementEra_Label:SetText("Choose Era");
    
    g_iCreateImprovementState = 10000;
    Controls.ImprovementState_Label:SetText("Choose State");
    
    g_iCreateImprovementType = i;
    Controls.ImprovementType_Label:SetText(v.ImprovementType);
end
Controls.ImprovementType_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeImprovementType );
Controls.ImprovementType_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeImprovementType );

-------------------------------------------------
function ChangeImprovementRRType( Inc )
    if(g_iCreateImprovementType ~= 10000) then
		local i,v = ChangeAssetType( Inc, g_iCreateImprovementRRType, ArtInfo.Improvements[g_iCreateImprovementType].ResourceTypes);
		
		-- Reset globals
		g_iCreateImprovementEra = 10000;
		Controls.ImprovementEra_Label:SetText("Choose Era");
    
		g_iCreateImprovementState = 10000;
		Controls.ImprovementState_Label:SetText("Choose State");
	
		g_iCreateImprovementRRType = i;
		Controls.ImprovementRRType_Label:SetText(v.ResourceType);
	end
end
Controls.ImprovementRRType_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeImprovementRRType );
Controls.ImprovementRRType_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeImprovementRRType );

-------------------------------------------------
function ChangeImprovementEra( Inc )
    if(g_iCreateImprovementType ~= 10000 and g_iCreateImprovementRRType ~= 10000) then
		local i,v = ChangeAssetType( Inc, g_iCreateImprovementEra, ArtInfo.Improvements[g_iCreateImprovementType].ResourceTypes[g_iCreateImprovementRRType].Eras);
		
		-- Reset globals
		g_iCreateImprovementState = 10000;
		Controls.ImprovementState_Label:SetText("Choose State");
		
		g_iCreateImprovementEra = i;
		Controls.ImprovementEra_Label:SetText(v.EraName);
	end
end
Controls.ImprovementEra_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeImprovementEra );
Controls.ImprovementEra_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeImprovementEra );

-------------------------------------------------
function ChangeImprovementState( Inc )
    if(g_iCreateImprovementType ~= 10000 and g_iCreateImprovementRRType ~= 10000 and g_iCreateImprovementEra ~= 10000) then
		local i,v = ChangeAssetType( Inc, g_iCreateImprovementState, ArtInfo.Improvements[g_iCreateImprovementType].ResourceTypes[g_iCreateImprovementRRType].Eras[g_iCreateImprovementEra].States);
		
		g_iCreateImprovementState = i;
		Controls.ImprovementState_Label:SetText(v.StateName);
	end
end
Controls.ImprovementState_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeImprovementState );
Controls.ImprovementState_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeImprovementState );

-------------------------------------------------
-- Resource Edit Mode Buttons
-------------------------------------------------
g_iCreateResourceType = 10000;
function ChangeResourceType( Inc )
	local i,v = ChangeAssetType( Inc, g_iCreateResourceType, ArtInfo.Resources);
	
    g_iCreateResourceType = i;
    Controls.ResourceType_Label:SetText(v.Type);
end
Controls.ResourceType_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeResourceType );
Controls.ResourceType_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeResourceType );


-------------------------------------------------
-- Asset Reload Mode Buttons
-------------------------------------------------

-- The culture of the city's buildings
g_iAssetCulture = -1;
function ChangeCityAssetCulture( Inc )
	g_iAssetCulture = g_iAssetCulture + Inc;
	if( g_iAssetCulture < -1 ) then
		g_iAssetCulture = -1;
	end
	if( g_iAssetCulture > 3 ) then
		g_iAssetCulture = 3;
	end
	Controls.BuildingCulture_Label:SetText(g_iAssetCulture);
end
Controls.BuildingCulture_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeCityAssetCulture );
Controls.BuildingCulture_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeCityAssetCulture );

-- The era of the city's buildings
g_iAssetEra = -1;
function ChangeCityAssetEra( Inc )
	g_iAssetEra = g_iAssetEra + Inc;
	if( g_iAssetEra < -1 ) then
		g_iAssetEra = -1;
	end
	if( g_iAssetEra > 3 ) then
		g_iAssetEra = 3;
	end
	Controls.BuildingEra_Label:SetText(g_iAssetEra );
end
Controls.BuildingEra_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeCityAssetEra );
Controls.BuildingEra_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeCityAssetEra );

--Reload assets based on type
function ReloadAssets( Type )
    --Features
    if( Type == 0 ) then
	    Events.LandmarkLibrarySwap( );
    end
    --Units
    if( Type == 1 ) then
	    Events.UnitLibrarySwap( );
    end
    --Buildings
    if( Type == 2 ) then
	    Events.BuildingLibrarySwap( g_iAssetCulture, g_iAssetEra );
    end
end
Controls.ReloadLandmarks_Button:RegisterCallback( Mouse.eLClick, ReloadAssets );
Controls.ReloadUnits_Button:RegisterCallback( Mouse.eLClick, ReloadAssets );
Controls.ReloadBuildings_Button:RegisterCallback( Mouse.eLClick, ReloadAssets );

-------------------------------------------------
-- Keep track of the selected hex
-------------------------------------------------

-- The currently selected hex
g_iHexX = -100;
g_iHexY = -100;

function HexSelected( iHexX, iHexY )

	-- Only deal with highlighting in city edit mode or road creation mode
	print( "HexSelected:g_iControl " .. tostring(g_iControl) );
	if( ( g_iControl == 3 ) or ( g_iControl == 7 ) ) then

		-- Remove any old highlighting and store the currently selected hex
		Events.SerialEventHexHighlight( Vector2( g_iHexX, g_iHexY ), false, Vector4( 0.5, 0.5, 0.5, 1.0 ) );
		g_iHexX = iHexX;
		g_iHexY = iHexY;
	
		-- Highlight the hex
		Events.SerialEventHexHighlight( Vector2( g_iHexX, g_iHexY ), true, Vector4( 0.5, 0.5, 0.5, 1.0 ) );
	end
	
	-- If in road create mode, generate a road
	if( g_iControl == 7 ) then
		if( Controls.CreateRoad_Checkbox:IsChecked() ) then
			Events.SerialEventRoadDestroyed( g_iHexX, g_iHexY );
		else
			Events.SerialEventRoadCreated( g_iHexX, g_iHexY, g_iCurrPlayer, 0 );
		end
	end

	-- If in unit edit mode, select the unit.
	if( g_iControl == 11 )then	
		local playerID = Game.GetActivePlayer();

		Events.UnitSelectionChanged(playerID, -1, 0, 0, 0, false, false);			-- Does this even work?  We want to essentially deselect the currently selected unit.  cvUnitSimSystem::OnUnitSelected does not seem to handle unitID == -1
		Events.UnitSelectionChanged(-1, -1, iHexX, iHexY, 0, true, true);			-- cvUnitSimSystem::OnUnitSelected checks for a -1 in the PlayerID field to determine if it should use the plot location.
		
		-- Reset the member index.
		g_iUnitMemberIndex = -1;
		ModUnitMemberIndex(0);
	end

	-- If in improvement mode, build an improvement
	if( g_iControl == 4 )then
		local playerID = Game.GetActivePlayer();
		
		local plot = Map.GetPlot( iHexX, iHexY );
		local continent = plot:GetContinentArtType();
		
		--              LandmarkCreatedData ( HPosX, HPosY, CultureType, ContinentType, PlayerType, ImprovementType,          RawResourceType,            ImprovementEra,          ImprovementState          )
		Events.SerialEventImprovementCreated( iHexX, iHexY, continent,   continent,     playerID,   g_iCreateImprovementType, g_iCreateImprovementRRType, g_iCreateImprovementEra, g_iCreateImprovementState );
	end

	-- If in Resource mode, plop a resource
	if( g_iControl == 5 )then
		local playerID = Game.GetActivePlayer();
		
		local plot = Map.GetPlot( iHexX, iHexY );
		local continent = plot:GetContinentArtType();
		
		--              LandmarkCreatedData ( HPosX, HPosY, CultureType, ContinentType, PlayerType, ImprovementType, RawResourceType,       ImprovementEra, ImprovementState )
		Events.SerialEventRawResourceCreated( iHexX, iHexY, continent,   continent,     playerID,   -1,              g_iCreateResourceType, 0,              0                );
	end

	-- Clear out any temp data
	g_iCityEra = 1;
end
Events.SerialEventHexSelected.Add( HexSelected );


-------------------------------------------------
--Generic Player select button handling
g_iCurrPlayer = 0;
function ChangeCurrentCityPlayer( Inc )
	g_iCurrPlayer = g_iCurrPlayer + Inc;
	Controls.CityPlayer_Label:SetText(" " .. g_iCurrPlayer .. " " );
	Controls.RoadPlayer_Label:SetText(" " .. g_iCurrPlayer .. " " );
end
Controls.CityPlayer_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeCurrentCityPlayer );
Controls.RoadPlayer_ButtonDown:RegisterCallback( Mouse.eLClick, ChangeCurrentCityPlayer );
Controls.CityPlayer_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeCurrentCityPlayer );
Controls.RoadPlayer_ButtonUp:RegisterCallback( Mouse.eLClick, ChangeCurrentCityPlayer );

-------------------------------------------------
--  Unit Mode Buttons
-------------------------------------------------

g_iUnitCulture        = 0;
g_iUnitType           = 0;
g_fUnitHealth         = 1;
g_bUnitFortified      = false;
g_tFloatVarList       = 0;
g_sVariableName       = "UNKNOWN_VAR";
g_fVariableValue      = 0.0;
g_iVariableIndex      = 1;
g_iUnitMemberIndex    = 0;
g_iUnitMemberCount	  = 1;
g_iUnitSpecularRender = 0;
-------------------------------------------------
-------------------------------------------------
function ModUnitHealth( isInc )
    if isInc == 1
    then
        g_fUnitHealth = g_fUnitHealth + 0.1;
    else
        g_fUnitHealth = g_fUnitHealth - 0.1;
    end
    
    g_fUnitHealth = math.min( g_fUnitHealth, 1.0 );
    g_fUnitHealth = math.max( g_fUnitHealth, 0 );
    
	Events.UnitFlagUpdated( g_iUnitCulture, g_iUnitType, g_fUnitHealth, g_bUnitFortified );
end
Controls.UnitFlagHealth_ButtonUp:RegisterCallback( Mouse.eLClick, ModUnitHealth );
Controls.UnitFlagHealth_ButtonDown:RegisterCallback( Mouse.eLClick, ModUnitHealth );

-------------------------------------------------
-------------------------------------------------
function ModUnitCulture( isInc )
    if isInc == 1
    then
        g_iUnitCulture = g_iUnitCulture + 1;
    else
        g_iUnitCulture = g_iUnitCulture - 1;
    end
    
    g_iUnitCulture = math.min( g_iUnitCulture, 24 );
    g_iUnitCulture = math.max( g_iUnitCulture, 0 );
    
	Events.UnitFlagUpdated( g_iUnitCulture, g_iUnitType, g_fUnitHealth, g_bUnitFortified );
end
Controls.UnitFlagCulture_ButtonUp:RegisterCallback( Mouse.eLClick, ModUnitCulture );
Controls.UnitFlagCulture_ButtonDown:RegisterCallback( Mouse.eLClick, ModUnitCulture );

-------------------------------------------------
-------------------------------------------------
function ModUnitType( isInc )
    if isInc == 1
    then
        g_iUnitType = g_iUnitType + 1;
    else
        g_iUnitType = g_iUnitType - 1;
    end
    
    g_iUnitType = math.min( g_iUnitType, 83 );
    g_iUnitType = math.max( g_iUnitType, 0 );
    
	Events.UnitFlagUpdated( g_iUnitCulture, g_iUnitType, g_fUnitHealth, g_bUnitFortified );
end
Controls.UnitFlagType_ButtonUp:RegisterCallback( Mouse.eLClick, ModUnitType );
Controls.UnitFlagType_ButtonDown:RegisterCallback( Mouse.eLClick, ModUnitType );

-------------------------------------------------
-------------------------------------------------
function ModUnitFort()
    g_bUnitFortified = not g_bUnitFortified;
    
    Controls.UnitFlagFortify_Checkbox:SetCheck( g_bUnitFortified );
    
	Events.UnitFlagUpdated( g_iUnitCulture, g_iUnitType, g_fUnitHealth, g_bUnitFortified );
end
Controls.UnitFlagFortify_Checkbox:RegisterCallback( Mouse.eLClick, ModUnitFort );

-------------------------------------------------
-------------------------------------------------
function SpecularRenderUnit( Inc )
    g_iUnitSpecularRender = Inc;

	g_iUnitSpecularRender = math.min( g_iUnitSpecularRender, 2 );
    g_iUnitSpecularRender = math.max( g_iUnitSpecularRender, 0 );
    
	Controls.UnitSpecRenderNormal_Checkbox:SetCheck( g_iUnitSpecularRender == 0 );
	Controls.UnitSpecRenderDiffuse_Checkbox:SetCheck( g_iUnitSpecularRender == 1 );
	Controls.UnitSpecRenderSpecular_Checkbox:SetCheck( g_iUnitSpecularRender == 2 );
end

Controls.UnitSpecRenderNormal_Checkbox:RegisterCallback( Mouse.eLClick, SpecularRenderUnit );
Controls.UnitSpecRenderDiffuse_Checkbox:RegisterCallback( Mouse.eLClick, SpecularRenderUnit );
Controls.UnitSpecRenderSpecular_Checkbox:RegisterCallback( Mouse.eLClick, SpecularRenderUnit );

-------------------------------------------------
-------------------------------------------------
g_bUnitMemberFSMDebugCheck = false;
function UpdateUnitAnimationLabels( )
    local iCount = 1;
    for index, value in pairs( g_tFloatVarList ) do 
        if iCount == g_iVariableIndex then
            Controls.UnitVariableName_Label:SetText( index );
            Controls.UnitVariableValue_Label:SetText( string.format("%.1f", value) );
            
            g_sVariableName = index;
            g_fVariableValue = value;
            
            -- if value >= 0.0 and value <= 1.0 then
            --     Controls.UnitVariableValue_Slider:SetValue( value );
            -- end
        end
        iCount = iCount + 1;
    end
    
    Controls.UnitMemberFSMDebug_Checkbox:SetCheck( g_bUnitMemberFSMDebugCheck );
    
    SpecularRenderUnit( g_iUnitSpecularRender );
end

-------------------------------------------------
-------------------------------------------------
function ModUnitAnimationVariable( isInc )
    g_iVariableIndex = g_iVariableIndex + isInc;

    g_iVariableIndex = math.min( g_iVariableIndex, table.count(g_tFloatVarList) );
    g_iVariableIndex = math.max( g_iVariableIndex, 1 );
    
    UpdateUnitAnimationLabels();
end
Controls.UnitVariableName_ButtonUp:RegisterCallback( Mouse.eLClick, ModUnitAnimationVariable );
Controls.UnitVariableName_ButtonDown:RegisterCallback( Mouse.eLClick, ModUnitAnimationVariable );

-------------------------------------------------
-------------------------------------------------
function ModUnitVariableValue( isInc )
    fChange = 10;
    if isInc > 2 or isInc < -2 then
        fChange = 1000;
    elseif isInc > 1 or isInc < -1 then
        fChange = 100;
    end
    
    g_fVariableValue = g_fVariableValue + (fChange * (isInc/math.abs(isInc)));
    
    g_tFloatVarList[g_sVariableName] = g_fVariableValue;
    
    Controls.UnitVariableValue_Label:SetText( string.format("%.1f", g_fVariableValue) );
    -- Controls.UnitVariableValue_Slider:SetValue( g_fVariableValue );
end
Controls.UnitVariableValue_ButtonUp:RegisterCallback( Mouse.eLClick, ModUnitVariableValue );
Controls.UnitVariableValue_ButtonDown:RegisterCallback( Mouse.eLClick, ModUnitVariableValue );
Controls.UnitVariableValue_ButtonGrtUp:RegisterCallback( Mouse.eLClick, ModUnitVariableValue );
Controls.UnitVariableValue_ButtonGrtDown:RegisterCallback( Mouse.eLClick, ModUnitVariableValue );
Controls.UnitVariableValue_ButtonMaxUp:RegisterCallback( Mouse.eLClick, ModUnitVariableValue );
Controls.UnitVariableValue_ButtonMaxDown:RegisterCallback( Mouse.eLClick, ModUnitVariableValue );

-- function SliderUnitVariableValue( fValue )
--     g_tFloatVarList[g_sVariableName] = fValue;
--     Controls.UnitVariableValue_Label:SetText( string.format("%.1f", fValue) );
-- end
-- Controls.UnitVariableValue_Slider:RegisterSliderCallback( SliderUnitVariableValue );

-------------------------------------------------
-------------------------------------------------
function ModUnitMemberIndex( isInc )
    if isInc == 0 and g_iUnitMemberIndex ~= -1 then
        g_iUnitMemberIndex = -1;
        g_bUnitMemberFSMDebugCheck = false;
        
        Controls.UnitMemberSelectAll_Checkbox:SetCheck( true );
        Controls.UnitMemberFSMDebug_Checkbox:SetCheck( g_bUnitMemberFSMDebugCheck );
    else
        g_iUnitMemberIndex = g_iUnitMemberIndex + isInc;
        
        g_iUnitMemberIndex = math.min( g_iUnitMemberIndex, g_iUnitMemberCount );
        g_iUnitMemberIndex = math.max( g_iUnitMemberIndex, 1 );
        
        Controls.UnitMemberSelectAll_Checkbox:SetCheck( false );
        
        Events.UnitDataRequested( g_iUnitMemberIndex-1 );
    end
    
    Controls.UnitMember_Label:SetText( string.format("%d", g_iUnitMemberIndex) );
end
Controls.UnitMember_ButtonUp:RegisterCallback( Mouse.eLClick, ModUnitMemberIndex );
Controls.UnitMember_ButtonDown:RegisterCallback( Mouse.eLClick, ModUnitMemberIndex );
Controls.UnitMemberSelectAll_Checkbox:RegisterCallback( Mouse.eLClick, ModUnitMemberIndex );

-------------------------------------------------
-------------------------------------------------
function DebugUnitFSM()
    g_bUnitMemberFSMDebugCheck = true;
    Controls.UnitMemberFSMDebug_Checkbox:SetCheck( g_bUnitMemberFSMDebugCheck );
    
    Events.UnitDebugFSM( g_iUnitMemberIndex-1 );
end

Controls.UnitMemberFSMDebug_Checkbox:RegisterCallback( Mouse.eLClick, DebugUnitFSM );

-------------------------------------------------
-------------------------------------------------
function ApplyUnitAnimationChanges()
    Events.UnitDataEdited( g_tFloatVarList, g_iUnitMemberCount, g_iUnitMemberIndex-1, false, g_iUnitSpecularRender );
end
Controls.EditUnit_ButtonApply:RegisterCallback( Mouse.eLClick, ApplyUnitAnimationChanges );

-------------------------------------------------
-------------------------------------------------
function OnFloatVarListTransfer( floatVarList, memberCount, memberIndex, isDebugFSM, specRender, isFromLua )
    if isFromLua ~= true then
        g_tFloatVarList = floatVarList;
        
        g_iVariableIndex = math.min( g_iVariableIndex, table.count(g_tFloatVarList) );
        g_iVariableIndex = math.max( g_iVariableIndex, 1 );
        
        g_iUnitMemberCount = memberCount;
        g_iUnitMemberIndex = memberIndex + 1;
        
        g_bUnitMemberFSMDebugCheck = isDebugFSM;
        
        g_iUnitSpecularRender = specRender;
        
        UpdateUnitAnimationLabels();
    end
end
Events.UnitDataEdited.Add( OnFloatVarListTransfer );
