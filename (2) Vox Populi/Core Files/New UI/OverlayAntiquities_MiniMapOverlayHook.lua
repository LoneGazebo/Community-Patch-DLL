function OnMiniMapOverlayNextPreviousHook(bNext)
  -- print("Antiquities_OnMiniMapOverlayNextPreviousHook()")

  if (bNext) then
    LuaEvents.AntiquitiesDisplayNext()
  else
    LuaEvents.AntiquitiesDisplayPrevious()
  end
end

function OnMiniMapOverlayHook(bShow)
  -- print("Antiquities_OnMiniMapOverlayHook()")

  LuaEvents.AntiquitiesDisplay(bShow) 
end

LuaEvents.MiniMapOverlayAddin(
  {
    text="TXT_KEY_ANTIQUITY_OVERLAY_ANTIQUITIES", 
    group="other", 
    call=OnMiniMapOverlayHook, 
    nav=OnMiniMapOverlayNextPreviousHook,
    next_tt="TXT_KEY_NEXT_ANTIQUITY_TT",
    prev_tt="TXT_KEY_PREVIOUS_ANTIQUITY_TT"
  }
) 
