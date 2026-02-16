if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- AdvisorInfo Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
local g_InstanceManager = InstanceManager:new( "RelatedConceptButton", "Button", Controls.RelatedConceptButtonStack );
local g_strConceptList = {};
local g_HistoryList = {};
local g_intHistoryLoc = 0;
local g_intHistorySize = 0;
local g_AdvisorModal = nil;

local m_PopupInfo = nil;

function HideAllAdvisors ()
	Controls.EconomicAdvisor:SetHide(true);
	Controls.ForeignAdvisor:SetHide(true);
	Controls.MilitaryAdvisor:SetHide(true);
	Controls.ScienceAdvisor:SetHide(true);
end

function AddToHistory (strConceptKey)
	g_intHistoryLoc = g_intHistoryLoc + 1;
	g_HistoryList[g_intHistoryLoc] = strConceptKey;
	g_intHistorySize = g_intHistoryLoc;
end

function CanGoBackInHistory ()
	if (g_intHistoryLoc > 1) then
		return true;
	else
		return false;
	end
end

function CanGoForwardInHistory ()
	if (g_intHistoryLoc < g_intHistorySize) then
		return true;
	else
		return false;
	end
end

function BackInHistory ()
	if (CanGoBackInHistory()) then
		--print("Going back!");
		g_intHistoryLoc = g_intHistoryLoc - 1;
		ShowConcept(g_HistoryList[g_intHistoryLoc]);
	end
end

function ForwardInHistory ()
	if (CanGoForwardInHistory) then
		--print("Going forward!");
		g_intHistoryLoc = g_intHistoryLoc + 1;
		ShowConcept(g_HistoryList[g_intHistoryLoc]);
	end
end

function ConceptSelected ( index )
	--print("ConceptSelected " .. index);
	local strNewConcept = g_strConceptList[index];
	--print("strNewConcept " .. strNewConcept);
	if strNewConcept then	
		AddToHistory(strNewConcept);
		ShowConcept(strNewConcept);
	end
end

local g_strConcept;

function ShowConcept( strConceptKey )

	g_strConcept = strConceptKey;

	Controls.TitleLabel:SetText(Locale.ConvertTextKey(GameInfo.Concepts[strConceptKey].Description))
	Controls.DescriptionLabel:SetText(Locale.ConvertTextKey(GameInfo.Concepts[strConceptKey].Summary));
	Controls.DescriptionPanel:CalculateInternalSize();
		
	HideAllAdvisors();
	
	Controls.Back:SetHide(not CanGoBackInHistory());
	Controls.Forward:SetHide(not CanGoForwardInHistory());
		
	local strAdvisor = GameInfo.Concepts[strConceptKey].Advisor;
	local strAdvisorText = "";
	local imgCtrl;
	if (strAdvisor == "MILITARY") then
		strAdvisorText = "TXT_KEY_ADVISOR_MILITARY_TITLE";
		imgCtrl = Controls.MilitaryAdvisor;
	elseif (strAdvisor == "ECONOMIC") then
		strAdvisorText = "TXT_KEY_ADVISOR_ECON_TITLE";
		imgCtrl = Controls.EconomicAdvisor;
	elseif (strAdvisor == "FOREIGN") then
		strAdvisorText = "TXT_KEY_ADVISOR_FOREIGN_TITLE";
		imgCtrl = Controls.ForeignAdvisor;
	elseif (strAdvisor == "SCIENCE") then
		strAdvisorText = "TXT_KEY_ADVISOR_SCIENCE_TITLE";
		imgCtrl = Controls.ScienceAdvisor;
	end
	Controls.AdvisorLabel:SetText(Locale.ConvertTextKey(strAdvisorText));	
	imgCtrl:SetHide(false);
		
	-- build concept list
	g_InstanceManager:ResetInstances();
	
	g_strConceptList = {};
	local i = 0;

	--print("Build related concepts");
	
	for row in GameInfo.Concepts_RelatedConcept{ConceptType = strConceptKey} do
		--print("row.RelatedConcept: " .. row.RelatedConcept);
		local strRelatedConcept = GameInfo.Concepts[row.RelatedConcept].Type;
		
		local controlTable = g_InstanceManager:GetInstance();
		controlTable.Button:SetText(Locale.ConvertTextKey(GameInfo.Concepts[strRelatedConcept].Description));
		controlTable.Button:RegisterCallback( Mouse.eLClick, ConceptSelected );
		controlTable.Button:SetVoid1(i);
		
		g_strConceptList[ i ] = strRelatedConcept;
		i = i + 1;
	end
	
	if (GameInfo.Concepts[strConceptKey].CivilopediaPage) then
		Controls.Civilopedia_List:SetHide(false);
		Controls.Civilopedia_List:SetText(Locale.ConvertTextKey(GameInfo.Concepts[strConceptKey].CivilopediaPageText));
	else
		Controls.Civilopedia_List:SetHide(true);		
	end
	
	Controls.RelatedConceptButtonStack:CalculateSize();
	Controls.RelatedConceptButtonStack:ReprocessAnchoring();
	Controls.RelatedConceptPanel:CalculateInternalSize();
