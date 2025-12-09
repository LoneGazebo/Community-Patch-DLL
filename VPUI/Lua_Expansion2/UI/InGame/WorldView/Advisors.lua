if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Advisor button
-------------------------------------------------
local bModal = false;


g_TutorialQueue = {};	-- Queue of Advisor Messages!

-------------------------------------------------
-------------------------------------------------
function OnAdvisorButton( button )
    if( button == Mouse.eLClick ) then
        if(not  IsAdvisorDisplayOpen() ) then
    		-- this should be reconsidered        		
			local popupInfo = {
				Type = ButtonPopupTypes.BUTTONPOPUP_ADVISOR_COUNSEL,
			}
			Events.SerialEventGameMessagePopup(popupInfo);
    		
    	end
    else
    	AdvisorClose();
    end
end
LuaEvents.AdvisorButtonEvent.Add( OnAdvisorButton );


function AdvisorOpen()
	ContextPtr:SetHide(false);
end

function AdvisorClose()

	if(g_TutorialQueue[1] ~= nil and Controls.DontShowAgainCheckbox:IsChecked()) then
		UI.SetAdvisorMessageHasBeenSeen(g_TutorialQueue[1].IDName, true);
	end
	
	table.remove(g_TutorialQueue, 1);

	ContextPtr:SetHide(true);
	if (bModal) then
		print("Popping Advisor UI");
		UIManager:PopModal(ContextPtr);
	end
end

function IsAdvisorDisplayOpen()
	return (not Controls.AdvisorDisplayFront:IsHidden());
end

function HideAllAdvisors ()
	Controls.ScienceAdvisor:SetHide(true);
	Controls.EconomicAdvisor:SetHide(true);
	Controls.ForeignAdvisor:SetHide(true);
	Controls.MilitaryAdvisor:SetHide(true);	
end


local AdvisorText = {
	[AdvisorTypes.ADVISOR_SCIENCE] = Locale.Lookup("TXT_KEY_ADVISOR_SCIENCE_TITLE"),
	[AdvisorTypes.ADVISOR_ECONOMIC] = Locale.Lookup("TXT_KEY_ADVISOR_ECON_TITLE"),
	[AdvisorTypes.ADVISOR_FOREIGN] = Locale.Lookup("TXT_KEY_ADVISOR_FOREIGN_TITLE"),
	[AdvisorTypes.ADVISOR_MILITARY] = Locale.Lookup("TXT_KEY_ADVISOR_MILITARY_TITLE"),
};

local AdvisorBlocks = {
	[AdvisorTypes.ADVISOR_SCIENCE] = Controls.ScienceAdvisor,
	[AdvisorTypes.ADVISOR_ECONOMIC] = Controls.EconomicAdvisor,
	[AdvisorTypes.ADVISOR_FOREIGN] = Controls.ForeignAdvisor,
	[AdvisorTypes.ADVISOR_MILITARY] = Controls.MilitaryAdvisor,
}

local AdvisorPortraits = {
	[AdvisorTypes.ADVISOR_SCIENCE] = {
		ERA_ANCIENT = Controls.ScienceAdvisorPortrait1,
		ERA_RENAISSANCE = Controls.ScienceAdvisorPortrait2,
		ERA_MODERN = Controls.ScienceAdvisorPortrait3,
	},
	
	[AdvisorTypes.ADVISOR_ECONOMIC] = {
		ERA_ANCIENT = Controls.EconomicAdvisorPortrait1,
		ERA_RENAISSANCE = Controls.EconomicAdvisorPortrait2,
		ERA_MODERN = Controls.EconomicAdvisorPortrait3,
	},
	
	[AdvisorTypes.ADVISOR_FOREIGN] = {
		ERA_ANCIENT = Controls.ForeignAdvisorPortrait1,
		ERA_RENAISSANCE = Controls.ForeignAdvisorPortrait2,
		ERA_MODERN = Controls.ForeignAdvisorPortrait3,
	},
	
	[AdvisorTypes.ADVISOR_MILITARY] = {
		ERA_ANCIENT = Controls.MilitaryAdvisorPortrait1,
		ERA_RENAISSANCE = Controls.MilitaryAdvisorPortrait2,
		ERA_MODERN = Controls.MilitaryAdvisorPortrait3,
	},
}

