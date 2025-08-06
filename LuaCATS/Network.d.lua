--- @meta

--- Contains functions that send network messages to every player.<br>
--- Note that these are also used in single player.<br>
--- Each function should correspond to a function of the same name in `ICvDLLUtility.h`
--- @class Network
--- @field [string] function
Network = {}

--- See `ICvEngineUtility1::sendResearch` and `CvDllNetMessageHandler::ResponseResearch`
--- @param eTech TechType
--- @param iDiscover integer
--- @param ePlayerToStealFrom PlayerId
--- @param bShift boolean
function Network.SendResearch(eTech, iDiscover, ePlayerToStealFrom, bShift) end
