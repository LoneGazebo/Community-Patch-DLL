include( "EUI_tooltips" )
for k in pairs(EUI.InfoTooltipInclude) do
	loadstring(k.."=EUI."..k)()
end
