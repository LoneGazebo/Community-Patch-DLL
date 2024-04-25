local SquadsOptions = Modding.OpenUserData( "Squads Options", 1 );

-- Defaults
local HighlightSquadUnits = true;
local ShowSquadNumberUnderFlag = true;
local SquadsEndMovementType = 0;

-- UI handlers
function OpenSquadsOptions()
    UpdateOptionsPanel();
    Controls.OptionsPanel:SetHide( false );
    -- Controls.SideStack:CalculateSize();
    -- Controls.SideStack:ReprocessAnchoring();
end

function CloseSquadsOptions()
    Controls.OptionsPanel:SetHide( true );
    -- Controls.SideStack:CalculateSize();
    -- Controls.SideStack:ReprocessAnchoring();
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
    print("Squads Options Button Pressed");
    ToggleSquadsOptions()
end
Controls.SquadsOptionsButton:RegisterCallback(Mouse.eLClick, HandleSquadsOptionsButton);

function HandleHighlightSquadUnitsCheckbox()
    print("HandleHighlightSquadUnitsCheckbox")
    HighlightSquadUnits = not HighlightSquadUnits;
    LuaEvents.SQUADS_OPTIONS_CHANGED("HighlightSquadUnits", HighlightSquadUnits)
end
Controls.HighlightSquadUnitsCheckBox:RegisterCheckHandler(HandleHighlightSquadUnitsCheckbox);

function HandleFlagsNumberCheckBox()
    print("HandleFlagsNumberCheckBox")
    ShowSquadNumberUnderFlag = not ShowSquadNumberUnderFlag;
    LuaEvents.SQUADS_OPTIONS_CHANGED("ShowSquadNumberUnderFlag", ShowSquadNumberUnderFlag)
end
Controls.FlagsNumberCheckBox:RegisterCheckHandler(HandleFlagsNumberCheckBox);

function HandleAlertOnArrivalRadioButton()
    print("HandleAlertOnArrivalRadioButton")
    if SquadsEndMovementType ~= 0 then
        SquadsEndMovementType = 0;
        LuaEvents.SQUADS_OPTIONS_CHANGED("SquadsEndMovementType", SquadsEndMovementType);
    end
end
Controls.AlertOnArrivalRadioButton:RegisterCheckHandler(HandleAlertOnArrivalRadioButton);

function HandleWakeOnUnitArrivalRadioButton()
    print("HandleWakeOnUnitArrivalRadioButton")
    if SquadsEndMovementType ~= 1 then
        SquadsEndMovementType = 1;
        LuaEvents.SQUADS_OPTIONS_CHANGED("SquadsEndMovementType", SquadsEndMovementType);
    end
end
Controls.WakeOnUnitArrivalRadioButton:RegisterCheckHandler(HandleWakeOnUnitArrivalRadioButton);

function HandleWakeOnSquadArrivalRadioButton()
    print("HandleWakeOnSquadArrivalRadioButton")
    if SquadsEndMovementType ~= 2 then
        SquadsEndMovementType = 2;
        LuaEvents.SQUADS_OPTIONS_CHANGED("SquadsEndMovementType", SquadsEndMovementType);
    end
end
Controls.WakeOnSquadArrivalRadioButton:RegisterCheckHandler(HandleWakeOnSquadArrivalRadioButton);

-- Options Logic
function SquadsOptionChanged(optionKey, newValue)
    if optionKey == "HighlightSquadUnits" then
        Controls.HighlightSquadUnitsCheckBox:SetCheck(newValue);
        print("Setting highlight squad units checkbox to ", newValue);
    elseif optionKey == "ShowSquadNumberUnderFlag" then
        Controls.FlagsNumberCheckBox:SetCheck(newValue);
        print("Setting FlagsNumberCheckBox checkbox to ", newValue);
    elseif optionKey == "SquadsEndMovementType" then
        print("Setting SquadsEndMovementType to ", newValue);
        if newValue == 0 then
            Controls.AlertOnArrivalRadioButton:SetCheck(true);
        elseif newValue == 1 then
            Controls.WakeOnUnitArrivalRadioButton:SetCheck(true);
        elseif newValue == 2 then
            Controls.WakeOnSquadArrivalRadioButton:SetCheck(true);
        end
    end
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

local _ssnuf = SquadsOptions.GetValue("ShowSquadNumberUnderFlag");
if _ssnuf ~= nil then
    ShowSquadNumberUnderFlag = _ssnuf == 1;
end
LuaEvents.SQUADS_OPTIONS_CHANGED("ShowSquadNumberUnderFlag", ShowSquadNumberUnderFlag)

local _semt = SquadsOptions.GetValue("SquadsEndMovementType");
if _semt ~= nil then
    SquadsEndMovementType = _semt;
end
LuaEvents.SQUADS_OPTIONS_CHANGED("SquadsEndMovementType", SquadsEndMovementType)