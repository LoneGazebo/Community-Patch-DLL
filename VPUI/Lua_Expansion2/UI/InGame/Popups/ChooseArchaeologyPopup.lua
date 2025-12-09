if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end

-------------------------------------------------
-- Choose Archaeology Popup
-------------------------------------------------

include( "IconSupport" );
include( "InstanceManager" );

-- Used for Piano Keys
local ltBlue = {19/255,32/255,46/255,120/255};
local dkBlue = {12/255,22/255,30/255,120/255};

local g_ItemManager = InstanceManager:new( "ItemInstance", "Button", Controls.ItemStack );

g_iUnitIndex = -1;
g_iWork = -1;

local bHidden = true;

local screenSizeX, screenSizeY = UIManager:GetScreenSizeVal()
local spWidth, spHeight = Controls.ItemScrollPanel:GetSizeVal();

-- Original UI designed at 1050px 
local heightOffset = screenSizeY - 1020;

spHeight = spHeight + heightOffset;
Controls.ItemScrollPanel:SetSizeVal(spWidth, spHeight); 
Controls.ItemScrollPanel:CalculateInternalSize();
Controls.ItemScrollPanel:ReprocessAnchoring();

local bpWidth, bpHeight = Controls.BottomPanel:GetSizeVal();
bpHeight = bpHeight + heightOffset 

Controls.BottomPanel:SetSizeVal(bpWidth, bpHeight);
Controls.BottomPanel:ReprocessAnchoring();

local g_PopupInfo = nil;
-------------------------------------------------
-------------------------------------------------
function OnPopupMessage(popupInfo)
	  
	local popupType = popupInfo.Type;
	
	g_iUnitIndex = popupInfo.Data2;
	
	if popupType ~= ButtonPopupTypes.BUTTONPOPUP_CHOOSE_ARCHAEOLOGY then
		return;
	end	
	
	g_PopupInfo = popupInfo;
	
   	UIManager:QueuePopup( ContextPtr, PopupPriority.SocialPolicy );
