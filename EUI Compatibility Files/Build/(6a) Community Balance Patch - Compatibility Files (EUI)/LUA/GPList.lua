--------------------------------------------------------------
--------------------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "SupportFunctions" );

local m_sortTable;

local eCity  = 0;
local eTurns = 1;

local m_SortMode = eTurns;
local m_bSortReverse = false;

local m_ArtistIM = InstanceManager:new( "GPInstance", "Root", Controls.ArtistStack );
local m_DignitaryIM = InstanceManager:new( "GPInstance", "Root", Controls.DignitaryStack ); -- JFD CID
local m_DiplomatIM = InstanceManager:new( "GPInstance", "Root", Controls.DiplomatStack ); -- Gazebo CSD
local m_DoctorIM = InstanceManager:new( "GPInstance", "Root", Controls.DoctorStack ); -- JFD CID
local m_EngineerIM = InstanceManager:new( "GPInstance", "Root", Controls.EngineerStack );
local m_MagistrateIM = InstanceManager:new( "GPInstance", "Root", Controls.MagistrateStack ); -- JFD CID
local m_MerchantIM = InstanceManager:new( "GPInstance", "Root", Controls.MerchantStack );
local m_MusicianIM = InstanceManager:new( "GPInstance", "Root", Controls.MusicianStack );
local m_ScientistIM = InstanceManager:new( "GPInstance", "Root", Controls.ScientistStack );
local m_ProphetIM = InstanceManager:new( "GPInstance", "Root", Controls.ProphetStack ); -- JFD Piety
local m_WriterIM = InstanceManager:new( "GPInstance", "Root", Controls.WriterStack );

-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide )
	if( not bIsHide ) then
		UpdateDisplay();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );


-------------------------------------------------
-------------------------------------------------
function OnClose( )
	ContextPtr:SetHide( true );
	Events.OpenInfoCorner( InfoCornerID.None );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );


----------------------------------------------------------------
-- Key Down Processing
----------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE then
			OnClose();
			return true;
		end
	end
end
ContextPtr:SetInputHandler( InputHandler );

-------------------------------------------------
-------------------------------------------------
function OnChangeEvent()
	if( ContextPtr:IsHidden() == false ) then
		UpdateDisplay();
	end
end
Events.SerialEventGameDataDirty.Add( OnChangeEvent );
Events.SerialEventCityInfoDirty.Add( OnChangeEvent );
Events.GameplaySetActivePlayer.Add( OnChangeEvent );