end


-------------------------------------------------
-- On Display
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_ADVISOR_INFO ) then
		return;
	end

	CivIconHookup( Game.GetActivePlayer(), 64, Controls.Icon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
	
	m_PopupInfo = popupInfo;
	local strConceptKey = popupInfo.Text;
	ShowConcept(strConceptKey);
	AddToHistory(strConceptKey);
	
	g_AdvisorModal = nil;
	-- If the main advisor panel is on the modal stack, hide it	
	local uiCount = UIManager:GetNamedContextCount("Advisors");	
	for i = 1, uiCount do	
		local advisorModal = UIManager:GetNamedContextByIndex("Advisors", i - 1);
		if (advisorModal ~= nil) then
			if (UIManager:IsTopModal(advisorModal)) then
				advisorModal:SetModal(false);	-- This will just hide it in the modal stack, it will still be on the stack
				g_AdvisorModal = advisorModal;				
				break
			end
		end
	end
	
	-- If the priority is PopupPriority.Default, then use our default, else just pass it through
	local iPriority = popupInfo.Priority;
	if (iPriority == PopupPriority.Default) then 
		iPriority = PopupPriority.AdvisorInfo ;
	end
	UIManager:QueuePopup( ContextPtr, iPriority);
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


----------------------------------------------------------------        
----------------------------------------------------------------        
function Close ()

	print("Closing Advisor Info");
	-- If the main advisor panel was on the modal stack, show it
	if (g_AdvisorModal ~= nil) then
		print("Advisor modal not nil");
		if (UIManager:IsModal(g_AdvisorModal)) then
			print("Setting Advisor to Modal");
			g_AdvisorModal:SetModal(true);	-- If it is still on the stack, restore it
		end
		g_AdvisorModal = nil;
	end

	UIManager:DequeuePopup( ContextPtr );
end


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCloseButtonClicked ()
	Close();
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnCloseButtonClicked );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCivilopediaClicked ()
	if (g_strConcept) then
		-- search by description
		local searchString = GameInfo.Concepts[g_strConcept].Description;
		Events.SearchForPediaEntry( searchString );
	end
end
Controls.Civilopedia:RegisterCallback( Mouse.eLClick, OnCivilopediaClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCivilopediaListClicked ()
	if (g_strConcept) then
		Events.GoToPediaHomePage( GameInfo.Concepts[g_strConcept].CivilopediaPage );
	end
end
Controls.Civilopedia_List:RegisterCallback( Mouse.eLClick, OnCivilopediaListClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnForwardClicked()
	ForwardInHistory();
end
Controls.Forward:RegisterCallback( Mouse.eLClick, OnForwardClicked );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnBackClicked()
	BackInHistory();
end
Controls.Back:RegisterCallback( Mouse.eLClick, OnBackClicked );


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

function UpdateAdvisorPortraits()
	
	local player = Players[Game.GetActivePlayer()];
	local currentEra = player:GetCurrentEra();
	
	local Eras = {};
	for row in GameInfo.Eras() do
		Eras[row.Type] = row.ID;
	end
	
	for iAdvisor, portraits in pairs(AdvisorPortraits) do
	
		local bestPortrait = nil;
		local bestPortraitEra = -2;
	
		for eraType, portrait in pairs(AdvisorPortraits[iAdvisor]) do
		
			-- Hide portrait by default.
			portrait:SetHide(true);
			
			local era = Eras[eraType];
			local eraID = (era ~= nil) and era or -1;
			
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
end
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
	if uiMsg == KeyEvents.KeyDown then
		if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
			Close();
			return true;
		end
	end
end
ContextPtr:SetInputHandler( InputHandler );


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function ShowHideHandler( bIsHide, bInitState )

	if( not bInitState ) then
		if( not bIsHide ) then
			UI.incTurnTimerSemaphore();
			Events.SerialEventGameMessagePopupShown(m_PopupInfo);
			UpdateAdvisorPortraits();
		else
			UI.decTurnTimerSemaphore();
			Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_ADVISOR_INFO, 0);
		end
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(Close);