end
Events.SerialEventGameMessagePopup.Add( OnPopupMessage );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    ----------------------------------------------------------------        
    -- Key Down Processing
    ----------------------------------------------------------------        
    if uiMsg == KeyEvents.KeyDown then
        if (wParam == Keys.VK_ESCAPE) then
			if(not Controls.ChooseConfirm:IsHidden()) then
				Controls.ChooseConfirm:SetHide(true);
			else
				OnClose();
			end
        	return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
	Controls.ChooseConfirm:SetHide(true);
    UIManager:DequeuePopup(ContextPtr);
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function RefreshList()
	g_ItemManager:ResetInstances();
		
	local pPlayer = Players[Game.GetActivePlayer()];
	CivIconHookup( pPlayer:GetID(), 64, Controls.CivIcon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
	
	local archeologyUnit = GameInfo.Units["UNIT_ARCHAEOLOGIST"];
	
	IconHookup(archeologyUnit.PortraitIndex, 64, archeologyUnit.IconAtlas, Controls.ArcheologyIcon);
	
	local bShow2ndPlayer = true;
	local bArtSlotOpen = pPlayer:HasAvailableGreatWorkSlot(GameInfo.GreatWorkSlots.GREAT_WORK_SLOT_ART_ARTIFACT.ID);
	local bWritingSlotOpen = pPlayer:HasAvailableGreatWorkSlot(GameInfo.GreatWorkSlots.GREAT_WORK_SLOT_LITERATURE.ID);
	local pPlot = pPlayer:GetNextDigCompletePlot();
	UI.LookAt(pPlot, 0);
	local bWrittenArtifact = pPlot:HasWrittenArtifact();
	
	Controls.ArchDescLeadIn:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_LEAD_IN");
	
	local szTypeString;
	local szDetails1String;
	local szDetails2String;
	local szDetails3String;
	local szDetails4String;
	local szDateString;

	local iType = pPlot:GetArchaeologyArtifactType();
	local pPlayer1 = Players[pPlot:GetArchaeologyArtifactPlayer1()];
	local pPlayer2 = Players[pPlot:GetArchaeologyArtifactPlayer2()];
	szDateString = Locale.ConvertTextKey(GameInfo.Eras[pPlot:GetArchaeologyArtifactEra()].Description);
	szDetails1String = Locale.Lookup(Game.GetArtifactName(pPlot));
	
	if (iType == GameInfo.GreatWorkArtifactClasses.ARTIFACT_BATTLE_RANGED.ID) then
		szTypeString = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_ARTIFACT_TYPE1");
		szDetails2String = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_ATTACKER") .. " " .. Locale.ConvertTextKey(pPlayer1:GetCivilizationShortDescriptionKey());
		szDetails3String = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_DEFENDER") .. " " .. Locale.ConvertTextKey(pPlayer2:GetCivilizationShortDescriptionKey());
		szDetails4String = szDateString;	
	elseif (iType == GameInfo.GreatWorkArtifactClasses.ARTIFACT_BATTLE_MELEE.ID) then
		szTypeString = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_ARTIFACT_TYPE2");
		szDetails2String = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_ATTACKER") .. " " .. Locale.ConvertTextKey(pPlayer1:GetCivilizationShortDescriptionKey());
		szDetails3String = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_DEFENDER") .. " " .. Locale.ConvertTextKey(pPlayer2:GetCivilizationShortDescriptionKey());
		szDetails4String = szDateString;
	elseif (iType == GameInfo.GreatWorkArtifactClasses.ARTIFACT_BARBARIAN_CAMP.ID) then
		szTypeString = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_ARTIFACT_TYPE3");
		szDetails2String = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_PLUNDERER") .. " " .. Locale.ConvertTextKey(pPlayer1:GetCivilizationShortDescriptionKey());
		szDetails3String = szDateString;
		szDetails4String = "";
		bShow2ndPlayer = false;
	elseif (iType == GameInfo.GreatWorkArtifactClasses.ARTIFACT_ANCIENT_RUIN.ID) then
		szTypeString = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_ARTIFACT_TYPE4");
		szDetails2String = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_PLUNDERER") .. " " .. Locale.ConvertTextKey(pPlayer1:GetCivilizationShortDescriptionKey());
		szDetails3String = szDateString;
		szDetails4String = "";
		bShow2ndPlayer = false;
	elseif (iType == GameInfo.GreatWorkArtifactClasses.ARTIFACT_RAZED_CITY.ID) then
		szTypeString = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_ARTIFACT_TYPE5");
		szDetails1String = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_CONQUEROR") .. " " .. Locale.ConvertTextKey(pPlayer1:GetCivilizationShortDescriptionKey());
		szDetails2String = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_FOUNDER") .. " " .. Locale.ConvertTextKey(pPlayer2:GetCivilizationShortDescriptionKey());
		szDetails3String = szDateString;
		szDetails4String = "";
	elseif (iType == GameInfo.GreatWorkArtifactClasses.ARTIFACT_WRITING.ID) then
	    g_iWork = pPlot:GetArchaeologyArtifactWork();
		szTypeString = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_ARTIFACT_TYPE6");
		szDetails1String = Locale.ConvertTextKey(GameInfo.GreatWorks[g_iWork].Description);
		szDetails2String = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_AUTHOR") .. " " .. Locale.ConvertTextKey(pPlayer1:GetCivilizationShortDescriptionKey());
		szDetails3String = szDateString;
		szDetails4String = "";
	end
	
	Controls.ArchDescTitle:SetText(szTypeString);	
	Controls.ArchDescLine1:SetText(szDetails1String);
	Controls.ArchDescLine2:SetText(szDetails2String);
	Controls.ArchDescLine3:SetText(szDetails3String);
	Controls.ArchDescLine4:SetText(szDetails4String);
	
	if ((bWrittenArtifact and not bWritingSlotOpen) or (not bWrittenArtifact and not bArtSlotOpen)) then
		Controls.ArchDescLine5:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_NO_SLOTS");		
	else
		Controls.ArchDescLine5:SetText("");	
	end
	
	if (bWrittenArtifact) then
		Controls.ViewButton:SetHide(false);
		Controls.ViewButton:RegisterCallback(Mouse.eLClick, function() DoPopup(); end);
	else
		Controls.ViewButton:SetHide(true);
	end
	
	-- First choice: Player 1 artifact
	if (not bWrittenArtifact and bArtSlotOpen) then
		local itemInstance = g_ItemManager:GetInstance();
		itemInstance.Name:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_ARTIFACT_HEADER", Locale.ConvertTextKey(pPlayer1:GetCivilizationAdjectiveKey()));
		itemInstance.Description:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_ARTIFACT_RESULT");	
		itemInstance.Button:RegisterCallback(Mouse.eLClick, function() SelectArchaeologyChoice(2); end);
		itemInstance.Box:SetColorVal(unpack(ltBlue));
		itemInstance.GreatArtifactIcon:SetHide(false);
		itemInstance.GreatArtifactWritingIcon:SetHide(true);
					
		itemInstance.Icon:SetTexture("CreateArtifact64.dds");
		itemInstance.Icon:SetTextureOffsetVal(0,0);
		
		itemInstance.SubIconFrame:SetHide(false);
		
		CivIconHookup(pPlayer1:GetID(), 32, itemInstance.SubIcon, itemInstance.SubIconBG, itemInstance.SubIconShadow, false, true, itemInstance.SubIconHL);
		
		itemInstance.TextStack:CalculateSize();
		itemInstance.TextStack:ReprocessAnchoring();
		itemInstance.TitleStack:CalculateSize();
		itemInstance.TitleStack:ReprocessAnchoring();
		itemInstance.EntryStack:CalculateSize();
		itemInstance.EntryStack:ReprocessAnchoring();
		
		local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
		local descWidth, descHeight = itemInstance.EntryStack:GetSizeVal();
		local newHeight = descHeight + 20;	
		itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
		itemInstance.Box:SetSizeVal(buttonWidth + 20, newHeight);
		itemInstance.BounceAnim:SetSizeVal(buttonWidth + 20, newHeight + 5);
		itemInstance.BounceGrid:SetSizeVal(buttonWidth + 20, newHeight + 5);
	end
	
	-- First choice: for written artifact
	if (bWrittenArtifact and bWritingSlotOpen) then
		local itemInstance = g_ItemManager:GetInstance();
		itemInstance.Name:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_WRITING_HEADER", Locale.ConvertTextKey(pPlayer1:GetCivilizationAdjectiveKey()));
		itemInstance.Description:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_WRITTEN_ARTIFACT_RESULT");	
		itemInstance.Button:RegisterCallback(Mouse.eLClick, function() SelectArchaeologyChoice(5); end);
		itemInstance.Box:SetColorVal(unpack(ltBlue));
		itemInstance.GreatArtifactIcon:SetHide(true);
		itemInstance.GreatArtifactWritingIcon:SetHide(false);
					
		itemInstance.Icon:SetTexture("CreateGreatWorksAtlas64_Exp2.dds");
		itemInstance.Icon:SetTextureOffsetVal(0,64);
		
		itemInstance.SubIconFrame:SetHide(false);
		
		CivIconHookup(pPlayer1:GetID(), 32, itemInstance.SubIcon, itemInstance.SubIconBG, itemInstance.SubIconShadow, false, true, itemInstance.SubIconHL);
		
		itemInstance.TextStack:CalculateSize();
		itemInstance.TextStack:ReprocessAnchoring();
		itemInstance.TitleStack:CalculateSize();
		itemInstance.TitleStack:ReprocessAnchoring();
		itemInstance.EntryStack:CalculateSize();
		itemInstance.EntryStack:ReprocessAnchoring();
		
		local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
		local descWidth, descHeight = itemInstance.EntryStack:GetSizeVal();
		local newHeight = descHeight + 20;	
		itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
		itemInstance.Box:SetSizeVal(buttonWidth + 20, newHeight);
		itemInstance.BounceAnim:SetSizeVal(buttonWidth + 20, newHeight + 5);
		itemInstance.BounceGrid:SetSizeVal(buttonWidth + 20, newHeight + 5);
	end
	
	-- Second choice: Player 2 artifact
	if (not bWrittenArtifact and bArtSlotOpen and bShow2ndPlayer) then
		local itemInstance = g_ItemManager:GetInstance();
		itemInstance.Name:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_ARTIFACT_HEADER", Locale.ConvertTextKey(pPlayer2:GetCivilizationAdjectiveKey()));
		itemInstance.Description:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_ARTIFACT_RESULT");	
		itemInstance.Button:RegisterCallback(Mouse.eLClick, function() SelectArchaeologyChoice(3); end);
		itemInstance.Box:SetColorVal(unpack(ltBlue));
		itemInstance.GreatArtifactIcon:SetHide(false);
		itemInstance.GreatArtifactWritingIcon:SetHide(true);
		
		itemInstance.SubIconFrame:SetHide(false);
		
		CivIconHookup(pPlayer2:GetID(), 32, itemInstance.SubIcon, itemInstance.SubIconBG, itemInstance.SubIconShadow, false, true, itemInstance.SubIconHL);
		
		itemInstance.Icon:SetTexture("CreateArtifact64.dds");
		itemInstance.Icon:SetTextureOffsetVal(0,0);
		
		itemInstance.TextStack:CalculateSize();
		itemInstance.TextStack:ReprocessAnchoring();
		itemInstance.TitleStack:CalculateSize();
		itemInstance.TitleStack:ReprocessAnchoring();
		itemInstance.EntryStack:CalculateSize();
		itemInstance.EntryStack:ReprocessAnchoring();
		
		local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
		local descWidth, descHeight = itemInstance.EntryStack:GetSizeVal();
		local newHeight = descHeight + 20;	
		itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
		itemInstance.Box:SetSizeVal(buttonWidth + 20, newHeight);
		itemInstance.BounceAnim:SetSizeVal(buttonWidth + 20, newHeight + 5);
		itemInstance.BounceGrid:SetSizeVal(buttonWidth + 20, newHeight + 5);	
	end
	
	-- Third choice: Create Landmark or Cultural Renaissance
	local itemInstance = g_ItemManager:GetInstance();
	if (not bWrittenArtifact) then
		itemInstance.Name:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_LANDMARK_HEADER");
		local strLandmarkText = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_LANDMARK_RESULT");
		if (pPlot:GetOwner() ~= -1) then
			if (Players[pPlot:GetOwner()]:IsMinorCiv()) then
				strLandmarkText = strLandmarkText .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_LANDMARK_MINOR_CIV");
			elseif (not Players[pPlot:GetOwner()]:IsHuman()) then
				strLandmarkText = strLandmarkText .. Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_LANDMARK_MAJOR_CIV");
			end
		end
		itemInstance.Description:SetText(strLandmarkText);	
		itemInstance.Button:RegisterCallback(Mouse.eLClick, function() SelectArchaeologyChoice(1); end);
		itemInstance.Box:SetColorVal(unpack(ltBlue));
		itemInstance.GreatArtifactIcon:SetHide(true);
		itemInstance.GreatArtifactWritingIcon:SetHide(true);
		itemInstance.SubIconFrame:SetHide(true);
		local landmarkImprovement = GameInfo.Improvements["IMPROVEMENT_LANDMARK"];
		IconHookup(landmarkImprovement.PortraitIndex, 64, landmarkImprovement.IconAtlas, itemInstance.Icon);
	else	
		itemInstance.Name:LocalizeAndSetText("TXT_KEY_CHOOSE_ARCH_RENAISSANCE_HEADER");
		local strCultureText = Locale.ConvertTextKey("TXT_KEY_CHOOSE_ARCH_RENAISSANCE_RESULT");
		strCultureText = strCultureText .. "[NEWLINE]----------------[NEWLINE]+" .. pPlayer:GetWrittenArtifactCulture() .. "[ICON_CULTURE]";
		itemInstance.Description:SetText(strCultureText);	
		itemInstance.Button:RegisterCallback(Mouse.eLClick, function() SelectArchaeologyChoice(4); end);
		itemInstance.Box:SetColorVal(unpack(ltBlue));
		itemInstance.GreatArtifactIcon:SetHide(true);
		itemInstance.GreatArtifactWritingIcon:SetHide(true);
		itemInstance.SubIconFrame:SetHide(true);	
		itemInstance.Icon:SetTexture("CreateGreatWorksAtlas64_Exp2.dds");
		itemInstance.Icon:SetTextureOffsetVal(0,0);
	end

	itemInstance.TextStack:CalculateSize();
	itemInstance.TextStack:ReprocessAnchoring();
	itemInstance.TitleStack:CalculateSize();
	itemInstance.TitleStack:ReprocessAnchoring();
	itemInstance.EntryStack:CalculateSize();
	itemInstance.EntryStack:ReprocessAnchoring();
		
	local buttonWidth, buttonHeight = itemInstance.Button:GetSizeVal();
	local descWidth, descHeight = itemInstance.EntryStack:GetSizeVal();
	local newHeight = descHeight + 20;	
    itemInstance.Button:SetSizeVal(buttonWidth, newHeight);
	itemInstance.Box:SetSizeVal(buttonWidth + 20, newHeight);
	itemInstance.BounceAnim:SetSizeVal(buttonWidth + 20, newHeight + 5);
	itemInstance.BounceGrid:SetSizeVal(buttonWidth + 20, newHeight + 5);
	
	Controls.ItemStack:CalculateSize();
	Controls.ItemStack:ReprocessAnchoring();
	Controls.ItemScrollPanel:CalculateInternalSize();
end

function DoPopup(iChoice) 
		local popupInfo = {
			Type = ButtonPopupTypes.BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER,
			Data1 = -1,
			Data2 = g_iWork,
			Priority = PopupPriority.Current
		}
		Events.SerialEventGameMessagePopup(popupInfo);
end

function SelectArchaeologyChoice(iChoice) 
	g_iChoice = iChoice;
	
	Controls.ConfirmText:LocalizeAndSetText("TXT_KEY_POPUP_ARE_YOU_SURE"); 
	Controls.ChooseConfirm:SetHide(false);
end

function OnConfirmYes( )
	Controls.ChooseConfirm:SetHide(true);
	
	-- Extra splash screen for written artifacts
	if (g_iChoice == 5) then
		local popupInfo = {
			Type = ButtonPopupTypes.BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER,
			Data1 = -1,
			Data2 = g_iWork,
			Priority = PopupPriority.Current
		}
				
		Events.SerialEventGameMessagePopup(popupInfo);
	end
	
	Network.SendArchaeologyChoice(Game.GetActivePlayer(), g_iUnitIndex, g_iChoice);
	Events.AudioPlay2DSound("AS2D_INTERFACE_POLICY");	
	OnClose();
end
Controls.ConfirmYes:RegisterCallback( Mouse.eLClick, OnConfirmYes );

function OnConfirmNo( )
	Controls.ChooseConfirm:SetHide(true);
end
Controls.ConfirmNo:RegisterCallback( Mouse.eLClick, OnConfirmNo );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

	bHidden = bIsHide;
    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	Events.SerialEventGameMessagePopupShown(g_PopupInfo);
        	
        	RefreshList();
        
			local unitPanel = ContextPtr:LookUpControl( "/InGame/WorldView/UnitPanel/Base" );
			if( unitPanel ~= nil ) then
				unitPanel:SetHide( true );
			end
			
			local infoCorner = ContextPtr:LookUpControl( "/InGame/WorldView/InfoCorner" );
			if( infoCorner ~= nil ) then
				infoCorner:SetHide( true );
			end
               	
        else
      
			local unitPanel = ContextPtr:LookUpControl( "/InGame/WorldView/UnitPanel/Base" );
			if( unitPanel ~= nil ) then
				unitPanel:SetHide(false);
			end
			
			local infoCorner = ContextPtr:LookUpControl( "/InGame/WorldView/InfoCorner" );
			if( infoCorner ~= nil ) then
				infoCorner:SetHide(false);
			end
			
			if(g_PopupInfo ~= nil) then
				Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_PopupInfo.Type, 0);
			end
            UI.decTurnTimerSemaphore();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

function OnDirty()
	-- If the user performed any action that changes selection states, just close this UI.
	if not bHidden then
		OnClose();
	end
end
Events.UnitSelectionChanged.Add( OnDirty );