-------------------------------------------------
-------------------------------------------------
function UpdateDisplay()
	m_SortTable = {};

	local pPlayer = Players[Game:GetActivePlayer()];
	local pCity;
	local cityName;
	local instance;

	local bArtistFound = false;
	local bDignitaryFound = false; -- JFD CID
	local bDiplomatFound = false; -- Gazebo CSD
	local bDoctorFound = false; -- JFD CID
	local bEngineerFound = false;
	local bMagistrateFound = false; -- JFD CID
	local bMerchantFound = false;
	local bMusicianFound = false; 
	local bProphetFound = false; -- JFD Piety
	local bScientistFound = false;
	local bWriterFound = false;

	local iRate;
	local iThreshold;
	local iNumTurns;
	local iProgress;

	m_ArtistIM:ResetInstances();
	m_DignitaryIM:ResetInstances(); -- JFD CID
	m_DiplomatIM:ResetInstances(); -- Gazebo CSD
	m_DoctorIM:ResetInstances(); -- JFD CID
	m_EngineerIM:ResetInstances();
	m_MagistrateIM:ResetInstances(); -- JFD CID
	m_MerchantIM:ResetInstances();
	m_MusicianIM:ResetInstances();
	m_ScientistIM:ResetInstances();
	m_ProphetIM:ResetInstances(); -- JFD Piety
	m_WriterIM:ResetInstances();

	if (checkIfGP(pPlayer)) then
		for pSpecialistInfo in GameInfo.Specialists() do	
			local iSpecialistIndex =  pSpecialistInfo.ID;
			for pCity in pPlayer:Cities() do
				if (checkIfSpecialist(pPlayer, pSpecialistInfo, pCity)) then
					if (pSpecialistInfo.Type == "SPECIALIST_ARTIST") then
						instance = m_ArtistIM:GetInstance();
						bArtistFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_CIVIL_SERVANT") then -- Gazebo CSD
						instance = m_DiplomatIM:GetInstance();
						bDiplomatFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_JFD_DIGNITARY") then -- JFD CID
						instance = m_DignitaryIM:GetInstance();
						bDignitaryFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_JFD_DOCTOR") then -- JFD CID
						instance = m_DoctorIM:GetInstance();
						bDoctorFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_ENGINEER") then
						instance = m_EngineerIM:GetInstance();
						bEngineerFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_JFD_MAGISTRATE") then -- JFD CID
						instance = m_MagistrateIM:GetInstance();
						bMagistrateFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_MERCHANT") then
						instance = m_MerchantIM:GetInstance();
						bMerchantFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_JFD_MONK") then -- JFD Piety
						instance = m_ProphetIM:GetInstance();
						bProphetFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_MUSICIAN") then
						instance = m_MusicianIM:GetInstance();
						bMusicianFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_SCIENTIST") then
						instance = m_ScientistIM:GetInstance();
						bScientistFound = true;
					elseif (pSpecialistInfo.Type == "SPECIALIST_WRITER") then
						instance = m_WriterIM:GetInstance();
						bWriterFound = true;
					end		
							
					iProgress = pCity:GetSpecialistGreatPersonProgress(iSpecialistIndex);	
					if(iProgress>0) then 
						instance.Button:RegisterCallback( Mouse.eLClick, OnCityClick );
						instance.Button:RegisterCallback( Mouse.eRClick, OnCityRClick );
						instance.Button:SetVoids( pCity:GetX(), pCity:GetY() );
						
						local sortEntry = {};
						m_SortTable[ tostring( instance.Root ) ] = sortEntry;

						cityName = Locale.ConvertTextKey( pCity:GetNameKey() );
						sortEntry.name = cityName;
						TruncateString( instance.CityName, 74, sortEntry.name );

						local iUnitClass = GameInfo.UnitClasses[pSpecialistInfo.GreatPeopleUnitClass].ID
						iThreshold = pCity:GetSpecialistUpgradeThreshold(iUnitClass);
						iRate = getRateOfChange(pCity, pSpecialistInfo, pPlayer)

						local iNumTurns;
						local strNumTurns;
						
						if(iRate == 0) then
							iNumTurns = -1;
							strNumTurns = "--";
						elseif(iRate > 0) then
							if ((iThreshold - iProgress) == iRate) then
								iNumTurns = 9999;
								strNumTurns = "--";
							else
								iNumTurns = math.floor((((iThreshold - iProgress)/iRate)+1));
								strNumTurns = iNumTurns;
							end
						end

						--strTurns = Locale.ConvertTextKey( "TXT_KEY_GPLIST_TURNS", iRate, iNumTurns );
						strTurns = Locale.ConvertTextKey( "TXT_KEY_GPLIST_TURNS", iRate );
						strProgress = Locale.ConvertTextKey( "TXT_KEY_GPLIST_PROGRESS", iProgress, iThreshold, strNumTurns );

						sortEntry.turns = iNumTurns;

						instance.GPTurns:SetText( strTurns );
						instance.GPTurns:SetToolTipString( strProgress );
						instance.GPMeter:SetPercent( iProgress / iThreshold );
						instance.GPMeter:SetToolTipString( strProgress );

						instance.GPStack:CalculateSize();
						instance.GPStack:ReprocessAnchoring();
		
						sortEntry.cityID = pCity:GetID();
					end
				end
			end
		end
	end	

	--add great general at the end
	local fProgress  = pPlayer:GetCombatExperience();
	local fThreshold = pPlayer:GreatGeneralThreshold();
	local GGString = Locale.ConvertTextKey("TXT_KEY_GREAT_GENERAL_PROGRESS", fProgress, fThreshold);
	Controls.GGMeter:SetToolTipString( GGString );
	Controls.GGLabel:SetToolTipString( GGString );
	Controls.GGMeter:SetPercent( fProgress / fThreshold );
	
	
	local fProgress  = pPlayer:GetNavalCombatExperience();
	local fThreshold = pPlayer:GreatAdmiralThreshold();
	local GGString = Locale.ConvertTextKey("TXT_KEY_GREAT_GENERAL_PROGRESS", fProgress, fThreshold);
	Controls.GAMeter:SetToolTipString( GGString );
	Controls.GALabel:SetToolTipString( GGString );
	Controls.GAMeter:SetPercent( fProgress / fThreshold );

	Controls.ArtistStack:SortChildren( SortFunction );
	Controls.DignitaryStack:SortChildren( SortFunction ); -- JFD CID
	Controls.DiplomatStack:SortChildren( SortFunction ); -- Gazebo CSD
	Controls.DoctorStack:SortChildren( SortFunction ); -- JFD CID
	Controls.EngineerStack:SortChildren( SortFunction );
	Controls.MagistrateStack:SortChildren( SortFunction ); -- JFD CID
	Controls.MerchantStack:SortChildren( SortFunction );
	Controls.MusicianStack:SortChildren( SortFunction );
	Controls.ScientistStack:SortChildren( SortFunction );
	Controls.ProphetStack:SortChildren( SortFunction ); -- JFD Piety
	Controls.WriterStack:SortChildren( SortFunction );

	Controls.ArtistHeader:SetHide( not bArtistFound );
	Controls.DignitaryHeader:SetHide( not bDignitaryFound ); -- JFD CID
	Controls.DiplomatHeader:SetHide( not bDiplomatFound ); -- Gazebo CSD
	Controls.DoctorHeader:SetHide( not bDoctorFound ); -- JFD CID
	Controls.EngineerHeader:SetHide( not bEngineerFound );
	Controls.MagistrateHeader:SetHide( not bMagistrateFound ); -- JFD CID
	Controls.MerchantHeader:SetHide( not bMerchantFound );
	Controls.MusicianHeader:SetHide( not bMusicianFound );
	Controls.ScientistHeader:SetHide( not bScientistFound );
	Controls.ProphetHeader:SetHide( not bProphetFound ); -- JFD Piety
	Controls.WriterHeader:SetHide( not bWriterFound );

	Controls.ArtistStack:CalculateSize();
	Controls.ArtistStack:ReprocessAnchoring();
	Controls.DignitaryStack:CalculateSize();  -- JFD CID
	Controls.DignitaryStack:CalculateSize();  -- JFD CID
	Controls.DiplomatStack:CalculateSize();  -- Gazebo CSD
	Controls.DiplomatStack:CalculateSize();  -- Gazebo CSD
	Controls.DoctorStack:CalculateSize();  -- JFD CID
	Controls.DoctorStack:CalculateSize();  -- JFD CID
	Controls.EngineerStack:CalculateSize();
	Controls.EngineerStack:ReprocessAnchoring();
	Controls.MagistrateStack:CalculateSize(); -- JFD CID
	Controls.MagistrateStack:ReprocessAnchoring(); -- JFD CID
	Controls.MerchantStack:CalculateSize();
	Controls.MerchantStack:ReprocessAnchoring();
	Controls.MusicianStack:CalculateSize();
	Controls.MusicianStack:ReprocessAnchoring();
	Controls.ScientistStack:CalculateSize();
	Controls.ScientistStack:ReprocessAnchoring();
	Controls.ProphetStack:CalculateSize(); -- JFD Piety
	Controls.ProphetStack:ReprocessAnchoring(); -- JFD Piety
	Controls.WriterStack:CalculateSize();
	Controls.WriterStack:ReprocessAnchoring();

	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();

	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end

