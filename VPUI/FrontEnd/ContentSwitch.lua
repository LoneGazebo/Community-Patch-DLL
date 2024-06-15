-- Table of Vox Populi tips
-- Update this when adding new tips
local tips_count = 33;

-- Localize stuff for performance reason
local L = Locale.ConvertTextKey;
local math_random = math.random;
local math_randomseed = math.randomseed;
local os_time = os.time;


local g_TipType = {
	"TIP"--, "QUOTE", "FUNFACT", "HISTORY"
}

local g_ColorVPTip = {
	TIP = "[COLOR:198:136:50:255]"--, QUOTE = "[COLOR_MAGENTA]", FUNFACT = "[COLOR_YELLOW]", HISTORY = "[COLOR_CYAN]"
};

function GetVPTip()
	math_randomseed(os_time()); -- random initialize
	math_random(); math_random(); math_random(); -- warming up

	local iTipType = math_random(#g_TipType)
	return g_TipType[iTipType]
end

function VPTipTitle()
	return g_ColorVPTip[GetVPTip()] .. L('TXT_KEY_VPUI_TITLE_' .. GetVPTip()) .. '[ENDCOLOR]'
end

function VPTipText()
	local iTipText = 0
	if		GetVPTip() == "TIP" then
		iTipText = math_random(1, tips_count)
--[[
	elseif	GetVPTip() == "QUOTE" then
		iTipText = math_random(1, 4)
	elseif	GetVPTip() == "FUNFACT" then
		iTipText = math_random(1, 8)
	elseif	GetVPTip() == "HISTORY" then
		iTipText = math_random(1, 16)
--]]
	end
	return g_ColorVPTip[GetVPTip()] .. L('TXT_KEY_VPUI_' .. GetVPTip() .. '_' .. iTipText) .. '[ENDCOLOR]'
end

-------------------------------------------------
-- ContentSwitch
-------------------------------------------------

----------------------------------------------------------------
----------------------------------------------------------------
function OnShowHide( isHide, isInit )

	if(not isHide) then

	end

	Controls.Title:SetText(VPTipTitle()) -- Vox Populi tip title
	Controls.Tips:SetText(VPTipText()) -- Vox Populi tip content
	Controls.ContentSwitchGrid:SetSizeY( 290 ) -- Reset the size every time this UI appears
	local contentsize = Controls.Tips:GetSize().y
	if contentsize > 32 then -- This is the Tips box Y size, if the Tips box Y size is more than 32 pixel
		Controls.ContentSwitchGrid:SetSizeY( 290 + contentsize ) -- Resize the ContentSwitchGrid Y size with the Tips box Y size
	end
end
ContextPtr:SetShowHideHandler( OnShowHide );
