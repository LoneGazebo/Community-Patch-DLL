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

--- See `ICvEngineUtility1::SendFoundPantheon` and `CvDllNetMessageHandler::ResponseFoundPantheon`<br>
--- Despite the name, this network message is used for both pantheon and reformation<br>
--- This CAN fail for various reasons, in that case the player will be notified
--- @param ePlayer PlayerId
--- @param eBelief BeliefType # cannot be NO_BELIEF/-1
function Network.SendFoundPantheon(ePlayer, eBelief) end

--- See `ICvEngineUtility1::SendFoundReligion` and `CvDllNetMessageHandler::ResponseFoundReligion`<br>
--- This CAN fail (e.g. if someone else happened to pick the same beliefs at the same time), in that case the player will be prompted to found a religion again
--- @param ePlayer PlayerId
--- @param eReligion ReligionType
--- @param strCustomName string? # custom name of the religion, overriding the default
--- @param eBelief1 BeliefType # cannot be NO_BELIEF/-1
--- @param eBelief2 BeliefType # cannot be NO_BELIEF/-1
--- @param eBelief3 BeliefType
--- @param eBelief4 BeliefType
--- @param iHolyCityX integer # x-coordinate of holy city for notification
--- @param iHolyCityY integer # y-coordinate of holy city for notification
function Network.SendFoundReligion(ePlayer, eReligion, strCustomName, eBelief1, eBelief2, eBelief3, eBelief4, iHolyCityX, iHolyCityY) end

--- See `ICvEngineUtility1::SendEnhanceReligion` and `CvDllNetMessageHandler::ResponseEnhanceReligion`<br>
--- This CAN fail (e.g. if someone else happened to pick the same beliefs at the same time), in that case the player will be prompted to enhance their religion again
--- @param ePlayer PlayerId
--- @param eReligion ReligionType
--- @param strCustomName string? # UNUSED
--- @param eBelief1 BeliefType # cannot be NO_BELIEF/-1
--- @param eBelief2 BeliefType
--- @param iHolyCityX integer # x-coordinate of holy city for notification
--- @param iHolyCityY integer # y-coordinate of holy city for notification
function Network.SendEnhanceReligion(ePlayer, eReligion, strCustomName, eBelief1, eBelief2, iHolyCityX, iHolyCityY) end
