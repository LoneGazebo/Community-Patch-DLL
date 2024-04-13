print("Loading SquadBases.lua")

-- third party
include ( "TableSaverLoader.lua" )

local squadBasesLut = {};

function GetSquadBasePlot(squadNumber)
    return squadBasesLut[squadNumber];
end 

function SetSquadBasePlot(squadNumber, newBasePlot)
    squadBasesLut[squadNumber] = newBasePlot;
end

function ResetSquadBasePlot(squadNumber)
    squadBasesLut[squadNumber] = nil
end

for i=1,10 do
    ResetSquadBasePlot(i);
end

----------------------------------------------------------------
-- Save Handler
----------------------------------------------------------------
function SquadNamesInptHdlr(uiMsg, wParam, lParam)

    if uiMsg == KeyEvents.KeyDown then

        if wParam == Keys.VK_F11 then
            TableSave(squadBasesLut, "upMainsquadBasesLutStore")
            print("Quicksaving...")
            UI.QuickSave()
            return true

        elseif wParam == Keys.S and UIManager:GetControl() then
            print("ctrl-s detected")
            PazyrykOnSaveClicked()
            return true

        elseif wParam == Keys.VK_RETURN then --> When "Return" is pressed check button text for "End Turn"
            local VupEndTurnButtonText = ContextPtr:LookUpControl("/InGame/WorldView/ActionInfoPanel/EndTurnText"):GetText()
            local VupNextTurnText = Locale.ConvertTextKey("TXT_KEY_NEXT_TURN")

            if (VupEndTurnButtonText == VupNextTurnText) then
                Game.CycleUnits(true)                     --> Cycle units so if one unit is selected it will be deselected
                gTupGlobalVariables.bIsEndTurn = true --> Change global variable
                FupOnEndTurnClicked()                     --> Call developer function
                return true
            end
            
            return true;

        elseif UI.CtrlKeyDown() and UI.ShiftKeyDown() then --> Show all units destination hotkey
            if not gTupGlobalVariables.bCTRLSHIFTPressed then
                gTupGlobalVariables.bCTRLSHIFTPressed = true
                FupShowDestinationsWithETA()
                return true;
            end
        end

    elseif (uiMsg == KeyEvents.KeyUp) then
        if gTupGlobalVariables.bCTRLSHIFTPressed and (wParam == Keys.VK_CONTROL or wParam == Keys.VK_SHIFT) then
            gTupGlobalVariables.bCTRLSHIFTPressed = false
            gTupGlobalVariables.tForCTRLSHIFT.tPlotsToHighlight.Destinations.FuchsiaPlots = {}
            gTupGlobalVariables.tForCTRLSHIFT.tPlotsToHighlight.Destinations.RedPlots = {}
            gTupGlobalVariables.tForCTRLSHIFT.tPlotsToHighlight.Units = {}
            gTupGlobalVariables.tForCTRLSHIFT.tXY = {}
            FupRemovePathGraphics()

            if (UI.GetSelectedUnitID() ~= nil) then --> If one unit have a stored table and is selected while CTRL or SHIFT is
                FupUnitPathViewer(1, 1, 1)               --  released its path will be shown
            end

            return true;
        end
    end
end
ContextPtr:SetInputHandler(SquadNamesInptHdlr)

-- Modified by Logharn to keep the standard behaviour for Save Games
function PazyrykOnSaveClicked()
    print("SaveGame clicked")
    TableSave(squadBasesLut, "upMainsquadBasesLutStore")
    UIManager:QueuePopup(ContextPtr:LookUpControl("/InGame/GameMenu/SaveMenu"), PopupPriority.SaveMenu)
end

function PazyrykOnQuickSaveClicked()
    print("QuickSaveGame clicked")
    TableSave(squadBasesLut, "upMainsquadBasesLutStore")
    UI.QuickSave()
end

function RegisterOnSaveCallback()
    local QuickSaveButton = ContextPtr:LookUpControl("/InGame/GameMenu/QuickSaveButton")
    local SaveCtrlButton = ContextPtr:LookUpControl("/InGame/GameMenu/SaveGameButton")
    local VupEndTurnButtonControl = ContextPtr:LookUpControl("/InGame/WorldView/ActionInfoPanel/EndTurnButton") --> Lookup the "End Turn Button"
    QuickSaveButton:RegisterCallback( Mouse.eLClick, PazyrykOnQuickSaveClicked )
    SaveCtrlButton:RegisterCallback( Mouse.eLClick, PazyrykOnSaveClicked )
    print ("SaveGame Buttons callbacks registered...")
end