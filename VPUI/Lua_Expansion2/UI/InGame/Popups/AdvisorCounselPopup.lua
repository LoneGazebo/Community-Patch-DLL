if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- AdvisorInfo Popup
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );

local AdvisorCounselTable = nil;

local m_PopupInfo = nil;

local AdvisorCounselIndexTable = {};
AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_ECONOMIC] = 0;
AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_MILITARY] = 0;
AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_FOREIGN]  = 0;
AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_SCIENCE]  = 0;


local AdvisorPortraits = {
	[AdvisorTypes.ADVISOR_SCIENCE] = {
		["ERA_ANCIENT"] = Controls.ScienceAdvisorPortrait1,
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

----------------------------------------------------------------        
----------------------------------------------------------------      
function CanAdvisorCounselAdvance (Advisor)
	local iTableLength = 0;
	if (AdvisorCounselTable[Advisor]) then
		iTableLength = table.count(AdvisorCounselTable[Advisor]);
	end
	
	local iIndex = AdvisorCounselIndexTable[Advisor];
	if (iIndex + 1 < iTableLength) then
		return true;
	else
		return false;
	end
end

----------------------------------------------------------------        
----------------------------------------------------------------      
function CanAdvisorCounselReverse (Advisor)
	local iTableLength = 0;
	if (AdvisorCounselTable[Advisor]) then
		iTableLength = table.count(AdvisorCounselTable[Advisor]);
	end

	local iIndex = AdvisorCounselIndexTable[Advisor];
	if (iIndex - 1 >= 0) then
		return true;
	else
		return false;
	end
end


----------------------------------------------------------------        
----------------------------------------------------------------        
function UpdateNextPrevButtons ()
	Controls.EconomicNextButton:SetDisabled(not CanAdvisorCounselAdvance(AdvisorTypes.ADVISOR_ECONOMIC));
	Controls.MilitaryNextButton:SetDisabled(not CanAdvisorCounselAdvance(AdvisorTypes.ADVISOR_MILITARY));
	Controls.ForeignNextButton:SetDisabled (not CanAdvisorCounselAdvance(AdvisorTypes.ADVISOR_FOREIGN));
	Controls.ScienceNextButton:SetDisabled (not CanAdvisorCounselAdvance(AdvisorTypes.ADVISOR_SCIENCE));	 

	Controls.EconomicPrevButton:SetDisabled(not CanAdvisorCounselReverse(AdvisorTypes.ADVISOR_ECONOMIC));
	Controls.MilitaryPrevButton:SetDisabled(not CanAdvisorCounselReverse(AdvisorTypes.ADVISOR_MILITARY));
	Controls.ForeignPrevButton:SetDisabled (not CanAdvisorCounselReverse(AdvisorTypes.ADVISOR_FOREIGN));
	Controls.SciencePrevButton:SetDisabled (not CanAdvisorCounselReverse(AdvisorTypes.ADVISOR_SCIENCE));
end

----------------------------------------------------------------        
----------------------------------------------------------------
function UpdatePageDisplay (Control, Advisor)
	local iTableLength = 0;
	if (AdvisorCounselTable[Advisor]) then
		iTableLength = table.count(AdvisorCounselTable[Advisor]);
	end
	
	local iIndex = AdvisorCounselIndexTable[Advisor];
	
	if (iTableLength == 0) then
		Control:SetText("");
	else
		Control:SetHide(false);
		Control:SetText(Locale.ConvertTextKey("TXT_KEY_ADVISOR_COUNSEL_PAGE_DISPLAY", iIndex + 1, iTableLength));
	end
end

----------------------------------------------------------------        
----------------------------------------------------------------
function UpdatePageCounts ()
	UpdatePageDisplay(Controls.EconomicPagesLabel, AdvisorTypes.ADVISOR_ECONOMIC);
	UpdatePageDisplay(Controls.MilitaryPagesLabel, AdvisorTypes.ADVISOR_MILITARY);
	UpdatePageDisplay(Controls.ForeignPagesLabel, AdvisorTypes.ADVISOR_FOREIGN);
	UpdatePageDisplay(Controls.SciencePagesLabel, AdvisorTypes.ADVISOR_SCIENCE);
end

----------------------------------------------------------------        
----------------------------------------------------------------        
function SetEconomicAdvisorText (strText)
	Controls.EACounselText:SetText(strText);
	Controls.EA_ScrollPanel:CalculateInternalSize();
end

----------------------------------------------------------------        
----------------------------------------------------------------        
function SetMilitaryAdvisorText (strText)
	Controls.MACounselText:SetText(strText);
	Controls.MA_ScrollPanel:CalculateInternalSize();
end

----------------------------------------------------------------        
----------------------------------------------------------------        
function SetForeignAdvisorText (strText)
	Controls.FACounselText:SetText(strText);
	Controls.FA_ScrollPanel:CalculateInternalSize();
end

----------------------------------------------------------------        
----------------------------------------------------------------        
function SetScienceAdvisorText (strText)
	Controls.SACounselText:SetText(strText);
	Controls.SA_ScrollPanel:CalculateInternalSize();
end

----------------------------------------------------------------        
----------------------------------------------------------------        
function ShowAdvisorText (Advisor, Index)
	local str = "";
	AdvisorCounselIndexTable[Advisor] = Index;
	if (AdvisorCounselTable[Advisor]) then
		if (AdvisorCounselTable[Advisor][Index]) then
			str = AdvisorCounselTable[Advisor][Index];
		end
	end

	if (Advisor == AdvisorTypes.ADVISOR_ECONOMIC) then
		SetEconomicAdvisorText(str);
	elseif (Advisor == AdvisorTypes.ADVISOR_MILITARY) then
		SetMilitaryAdvisorText(str);
	elseif (Advisor == AdvisorTypes.ADVISOR_FOREIGN) then
		SetForeignAdvisorText(str);
	elseif (Advisor == AdvisorTypes.ADVISOR_SCIENCE) then
		SetScienceAdvisorText(str);
	end

	
	UpdateNextPrevButtons();
	UpdatePageCounts();
end

----------------------------------------------------------------        
----------------------------------------------------------------        
function UpdateAdvisorSlots ()
	AdvisorCounselTable = Game.GetAdvisorCounsel();
	AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_ECONOMIC] = 0;
	AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_MILITARY] = 0;
	AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_FOREIGN]  = 0;
	AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_SCIENCE]  = 0;
	
	ShowAdvisorText(AdvisorTypes.ADVISOR_ECONOMIC, AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_ECONOMIC]);
	ShowAdvisorText(AdvisorTypes.ADVISOR_MILITARY, AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_MILITARY]);
	ShowAdvisorText(AdvisorTypes.ADVISOR_FOREIGN,  AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_FOREIGN]);
	ShowAdvisorText(AdvisorTypes.ADVISOR_SCIENCE,  AdvisorCounselIndexTable[AdvisorTypes.ADVISOR_SCIENCE]);
