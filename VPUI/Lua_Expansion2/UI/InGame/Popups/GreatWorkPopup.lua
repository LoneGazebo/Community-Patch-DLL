if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Wonder popup
-------------------------------------------------

include( "IconSupport" );
include( "InfoTooltipInclude" );
include( "CommonBehaviors" );

local iconTextureOffsets = 
{
	GREAT_WORK_ART = {u = 128 , v = 0 },
	GREAT_WORK_MUSIC = {u = 0 ,v = 128 },
	GREAT_WORK_LITERATURE = {u = 128 ,v = 128 },
}

local g_PopupInfo = nil;

local gwsWidth, gwsHeight = Controls.GreatWorkSplash:GetSizeVal();
local windowWidth, windowHeight = Controls.Window:GetSizeVal();


local deltaWidth = windowWidth - gwsWidth;
local deltaHeight = windowHeight - gwsHeight;

-------------------------------------------------
-- On Popup
-------------------------------------------------
function OnPopup( popupInfo )

	if( popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER ) then
		return;
	end
	
	g_PopupInfo = popupInfo;
		  
	-- If the priority is PopupPriority.Default, then use our default, else just pass it through
	local iPriority = popupInfo.Priority;
	if (iPriority == PopupPriority.Default) then 
		iPriority = PopupPriority.GreatWorkPopup;
	end
	
	UIManager:QueuePopup( ContextPtr, iPriority );
	
end
Events.SerialEventGameMessagePopup.Add( OnPopup );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnClose()
    UIManager:DequeuePopup( ContextPtr );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnClose);


-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnClose();
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
        	Events.SerialEventGameMessagePopupShown(g_PopupInfo);
        	
			local iGWIndex;
			local eGWType;
			local greatWork;

			iGWIndex = g_PopupInfo.Data1;
			
			if (iGWIndex ~= -1) then
				eGWType = Game.GetGreatWorkType(iGWIndex);
			else
				eGWType = g_PopupInfo.Data2;
			end
			
			greatWork = GameInfo.GreatWorks[eGWType];
		        
			local greatWorkImage = greatWork.Image;
			if(greatWorkImage == nil) then
    			greatWorkImage = "GW_Starry_Night.dds"
			end
			
			local textureOffset = iconTextureOffsets[greatWork.GreatWorkClassType];

			if(textureOffset ~= nil) then
				Controls.TopIcon:SetTextureOffsetVal(textureOffset.u, textureOffset.v);
			end
			
			Controls.GreatWorkSplash:SetTextureAndResize(greatWorkImage);
			
			m_fOriginalSizeX, m_fOriginalSizeY = Controls.GreatWorkSplash:GetSizeVal();
		  	
			Controls.Window:SetSizeVal(m_fOriginalSizeX + deltaWidth, m_fOriginalSizeY + deltaHeight);
			Controls.GreatWorkSplashClip:SetSizeVal(m_fOriginalSizeX, m_fOriginalSizeY);


			g_AnimUpdate = ZoomOutEffect{
				SplashControl = Controls.GreatWorkSplash, 
				ScaleFactor = 0.5,
				AnimSeconds = 5
			};
			
			ContextPtr:SetUpdate(function(dTime)
				if(g_AnimUpdate == nil or g_AnimUpdate(dTime) == true) then
					ContextPtr:ClearUpdate();
				end
			end);
			
			local lcMaxWidth = m_fOriginalSizeX + deltaWidth - 34;
			Controls.LowerCaption:SetWrapWidth(lcMaxWidth);
			    
			if (not greatWork.ArchaeologyOnly) then
   				Controls.Title:LocalizeAndSetText(Game.GetGreatWorkArtist(iGWIndex));
   			else
   				Controls.Title:LocalizeAndSetText("TXT_KEY_GREAT_WORK_POPUP_WRITTEN_ARTIFACT");
   			end
   			Controls.LowerCaption:LocalizeAndSetText(greatWork.Description);
		   	
   			local lcWidth, lcHeight = Controls.LowerCaption:GetSizeVal();
			local lbcWidth, lcbHeight = Controls.LowerCaptionFrame:GetSizeVal();

			local lcNewWidth = math.min(lcWidth + 100, lcMaxWidth);
			local lcNewHeight = lcHeight + 20;
	
			print("FLIBBY");			
			print(lcNewWidth);
			print(m_fOriginalSizeX + deltaWidth - 34);
			
			
			
			
			--if(lcNewWidth >= lcMaxWidth) then
				--lcNewWidth = lcMaxWidth;
				--Controls.LowerCaption:SetWrapWidth(lcMaxWidth);
				--local lcWidth, lcHeight = Controls.LowerCaption:GetSizeVal();
			--else
			--
			--end
		
			Controls.LowerCaptionFrame:SetSizeVal(lcNewWidth, lcNewHeight);
			Controls.LowerCaptionFrame:SetDisabled(true);

			Controls.LowerCaptionFrame:ReprocessAnchoring();
			
			Controls.GreatWorkSplashClip:ReprocessAnchoring();
			
			Controls.GreatWorkSplashContainer:SetSizeVal(m_fOriginalSizeX, m_fOriginalSizeY);
			Controls.GreatWorkSplashContainer:ReprocessAnchoring();
				
   			if(greatWork.Quote ~= nil) then
   				Controls.Quote:LocalizeAndSetText(greatWork.Quote);
				Controls.Quote:SetHide(false);
   			else
   				Controls.Quote:SetHide(true);
   			end        	
        else
            UI.decTurnTimerSemaphore();
            if(g_PopupInfo ~= nil) then
				Events.SerialEventGameMessagePopupProcessed.CallImmediate(g_PopupInfo.Type, 0);
            end
            
            ContextPtr:ClearUpdate();
			Controls.GreatWorkSplash:UnloadTexture();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
Events.GameplaySetActivePlayer.Add(OnClose); 