function SetAdvisorDisplay (iAdvisor)
	HideAllAdvisors();

	Controls.AdvisorDismissButton:SetHide(false);
	
	local advisorText = AdvisorText[iAdvisor];
	Controls.AdvisorTitleText:SetText(advisorText);
	
	local advisor = AdvisorBlocks[iAdvisor];
	advisor:SetHide(false);
	
	local player = Players[Game.GetActivePlayer()];
	local currentEra = player:GetCurrentEra();
	
	local bestPortrait = nil;
	local bestPortraitEra = -2;
	
	for eraType, portrait in pairs(AdvisorPortraits[iAdvisor]) do
		
		-- Hide portrait by default.
		portrait:SetHide(true);
		
		local era = GameInfo.Eras[eraType];
		local eraID = (era ~= nil) and era.ID or -1;
		
		-- If it is nil, assign it SOMETHING.
		if(bestPortrait == nil) then
			bestPortrait = portrait;
		end
		
		if(eraID > bestPortraitEra and eraID <= currentEra) then
			bestPortrait = portrait;
			bestPortraitEra = eraID;
		end		
	end
	
	bestPortrait:SetHide(false);
end

function ResetAdvisorDisplay ()
	HideAllAdvisors();
	Controls.ActivateButton:SetHide(true);
	Controls.Question1String:SetHide(true);
	Controls.Question2String:SetHide(true);
	Controls.Question3String:SetHide(true);
	Controls.Question4String:SetHide(true);
	
	SetAdvisorDisplay(0);
	
	Controls.AdvisorHeaderText:SetText( Locale.ConvertTextKey( "TXT_KEY_ADVISOR_RESET_TITLE" ));
	Controls.AdvisorBodyText:SetText( Locale.ConvertTextKey( "TXT_KEY_ADVISOR_RESET_TEXT" ));
end

function OnAdvisorHelpClicked ()
	local popupInfo = 
	{
		Type  = 99997,
	};
	Events.SerialEventGameMessagePopup(popupInfo);
end
Controls.ActivateButton:RegisterCallback( Mouse.eLClick, OnAdvisorHelpClicked );

function OnAdvisorDismissClicked ()
	Events.AdvisorDisplayHide();
end
Controls.AdvisorDismissButton:RegisterCallback( Mouse.eLClick, OnAdvisorDismissClicked );

local AdvisorConcept1 = "";
local AdvisorConcept2 = "";
local AdvisorConcept3 = "";

function OnQuestion1Clicked ()
	local popupInfo = {
		Type = ButtonPopupTypes.BUTTONPOPUP_ADVISOR_INFO,
		Text = AdvisorConcept1,
		Priority = PopupPriority.Current,
	}
	Events.SerialEventGameMessagePopup(popupInfo);
end
Controls.Question1String:RegisterCallback( Mouse.eLClick, OnQuestion1Clicked);

function OnQuestion2Clicked ()
	local popupInfo = {
		Type = ButtonPopupTypes.BUTTONPOPUP_ADVISOR_INFO,
		Text = AdvisorConcept2,
		Priority = PopupPriority.Current,
	}
	Events.SerialEventGameMessagePopup(popupInfo);
end
Controls.Question2String:RegisterCallback( Mouse.eLClick, OnQuestion2Clicked);

function OnQuestion3Clicked ()
	local popupInfo = {
		Type = ButtonPopupTypes.BUTTONPOPUP_ADVISOR_INFO,
		Text = AdvisorConcept3,
		Priority = PopupPriority.Current,
	}
	Events.SerialEventGameMessagePopup(popupInfo);
