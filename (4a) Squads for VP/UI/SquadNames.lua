-- third party
include ( "TableSaverLoader.lua" )

print("Assigning default names");
squadNamesLut = {};


for i=1,10 do
    squadNamesLut[i] = string.format("Squad %d", i);
end

function GetSquadName(squadNumber)
    return squadNamesLut[squadNumber];
end 

function SetSquadName(squadNumber, newSquadName)
    squadNamesLut[squadNumber] = newSquadName;
end

-- save/load
function OnEndActivePlayerTurn()
    TableSave(squadNamesLut, "upMainsquadNamesLutStore")
end
Events.ActivePlayerTurnEnd.Add(OnEndActivePlayerTurn)

-- <-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><->
-- |                 TABLE SAVER FUNCTIONS by PAZYRYK                  |
-- <-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><->
-- Functions modified a bit by Black.Cobra a.k.a. Logharn
function PazyrykOnEnterGame()
    local DBQuery = Modding.OpenSaveData().Query
    print ("Player entering game ...")
    RegisterOnSaveCallback()
    -- we need to know if this is after a load from a saved game; use the presence of MyMod_Info table to do this
    local bNewGame = true
    for row in DBQuery("SELECT name FROM sqlite_master WHERE name='upMainsquadNamesLutStore_Info'") do       --change 'MyMod' to prefix used in TableSaverLoader.lua
        if row.name then bNewGame = false end
    end
    if bNewGame then
        print("new game, saving table")
        TableSave(squadNamesLut, "upMainsquadNamesLutStore")     --here to make sure save happens before any attempts to load
    else
        print("not new game. loading table")
        TableLoad(squadNamesLut, "upMainsquadNamesLutStore")
    end

end
Events.LoadScreenClose.Add(PazyrykOnEnterGame)

----------------------------------------------------------------
-- Save Handler
----------------------------------------------------------------
function SquadNamesInptHdlr(uiMsg, wParam, lParam)

    if uiMsg == KeyEvents.KeyDown then

        if wParam == Keys.VK_F11 then
            TableSave(squadNamesLut, "upMainsquadNamesLutStore")
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
    TableSave(squadNamesLut, "upMainsquadNamesLutStore")
    UIManager:QueuePopup(ContextPtr:LookUpControl("/InGame/GameMenu/SaveMenu"), PopupPriority.SaveMenu)
end

function PazyrykOnQuickSaveClicked()
    print("QuickSaveGame clicked")
    TableSave(squadNamesLut, "upMainsquadNamesLutStore")
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