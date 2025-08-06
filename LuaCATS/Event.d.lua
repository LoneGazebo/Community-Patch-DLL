--- @meta

--- @class Event
--- @field Add function

Events = {}

--- @class SerialEventGameMessagePopupProcessed: Event
--- @field Add fun(EventHandler: fun(strSearch: string))
--- @overload fun(strSearch: string)
Events.SearchForPediaEntry = {}

--- @class SerialEventGameMessagePopup: Event
--- @field Add fun(EventHandler: fun(popupInfo: PopupInfo))
--- @overload fun(popupInfo: PopupInfo)
Events.SerialEventGameMessagePopup = {}

--- @class SerialEventGameMessagePopupProcessed: Event
--- @field Add fun(EventHandler: fun(ePopup: ButtonPopupType, arg1: number))
--- @field CallImmediate fun(ePopup: ButtonPopupType, arg1: number)
--- @overload fun(ePopup: ButtonPopupType, arg1: number)
Events.SerialEventGameMessagePopupProcessed = {}

--- @class SerialEventGameMessagePopupShown: Event
--- @field Add fun(EventHandler: fun(popupInfo: PopupInfo))
--- @overload fun(popupInfo: PopupInfo)
Events.SerialEventGameMessagePopupShown = {}
