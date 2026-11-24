-- Map Seed row of Advanced Setup: empty gives a new map each game, a number makes the same map again.
-- Stored as GAMEOPTION_MAP_SEED, which the Community Patch DLL reads in place of the clock seed.

-- A LuaContext is as tall as the screen; shrink it to the row so the options stack around it closes up
ContextPtr:SetSizeX(450);
ContextPtr:SetSizeY(32);

-- Game options hold signed 32-bit values; seeds above 2^31 are stored negative
local function GetStoredSeed()
	local seed = PreGame.GetGameOption("GAMEOPTION_MAP_SEED") or 0;
	if seed < 0 then
		seed = seed + 4294967296;
	end
	return seed;
end

-- The edit box sizes itself to its text and sits centred in the frame, like a pulldown value
local function FitEdit()
	local text = Controls.MapSeedEdit:GetText() or "";
	Controls.MapSeedRandom:SetHide(text ~= "");
	if text == "" then
		-- the cursor waits just left of the "Random" placeholder
		Controls.MapSeedEdit:SetSizeX(16);
		Controls.MapSeedEdit:SetOffsetX(-math.floor(Controls.MapSeedRandom:GetSizeX() / 2) - 10);
	else
		-- 16 spare pixels so a typed digit never scrolls the text before the next resize
		Controls.MapSeedMeasure:SetText(text);
		Controls.MapSeedEdit:SetSizeX(Controls.MapSeedMeasure:GetSizeX() + 16);
		Controls.MapSeedEdit:SetOffsetX(8);
	end
	Controls.MapSeedFrame:ReprocessAnchoring();
end

local function ShowStoredSeed()
	local seed = GetStoredSeed();
	Controls.MapSeedEdit:SetText(seed ~= 0 and string.format("%.0f", seed) or "");
	FitEdit();
end

Controls.MapSeedEdit:RegisterCallback(function()
	local seed = tonumber(Controls.MapSeedEdit:GetText()) or 0;
	if seed > 4294967295 then
		seed = 4294967295;
		Controls.MapSeedEdit:SetText("4294967295");
	end
	FitEdit();
	if seed > 2147483647 then
		seed = seed - 4294967296;
	end
	PreGame.SetGameOption("GAMEOPTION_MAP_SEED", seed);
end);

-- a click anywhere in the frame types into the seed, not only on the narrow edit box
Controls.MapSeedFocus:RegisterCallback(Mouse.eLClick, function()
	Controls.MapSeedEdit:TakeFocus();
end);

-- Reset and the other setup screens change game options without telling this context, so follow the stored seed
ContextPtr:SetUpdate(function()
	if GetStoredSeed() ~= (tonumber(Controls.MapSeedEdit:GetText()) or 0) then
		ShowStoredSeed();
	end
end);

ContextPtr:SetShowHideHandler(function(bIsHide)
	if not bIsHide then
		-- only the Community Patch database has the option
		local bSupported = GameInfo.GameOptions.GAMEOPTION_MAP_SEED ~= nil;
		Controls.MapSeedRow:SetHide(not bSupported);
		if bSupported then
			ShowStoredSeed();
		end
	end
end);
