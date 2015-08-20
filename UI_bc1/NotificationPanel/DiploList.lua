--bc1 jump straight to the diplo screen

local isHidden = ContextPtr:IsHidden()
ContextPtr:SetShowHideHandler(
function( isHide, isInit )
	if isHidden ~= isHide then
		isHidden = isHide
		if not isInit and not isHide then
			ContextPtr:SetHide( true )
			Events.SerialEventGameMessagePopup({Type = ButtonPopupTypes.BUTTONPOPUP_DIPLOMATIC_OVERVIEW});
		end
	end
end)