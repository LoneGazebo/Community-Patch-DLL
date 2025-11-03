--- @meta

--- @class Event
--- @field Add function

Events = {}

--- @class SearchForPediaEntry: Event
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

--- @class SerialEventCityCaptured: Event
--- @field Add fun(EventHandler: fun(vHexPos: Vector2, ePlayer: PlayerId, eCity: CityId))
Events.SerialEventCityCaptured = {}

--- @class SerialEventCityDestroyed: Event
--- @field Add fun(EventHandler: fun(vHexPos: Vector2, ePlayer: PlayerId, eCity: CityId))
Events.SerialEventCityDestroyed = {}

--- @class OpenInfoCorner: Event
--- @field Add fun(EventHandler: fun(eInfoCorner: InfoCornerId))
--- @overload fun(eInfoCorner: InfoCornerId)
Events.OpenInfoCorner = {}

--- @class AIProcessingStartedForPlayer: Event
--- @field Add fun(EventHandler: fun(ePlayer: PlayerId))
Events.AIProcessingStartedForPlayer = {}

--- @class GameplaySetActivePlayer: Event
--- @field Add fun(EventHandler: fun(eActivePlayer: PlayerId, ePrevActivePlayer: PlayerId))
Events.GameplaySetActivePlayer = {}

--- @class AudioPlay2DSound: Event
--- @overload fun(str2DSound: string)
Events.AudioPlay2DSound = {}
