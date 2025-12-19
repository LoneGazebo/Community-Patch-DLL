if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- ContentSwitch
-------------------------------------------------

----------------------------------------------------------------  
----------------------------------------------------------------        
function OnShowHide( isHide, isInit )
	
	if(not isHide) then
		
	end	
end
ContextPtr:SetShowHideHandler( OnShowHide );