end

-------------------------------------------------
-- On Display
-------------------------------------------------
function OnPopup( popupInfo )
	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_ADVISOR_COUNSEL ) then
		return;
	end
	
	m_PopupInfo = popupInfo;
	CivIconHookup( Game.GetActivePlayer(), 64, Controls.Icon, Controls.CivIconBG, Controls.CivIconShadow, false, true );
	UpdateAdvisorSlots();
		
	if( m_PopupInfo.Data1 == 1 ) then
    	if( ContextPtr:IsHidden() == false ) then
    	    Close();
    	else
        	UIManager:QueuePopup( ContextPtr, PopupPriority.InGameUtmost );
    	end
	else
    	UIManager:QueuePopup( ContextPtr, PopupPriority.AdvisorCounsel );
	end
end
Events.SerialEventGameMessagePopup.Add( OnPopup );


----------------------------------------------------------------        
----------------------------------------------------------------        
function OnAdvisorNext (Advisor)
	if (CanAdvisorCounselAdvance(Advisor)) then
		ShowAdvisorText(Advisor, AdvisorCounselIndexTable[Advisor] + 1);	
	end
end
Controls.EconomicNextButton:RegisterCallback( Mouse.eLClick, function() OnAdvisorNext(AdvisorTypes.ADVISOR_ECONOMIC) end);
Controls.MilitaryNextButton:RegisterCallback( Mouse.eLClick, function() OnAdvisorNext(AdvisorTypes.ADVISOR_MILITARY) end);
Controls.ForeignNextButton:RegisterCallback( Mouse.eLClick,  function() OnAdvisorNext(AdvisorTypes.ADVISOR_FOREIGN)  end);
Controls.ScienceNextButton:RegisterCallback( Mouse.eLClick,  function() OnAdvisorNext(AdvisorTypes.ADVISOR_SCIENCE)  end);

----------------------------------------------------------------        
----------------------------------------------------------------        
function OnAdvisorPrev (Advisor)
	if (CanAdvisorCounselReverse(Advisor)) then
		ShowAdvisorText(Advisor, AdvisorCounselIndexTable[Advisor] - 1);
	end
end
Controls.EconomicPrevButton:RegisterCallback( Mouse.eLClick, function() OnAdvisorPrev(AdvisorTypes.ADVISOR_ECONOMIC) end);
Controls.MilitaryPrevButton:RegisterCallback( Mouse.eLClick, function() OnAdvisorPrev(AdvisorTypes.ADVISOR_MILITARY) end);
Controls.ForeignPrevButton:RegisterCallback( Mouse.eLClick,  function() OnAdvisorPrev(AdvisorTypes.ADVISOR_FOREIGN)  end);
Controls.SciencePrevButton:RegisterCallback( Mouse.eLClick,  function() OnAdvisorPrev(AdvisorTypes.ADVISOR_SCIENCE)  end);

----------------------------------------------------------------        
----------------------------------------------------------------        
function Close ()
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
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            Close();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );

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
		
		for eraType, portrait in pairs(portraits) do
			
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
function ShowHideHandler( bIsHide, bInitState )

    if( not bInitState ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        	UpdateAdvisorPortraits();
        	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
        else
            UI.decTurnTimerSemaphore();
            Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_ADVISOR_COUNSEL, 0);
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(Close);