-------------------------------------------------
-------------------------------------------------
--check if player has any GPs being built
function checkIfGP(player) 
	for pCity in player:Cities() do
		for pSpecialistInfo in GameInfo.Specialists() do	
			local iSpecialistIndex =  pSpecialistInfo.ID;			
			local iProgress = pCity:GetSpecialistGreatPersonProgress(iSpecialistIndex);	
			if(iProgress>0) then
				return true;
			end
		end
	end
	return false;
end

-------------------------------------------------
-------------------------------------------------
--check if player has any of a specific Specialist being built
function checkIfSpecialist(player, specialist, city) 
	local iSpecialistIndex =  specialist.ID;			
	local iProgress = city:GetSpecialistGreatPersonProgress(iSpecialistIndex);	
	if(iProgress>0) then
		return true;
	end
	return false;
end

-------------------------------------------------
-------------------------------------------------
--given a city and GP, returns the progress per turn
function getRateOfChange(city, specialistInfo, player)
	local iCount = city:GetSpecialistCount( specialistInfo.ID );
	local iGPPChange = specialistInfo.GreatPeopleRateChange * iCount * 100;
	for building in GameInfo.Buildings() do
		local buildingID = building.ID;
		if building.SpecialistType == specialistInfo.Type then
			if (city:IsHasBuilding(buildingID)) then
				iGPPChange = iGPPChange + building.GreatPeopleRateChange * 100;
			end
		end
	end
	if iGPPChange > 0 then
		local iMod = 0;
		
		-- Generic GP mods
		local iPlayerMod = player:GetGreatPeopleRateModifier();
		local iCityMod = city:GetGreatPeopleRateModifier();
		local iGoldenAgeMod = 0;
		local bGoldenAge = (player:GetGoldenAgeTurns() > 0);
		
		-- GP mods by type		
		if (specialistInfo.GreatPeopleUnitClass == "UNITCLASS_WRITER") then
			iPlayerMod = iPlayerMod + player:GetGreatWriterRateModifier();
			if (bGoldenAge and player:GetGoldenAgeGreatWriterRateModifier() > 0) then
				iGoldenAgeMod = iGoldenAgeMod + player:GetGoldenAgeGreatWriterRateModifier();
			end
		elseif (specialistInfo.GreatPeopleUnitClass == "UNITCLASS_ARTIST") then
			iPlayerMod = iPlayerMod + player:GetGreatArtistRateModifier();
			if (bGoldenAge and player:GetGoldenAgeGreatArtistRateModifier() > 0) then
				iGoldenAgeMod = iGoldenAgeMod + player:GetGoldenAgeGreatArtistRateModifier();
			end
		elseif (specialistInfo.GreatPeopleUnitClass == "UNITCLASS_MUSICIAN") then
			iPlayerMod = iPlayerMod + player:GetGreatMusicianRateModifier();
			if (bGoldenAge and player:GetGoldenAgeGreatMusicianRateModifier() > 0) then
				iGoldenAgeMod = iGoldenAgeMod + player:GetGoldenAgeGreatMusicianRateModifier();
			end
		elseif (specialistInfo.GreatPeopleUnitClass == "UNITCLASS_DIPLOMAT") then -- Gazebo CSD
			iPlayerMod = iPlayerMod + player:GetGreatDiplomatRateModifier();
			if (bGoldenAge and player:GetGoldenAgeGreatDiplomatRateModifier() > 0) then
				iGoldenAgeMod = iGoldenAgeMod + player:GetGoldenAgeGreatDiplomatRateModifier();
			end
		elseif (specialistInfo.GreatPeopleUnitClass == "UNITCLASS_SCIENTIST") then
			iPlayerMod = iPlayerMod + player:GetGreatScientistRateModifier();
		elseif (specialistInfo.GreatPeopleUnitClass == "UNITCLASS_MERCHANT") then
			iPlayerMod = iPlayerMod + player:GetGreatMerchantRateModifier();
		elseif (specialistInfo.GreatPeopleUnitClass == "UNITCLASS_ENGINEER") then
			iPlayerMod = iPlayerMod + player:GetGreatEngineerRateModifier();
		end
		
		local iMod = iPlayerMod + iCityMod + iGoldenAgeMod;
		iGPPChange = (iGPPChange * (100 + iMod)) / 100;
		
		return math.floor(iGPPChange/100);
	else
		return 0;
	end
