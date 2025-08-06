--- @meta

--- @class InstanceManager
InstanceManager = {}

--- @class Instance
--- @field [string] Control

--- Constructor
--- @param strInstanceName string
--- @param strRootControlId string
--- @param ParentControl Control
--- @return InstanceManager
function InstanceManager:new(strInstanceName, strRootControlId, ParentControl) end

--- Get an instance from the available list, or create a new one if there isn't any available
--- @return Instance
function InstanceManager:GetInstance() end

--- Move all the instances back to the available list and hide the root control
function InstanceManager:ResetInstances() end
