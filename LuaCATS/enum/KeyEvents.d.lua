--- @meta

--- Unlike most other enums, this one doesn't come from the game core DLL<br>
--- We'll have to trust modiki on the values of these
--- @enum KeyEventType
KeyEvents = {
	WM_KEYDOWN = 256,
	WM_KEYUP = 257,

	-- Likely aliases of the ones above; more commonly used
	KeyDown = 256,
	KeyUp = 257,
}
