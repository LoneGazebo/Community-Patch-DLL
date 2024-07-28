print('This is SquadsOptions.lua')

local SquadsOptions = Modding.OpenUserData( "Squads Options", 1 );

-- Defaults
local HighlightSquadUnits = true;
local ShowSquadNumberUnderFlag = true;
local SquadsEndMovementType = 0;

-- UI handlers
function OpenSquadsOptions()
    UpdateOptionsPanel();
    Controls.OptionsPanel:SetHide( false );
end

function CloseSquadsOptions()
    Controls.OptionsPanel:SetHide( true );
end

function ToggleSquadsOptions()
    if Controls.OptionsPanel:IsHidden() then
        OpenSquadsOptions()
    else
        CloseSquadsOptions()
    end
end

function UpdateOptionsPanel()

end

function HandleSquadsOptionsButton()
    ToggleSquadsOptions()
end
Controls.SquadsOptionsButton:RegisterCallback(Mouse.eLClick, HandleSquadsOptionsButton);

function HandleHighlightSquadUnitsCheckbox()
    HighlightSquadUnits = not HighlightSquadUnits;
    LuaEvents.SQUADS_OPTIONS_CHANGED("HighlightSquadUnits", HighlightSquadUnits)
end
Controls.HighlightSquadUnitsCheckBox:RegisterCheckHandler(HandleHighlightSquadUnitsCheckbox);

function HandleFlagsNumberCheckBox()
    ShowSquadNumberUnderFlag = not ShowSquadNumberUnderFlag;
    LuaEvents.SQUADS_OPTIONS_CHANGED("ShowSquadNumberUnderFlag", ShowSquadNumberUnderFlag)
end
Controls.FlagsNumberCheckBox:RegisterCheckHandler(HandleFlagsNumberCheckBox);

function HandleAlertOnArrivalRadioButton()
    if SquadsEndMovementType ~= 0 then
        SquadsEndMovementType = 0;
        LuaEvents.SQUADS_OPTIONS_CHANGED("SquadsEndMovementType", SquadsEndMovementType);
    end
end
Controls.AlertOnArrivalRadioButton:RegisterCheckHandler(HandleAlertOnArrivalRadioButton);

function HandleWakeOnUnitArrivalRadioButton()
    if SquadsEndMovementType ~= 1 then
        SquadsEndMovementType = 1;
        LuaEvents.SQUADS_OPTIONS_CHANGED("SquadsEndMovementType", SquadsEndMovementType);
    end
end
Controls.WakeOnUnitArrivalRadioButton:RegisterCheckHandler(HandleWakeOnUnitArrivalRadioButton);

function HandleWakeOnSquadArrivalRadioButton()
    if SquadsEndMovementType ~= 2 then
        SquadsEndMovementType = 2;
        LuaEvents.SQUADS_OPTIONS_CHANGED("SquadsEndMovementType", SquadsEndMovementType);
    end
end
Controls.WakeOnSquadArrivalRadioButton:RegisterCheckHandler(HandleWakeOnSquadArrivalRadioButton);

-- Options Logic
function SquadsOptionChanged(optionKey, newValue)
    SquadsOptions.SetValue(optionKey, newValue);
end
LuaEvents.SQUADS_OPTIONS_CHANGED.Add(SquadsOptionChanged);

-- Load Saved Options
local _hsu = SquadsOptions.GetValue("HighlightSquadUnits");
print("loaded the following value for HighlightSquadUnits", _hsu)
if _hsu ~= nil then
    HighlightSquadUnits = _hsu == 1;
end
LuaEvents.SQUADS_OPTIONS_CHANGED("HighlightSquadUnits", HighlightSquadUnits)
Controls.HighlightSquadUnitsCheckBox:SetCheck(HighlightSquadUnits);

local _ssnuf = SquadsOptions.GetValue("ShowSquadNumberUnderFlag");
if _ssnuf ~= nil then
    ShowSquadNumberUnderFlag = _ssnuf == 1;
end
LuaEvents.SQUADS_OPTIONS_CHANGED("ShowSquadNumberUnderFlag", ShowSquadNumberUnderFlag)
Controls.FlagsNumberCheckBox:SetCheck(ShowSquadNumberUnderFlag);

local _semt = SquadsOptions.GetValue("SquadsEndMovementType");
if _semt ~= nil then
    SquadsEndMovementType = _semt;
end
LuaEvents.SQUADS_OPTIONS_CHANGED("SquadsEndMovementType", SquadsEndMovementType)
print("loaded the following value for SquadsEndMovementType: ", SquadsEndMovementType);
if SquadsEndMovementType == 0 then
    Controls.AlertOnArrivalRadioButton:SetCheck(true);
elseif SquadsEndMovementType == 1 then
    Controls.WakeOnUnitArrivalRadioButton:SetCheck(true);
elseif SquadsEndMovementType == 2 then
    Controls.WakeOnSquadArrivalRadioButton:SetCheck(true);
end

print('Loaded SquadsOptions.lua')