end

-------------------------------------------------
-------------------------------------------------
function OnArtistToggle()
	local bWasHidden = Controls.ArtistStack:IsHidden();
	Controls.ArtistStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.ArtistToggle:LocalizeAndSetText("TXT_KEY_GP_ARTIST_DETAILS_COLLAPSE");
	else
		Controls.ArtistToggle:LocalizeAndSetText("TXT_KEY_GP_ARTIST_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.ArtistToggle:RegisterCallback( Mouse.eLClick, OnArtistToggle );
-------------------------------------------------
-- JFD CID
-------------------------------------------------
function OnDignitaryToggle()
	local bWasHidden = Controls.DignitaryStack:IsHidden();
	Controls.DignitaryStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.DignitaryToggle:LocalizeAndSetText("TXT_KEY_GP_DIGNITARY_DETAILS_COLLAPSE");
	else
		Controls.DignitaryToggle:LocalizeAndSetText("TXT_KEY_GP_DIGNITARY_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.DignitaryToggle:RegisterCallback( Mouse.eLClick, OnDignitaryToggle );
-------------------------------------------------
-- Gazebo CSD
-------------------------------------------------
function OnDiplomatToggle()
	local bWasHidden = Controls.DiplomatStack:IsHidden();
	Controls.DiplomatStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.DiplomatToggle:LocalizeAndSetText("TXT_KEY_GP_DIPLOMAT_DETAILS_COLLAPSE");
	else
		Controls.DiplomatToggle:LocalizeAndSetText("TXT_KEY_GP_DIPLOMAT_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.DiplomatToggle:RegisterCallback( Mouse.eLClick, OnDiplomatToggle );
-------------------------------------------------
-- JFD CID
-------------------------------------------------
function OnDoctorToggle()
	local bWasHidden = Controls.DoctorStack:IsHidden();
	Controls.DoctorStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.DoctorToggle:LocalizeAndSetText("TXT_KEY_GP_DOCTOR_DETAILS_COLLAPSE");
	else
		Controls.DoctorToggle:LocalizeAndSetText("TXT_KEY_GP_DOCTOR_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.DoctorToggle:RegisterCallback( Mouse.eLClick, OnDoctorToggle );
-------------------------------------------------
-------------------------------------------------
function OnEngineerToggle()
	local bWasHidden = Controls.EngineerStack:IsHidden();
	Controls.EngineerStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.EngineerToggle:LocalizeAndSetText("TXT_KEY_GP_ENGINEER_DETAILS_COLLAPSE");
	else
		Controls.EngineerToggle:LocalizeAndSetText("TXT_KEY_GP_ENGINEER_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.EngineerToggle:RegisterCallback( Mouse.eLClick, OnEngineerToggle );
-------------------------------------------------
-- JFD CID
-------------------------------------------------
function OnMagistrateToggle()
	local bWasHidden = Controls.MagistrateStack:IsHidden();
	Controls.MagistrateStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.MagistrateToggle:LocalizeAndSetText("TXT_KEY_GP_MAGISTRATE_DETAILS_COLLAPSE");
	else
		Controls.MagistrateToggle:LocalizeAndSetText("TXT_KEY_GP_MAGISTRATE_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.MagistrateToggle:RegisterCallback( Mouse.eLClick, OnMagistrateToggle );
-------------------------------------------------
-------------------------------------------------
function OnMerchantToggle()
	local bWasHidden = Controls.MerchantStack:IsHidden();
	Controls.MerchantStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.MerchantToggle:LocalizeAndSetText("TXT_KEY_GP_MERCHANT_DETAILS_COLLAPSE");
	else
		Controls.MerchantToggle:LocalizeAndSetText("TXT_KEY_GP_MERCHANT_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.MerchantToggle:RegisterCallback( Mouse.eLClick, OnMerchantToggle );

-------------------------------------------------
-------------------------------------------------
function OnMusicianToggle()
	local bWasHidden = Controls.MusicianStack:IsHidden();
	Controls.MusicianStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.MusicianToggle:LocalizeAndSetText("TXT_KEY_GP_MUSICIAN_DETAILS_COLLAPSE");
	else
		Controls.MusicianToggle:LocalizeAndSetText("TXT_KEY_GP_MUSICIAN_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.MusicianToggle:RegisterCallback( Mouse.eLClick, OnMusicianToggle );

-------------------------------------------------
-------------------------------------------------
function OnScientistToggle()
	local bWasHidden = Controls.ScientistStack:IsHidden();
	Controls.ScientistStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.ScientistToggle:LocalizeAndSetText("TXT_KEY_GP_SCIENTIST_DETAILS_COLLAPSE");
	else
		Controls.ScientistToggle:LocalizeAndSetText("TXT_KEY_GP_SCIENTIST_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.ScientistToggle:RegisterCallback( Mouse.eLClick, OnScientistToggle );
-------------------------------------------------
-- JFD Piety
-------------------------------------------------
function OnProphetToggle()
	local bWasHidden = Controls.ProphetStack:IsHidden();
	Controls.ProphetStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.ProphetToggle:LocalizeAndSetText("TXT_KEY_GP_PROPHET_DETAILS_COLLAPSE");
	else
		Controls.ProphetToggle:LocalizeAndSetText("TXT_KEY_GP_PROPHET_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.ProphetToggle:RegisterCallback( Mouse.eLClick, OnProphetToggle );
-------------------------------------------------
-------------------------------------------------
function OnWriterToggle()
	local bWasHidden = Controls.WriterStack:IsHidden();
	Controls.WriterStack:SetHide( not bWasHidden );
	if( bWasHidden ) then
		Controls.WriterToggle:LocalizeAndSetText("TXT_KEY_GP_WRITER_DETAILS_COLLAPSE");
	else
		Controls.WriterToggle:LocalizeAndSetText("TXT_KEY_GP_WRITER_DETAILS");
	end
	Controls.MainStack:CalculateSize();
	Controls.MainStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:ReprocessAnchoring();
end
Controls.WriterToggle:RegisterCallback( Mouse.eLClick, OnWriterToggle );

-------------------------------------------------
-------------------------------------------------
function SortFunction( a, b )
	local valueA, valueB;
	local entryA = m_SortTable[ tostring( a ) ];
	local entryB = m_SortTable[ tostring( b ) ];
	
	if (entryA == nil) or (entryB == nil) then 
		if entryA and (entryB == nil) then
			return false;
		elseif (entryA == nil) and entryB then
			return true;
		else
			if( m_bSortReverse ) then
				return tostring(a) > tostring(b); -- gotta do something deterministic
			else
				return tostring(a) < tostring(b); -- gotta do something deterministic
			end
		end
	else
		if( m_SortMode == eCity ) then
			valueA = entryA.name;
			valueB = entryB.name;
		elseif( m_SortMode == eTurns ) then
			valueA = entryA.turns;
			valueB = entryB.turns;
			if (valueA == -1) then
				valueA = 9999;
			end
			if (valueB == -1) then
				valueB = 9999;
			end
		end
		
		if( valueA == valueB ) then
			valueA = entryA.cityID;
			valueB = entryB.cityID;
		end
		
		if( m_bSortReverse ) then
			return valueA > valueB;
		else
			return valueA < valueB;
		end
	end
end

-------------------------------------------------
-------------------------------------------------
function OnSort( type )
	if( m_SortMode == type ) then
		m_bSortReverse = not m_bSortReverse;
	else
		m_bSortReverse = false;
	end

	m_SortMode = type;
	Controls.ArtistStack:SortChildren( SortFunction );
	Controls.DignitaryStack:SortChildren( SortFunction ); -- JFD CID
	Controls.DiplomatStack:SortChildren( SortFunction ); -- Gazebo CSD
	Controls.DoctorStack:SortChildren( SortFunction ); -- JFD CID
	Controls.EngineerStack:SortChildren( SortFunction ); 
	Controls.MagistrateStack:SortChildren( SortFunction ); -- JFD CID
	Controls.MerchantStack:SortChildren( SortFunction ); 
	Controls.MusicianStack:SortChildren( SortFunction );
	Controls.ScientistStack:SortChildren( SortFunction );
	Controls.ProphetStack:SortChildren( SortFunction ); -- JFD Piety
	Controls.WriterStack:SortChildren( SortFunction );
end
Controls.SortCity:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortTurns:RegisterCallback( Mouse.eLClick, OnSort );
Controls.SortCity:SetVoid1( eCity );
Controls.SortTurns:SetVoid1( eTurns );

-------------------------------------------------
-------------------------------------------------
function OnOpenInfoCorner( iInfoType )
	if( iInfoType == InfoCornerID.GP ) then
		ContextPtr:SetHide( false );
	else
		ContextPtr:SetHide( true );
	end
end
Events.OpenInfoCorner.Add( OnOpenInfoCorner );

-------------------------------------------------
-------------------------------------------------
function OnCityClick( x, y )
    local plot = Map.GetPlot( x, y );
    if( plot ~= nil ) then
    	UI.DoSelectCityAtPlot( plot );
	end
end
	
-------------------------------------------------
-------------------------------------------------
function OnCityRClick( x, y )
    local plot = Map.GetPlot( x, y );
    if( plot ~= nil ) then
    	UI.LookAt( plot );
	end
end