end
Controls.Question3String:RegisterCallback( Mouse.eLClick, OnQuestion3Clicked);

function OnAdvisorDisplayShow (eventInfo)

	table.insert(g_TutorialQueue, eventInfo);
	
	ResetAdvisorDisplay();

	AdvisorConcept1 = eventInfo.Concept1;
	AdvisorConcept2 = eventInfo.Concept2;
	AdvisorConcept3 = eventInfo.Concept3;

	SetAdvisorDisplay(eventInfo.Advisor);
	
	-- NOTE: This doesn't take into account pesturing tutorials which should never be marked.
	print("Marking " .. eventInfo.IDName .. " as seen");
	Game.SetAdvisorMessageHasBeenSeen(eventInfo.IDName, true);
		
	Controls.DontShowAgainCheckbox:SetCheck(false);
	
	Controls.AdvisorHeaderText:LocalizeAndSetText(eventInfo.TitleText);
	Controls.AdvisorBodyText:LocalizeAndSetText(eventInfo.BodyText);
	
	if (eventInfo.ActivateButtonText ~= nil and eventInfo.ActivateButtonText ~= "") then
		Controls.ActivateButtonText:SetText(eventInfo.ActivateButtonText);
		Controls.ActivateButton:SetHide(false);
	else 
		Controls.ActivateButton:SetHide(true);
	end
	
	if (eventInfo.Concept1 ~= nil and eventInfo.Concept1 ~= "") then
		local concept = GameInfo.Concepts[eventInfo.Concept1];
		if(concept ~= nil) then
			Controls.Question1String:SetHide(false);
			Controls.Question1String:SetText(Locale.ConvertTextKey(concept.AdvisorQuestion));
		else
			Controls.Question1String:SetHide(true);
			print("Could not find concept. " .. eventInfo.Concept1);
		end
	else
		Controls.Question1String:SetHide(true);
	end

	if (eventInfo.Concept2 ~= nil and eventInfo.Concept2 ~= "") then
		local concept = GameInfo.Concepts[eventInfo.Concept2];
		if(concept ~= nil) then
			Controls.Question2String:SetHide(false);
			Controls.Question2String:SetText(Locale.ConvertTextKey(concept.AdvisorQuestion));
		else
			Controls.Question2String:SetHide(true);
			print("Could not find concept. " .. eventInfo.Concept2);
		end
	else
		Controls.Question2String:SetHide(true);
	end

	if (eventInfo.Concept3 ~= nil and eventInfo.Concept3 ~= "") then
		local concept = GameInfo.Concepts[eventInfo.Concept3];
		if(concept ~= nil) then
			Controls.Question3String:SetHide(false);
			Controls.Question3String:SetText(Locale.ConvertTextKey(concept.AdvisorQuestion));		
		else
			Controls.Question3String:SetHide(true);
			print("Could not find concept. " .. eventInfo.Concept3);
		end
	else
		Controls.Question3String:SetHide(true);
	end
	
	if (eventInfo.Modal) then
		UIManager:PushModal(ContextPtr, true);	
		bModal = true;
	end
	
	AdvisorOpen();
end
Events.AdvisorDisplayShow.Add(OnAdvisorDisplayShow);

function OnClearAdvice()
	AdvisorClose();
	ResetAdvisorDisplay();
end
Events.AdvisorDisplayHide.Add( OnClearAdvice );


----------------------------------------------------------------        
-- Key Down Processing
----------------------------------------------------------------        
function InputHandler(uiMsg, wParam, lParam)
    if( IsAdvisorDisplayOpen() and
        uiMsg == KeyEvents.KeyDown ) then
        if( wParam == Keys.VK_ESCAPE ) then
			AdvisorClose();
            return true;
        end
    end
    
    return false;
end
ContextPtr:SetInputHandler( InputHandler );

---------------------
ResetAdvisorDisplay();