local pairs = pairs
local table_insert = table.insert
local table_remove = table.remove
local ContextPtr = ContextPtr
local Mouse_eLClick = Mouse.eLClick

EUI = EUI or (MapModData and MapModData.UI_bc1)
if not EUI then
	local c= ContextPtr:LookUpControl("..")
	if c:GetID() == "InGame" then
		c:LoadNewContext( "EUI_context" )
		EUI = MapModData.UI_bc1
	else
		include( "EUI_core_library" )
	end
end

function SimpleInstanceManager( i_name, i_root, i_parentControl, i_hiddenControl )
	local i_pending = {}
	local i_displayed = {}
	local i_heap = {}
	local self = {
		ResetInstances = function()
			i_pending = i_displayed
			i_displayed = {}
		end,
		Commit = function()
			for instance in pairs( i_pending ) do
				instance[i_root]:ChangeParent( i_hiddenControl )
				table_insert( i_heap, instance )
			end
			i_pending = {}
		end,
		GetInstance = function()
			local instance = table_remove( i_pending )
			if not instance then
				instance = table_remove( i_heap )
				if instance then
					instance[i_root]:ChangeParent( i_parentControl )
				else
					instance = {}
					ContextPtr:BuildInstanceForControl( i_name, instance, i_parentControl )
				end
			end
			table_insert( i_displayed, instance )
		end,
	}
	return self
end

function StackInstanceManager( instanceName, rootControlName, parentControl, collapseControl, collapseFunction, collapseControlAlwaysShown, isCollapsed )
	local n = 0
	local m = 0
	local self
	local function Collapse( value )
		if value ~= nil then
			isCollapsed = value
			parentControl:SetHide( value )
			collapseControl:SetText( value and "[ICON_PLUS]" or "[ICON_MINUS]" )
			if collapseFunction then
				collapseFunction( self, isCollapsed )
			end
		end
		return isCollapsed
	end
	self = {
		Count = function()
			return n
		end,
		ResetInstances = function()
			n = 0
		end,
		GetInstance = function()
			n = n + 1
			if n > #self then
				local instance = {}
				ContextPtr:BuildInstanceForControl( instanceName, instance, parentControl )
				--else ContextPtr:BuildInstance( instanceName, instance )
				self[n] = instance
				return instance, true
			else
				return self[n]
			end
		end,
		Commit = function()
			for i = m + 1, n do
				self[i][rootControlName]:SetHide(false)
			end
			for i = n + 1, m do
				self[i][rootControlName]:SetHide(true)
			end
			m = n
			if collapseControl then
				parentControl:CalculateSize()
				if not collapseControlAlwaysShown then
					collapseControl:SetHide( n <= 0 )
				end
			end
		end,
	}
	if collapseControl then
		self.Collapse = Collapse
		collapseControl:GetTextControl():SetAnchor( "L,C" )
		collapseControl:GetTextControl():SetOffsetVal( 12, -2 )
		collapseControl:RegisterCallback( Mouse_eLClick,
		function()
			Collapse( not isCollapsed )
		end)
		Collapse( isCollapsed or false )
	end
	return self
end
