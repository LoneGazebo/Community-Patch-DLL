if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Select Map Type
-------------------------------------------------
include("IconSupport");
include("InstanceManager");

g_InstanceManager = InstanceManager:new( "ItemInstance", "Root", Controls.Stack );

---------------------------------------------------------------- 
function IsWBMap(file)
	return Path.UsesExtension(file,".Civ5Map"); 
end

-------------------------------------------------
-------------------------------------------------
function OnBack()
    ContextPtr:SetHide( true );
    ContextPtr:LookUpControl( ".." ):SetHide( false );
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnBack );


----------------------------------------------------------------        
-- Input processing
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
    if uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE then
            OnBack();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


function OnMultiSizeMapSelected(mapEntry)
	if(mapEntry == nil) then
		PreGame.SetRandomMapScript(true);
	else
		PreGame.SetRandomMapScript(false);
		
		local mapSizes = {};
		
		for row in GameInfo.Map_Sizes{MapType = mapEntry.Type} do
			local world = GameInfo.Worlds[row.WorldSizeType];
			
			mapSizes[world.ID] = row.FileName; 
		end
		
		local worldSize = PreGame.GetWorldSize();
		--print(worldSize);
		
		if(mapSizes[worldSize] ~= nil) then
		
		--print("FOUND");
			local world = GameInfo.Worlds[worldSize];
			
			--Adjust Map Size
			PreGame.SetMapScript(mapSizes[worldSize]);
			PreGame.SetRandomWorldSize( false );
			PreGame.SetWorldSize( worldSize );
			PreGame.SetNumMinorCivs( world.DefaultMinorCivs );	
		else
		
			-- print("NOT FOUND");
			-- Not Found, pick random size and set filename to smallest world size.
			for row in GameInfo.Worlds("ID >= 0 ORDER BY ID") do
				local file = mapSizes[row.ID];
				if(file ~= nil) then
				
					PreGame.SetRandomWorldSize( false );
					PreGame.SetMapScript(file);
					PreGame.SetWorldSize( row.ID );
					PreGame.SetNumMinorCivs( row.DefaultMinorCivs );	
					break;
				end
			end
		end
	end
	
	OnBack();
end

function OnMapScriptSelected(script)
	if(script == nil) then
		PreGame.SetRandomMapScript(true);
	else
		PreGame.SetRandomMapScript(false);
		
		local mapScript = "";
		if(type(script) == "string") then
			mapScript = script;
		else
			mapScript = script.FileName;
		end
		
		PreGame.SetMapScript(mapScript);
		
		if(IsWBMap(mapScript)) then
		
			local mapInfo = UI.GetMapPreview(mapScript);
			if(mapInfo ~= nil) then		
				local world = GameInfo.Worlds[mapInfo.MapSize];
			
				--Adjust Map Size
				PreGame.SetRandomWorldSize( false );
				PreGame.SetWorldSize( mapInfo.MapSize );
				PreGame.SetNumMinorCivs( world.DefaultMinorCivs );	
				
			end
		end
	end
	
	OnBack();
end

function ShowHideHandler( bIsHide )
    if( not bIsHide ) then
			
        Controls.ScrollPanel:SetScrollValue( 0 );
        
        Refresh();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

function Refresh()
	
	-- Build Folder Hierarchy
	local folders = {};
	local rootFolder = {
		Items = {},
		Icon = {0, 64, "WORLDTYPE_ATLAS"},
	};
	
	-- Add base folder items (these will NOT be sorted).
	table.insert(rootFolder.Items, {
		Name = (not PreGame.IsMultiplayerGame()) and Locale.Lookup("TXT_KEY_RANDOM_MAP_SCRIPT") or Locale.Lookup("TXT_KEY_ANY_MAP_SCRIPT"),
		Description = (not PreGame.IsMultiplayerGame()) and Locale.Lookup("TXT_KEY_RANDOM_MAP_SCRIPT_HELP") or Locale.Lookup("TXT_KEY_ANY_MAP_SCRIPT_HELP"),
		Icon = {4, 64, "WORLDTYPE_ATLAS"},
		Callback = function() OnMapScriptSelected(); end,
	});

	function AddMapScriptByFileName(fileName)
		for row in GameInfo.MapScripts{FileName = fileName, SupportsSinglePlayer = 1, Hidden = 0} do
			table.insert(rootFolder.Items, {
				Name = Locale.ConvertTextKey(row.Name),
				Description = Locale.ConvertTextKey(row.Description or ""),
				Icon = {row.IconIndex or 0, 64, row.IconAtlas},
				Callback = function() OnMapScriptSelected(row.FileName) end,
			});	
		end
	end	

	AddMapScriptByFileName("Assets\\Maps\\Continents.lua");
	AddMapScriptByFileName("Assets\\Maps\\Pangaea.lua");
	AddMapScriptByFileName("Assets\\Maps\\Archipelago.lua");
	AddMapScriptByFileName("Assets\\Maps\\Fractal.lua");
		
	for folder in GameInfo.Map_Folders() do
	
		local folder = {
			Type = folder.Type,
			ParentType = folder.ParentType,
			Name = Locale.Lookup(folder.Title),
			Description = Locale.Lookup(folder.Description),
			--Icon = {folder.IconIndex, 64, folder.IconAtlas},
			Items = {},	
		}
		
		folder.Callback = function() View(folder); end
		folders[folder.Type] = folder;
	end
		
	local additionalMapsFolder = folders["MAP_FOLDER_ADDITIONAL"];
	
	for row in GameInfo.Maps() do
		local folder = rootFolder;
		if(row.FolderType ~= nil) then
			folder = folders[row.FolderType];
		end
		
		table.insert(folder.Items, {
			Name = Locale.Lookup(row.Name),
			Description = Locale.Lookup(row.Description),
			Callback = function() OnMultiSizeMapSelected(row); end,
			Icon = {row.IconIndex, 64, row.IconAtlas},
		});
	end
	
	local mapsToFilter = {};
	for row in GameInfo.Map_Sizes() do
		table.insert(mapsToFilter, Path.GetFileName(row.FileName));
	end
	
	local maps = Modding.GetMapFiles();
	for _, map in ipairs(maps) do
	
		local bFilter = false;
		for i, mapFile in ipairs(mapsToFilter) do
		
			if(Path.GetFileName(map.File) == mapFile) then
				bFilter = true;
				break;
			end
		end
		
		if(not bFilter) then
			
			local mapData = UI.GetMapPreview(map.File);
			
			local name, description;
			local isError = nil;
			
			if(mapData) then
				if(not Locale.IsNilOrWhitespace(map.Name)) then
					name = map.Name;
				elseif(not Locale.IsNilOrWhitespace(mapData.Name)) then
					name = Locale.Lookup(mapData.Name);
				else
					name = Path.GetFileNameWithoutExtension(map.File);
				end
				
				if(not Locale.IsNilOrWhitespace(map.Description)) then
					description = map.Description;
				else
					description = Locale.Lookup(mapData.Description);
				end
				
				--print(name);
			else
				name = Path.GetFileNameWithoutExtension(map.File);
				
				local nameTranslation = Locale.ConvertTextKey("TXT_KEY_INVALID_MAP_TITLE", name);
				if(nameTranslation and nameTranslation ~= "TXT_KEY_INVALID_MAP_TITLE") then
					name = nameTranslation;
				else
					name = "[COLOR_RED]" .. name .. "[ENDCOLOR]";
				end
				
				local descTranslation = Locale.ConvertTextKey("TXT_KEY_INVALID_MAP_DESC");
				if(descTranslation and descTranslation ~= "TXT_KEY_INVALID_MAP_DESC") then
					description = descTranslation;
				end
				
				isError = true;
			end
			
			local entryCallback = nil;
			if(not isError) then
				entryCallback = function() OnMapScriptSelected(map.File) end;
			end
			
			table.insert(additionalMapsFolder.Items, {
				Name = name,
				Description = description,
				Callback = entryCallback,
				Icon = {4, 64, "WORLDTYPE_ATLAS"},
				
			});				
		end
	end
	
	--Add Map Scripts!
	-- Filter out these since we already added them explicitly.
	local mapScriptsToFilter = {
		"Continents.lua",
		"Pangaea.lua",
		"Archipelago.lua",
		"Fractal.lua"
	};
	
	for row in GameInfo.MapScripts{SupportsSinglePlayer = 1, Hidden = 0} do
		
		local bFilter = false;
		for i, mapFile in ipairs(mapsToFilter) do
			if(Path.GetFileName(row.FileName) == mapFile) then
				bFilter = true;
				break;
			end
		end
	
		if(not bFilter) then
			local folder = (row.FolderType ~= nil) and folders[row.FolderType] or nil;
			folder = (folder) and folder or additionalMapsFolder;
			
			table.insert(folder.Items, {
				Name = Locale.Lookup(row.Name),
				Description = Locale.ConvertTextKey(row.Description or ""),
				Icon = {row.IconIndex or 0, 64, row.IconAtlas},
				Callback = function() OnMapScriptSelected(row.FileName) end,
			});
		end
	end
		
	local sorted_root_folders = {};
	for _ , folder in pairs(folders) do
		if (folder.ParentType ~= nil) then
			local parentFolder = folders[folder.ParentType];
			folder.ParentFolder = parentFolder;
			table.insert(folders[folder.ParentType].Items, folder);
		else
			folder.ParentFolder = rootFolder;
			table.insert(sorted_root_folders, folder);
		end
	end
	
	table.sort(sorted_root_folders, function(a, b)
		return Locale.Compare(a.Name, b.Name) == -1;
	end);
	
	for i,v in ipairs(sorted_root_folders) do
		table.insert(rootFolder.Items, v);
	end
	
	-- SORT (ignore root folder)
	for k, v in pairs(folders) do
		if(v.Items and #v.Items > 0) then
			table.sort(v.Items, function(a, b)
				if(a.Items ~= nil and b.Items == nil) then
					return true;
				else
					if(a.Items == nil and b.Items ~= nil) then
						return false;
					end
					
					return Locale.Compare(a.Name, b.Name) == -1;
				end
			end);
		end
	end
	
	View(rootFolder);
end

function View(folder)

	g_InstanceManager:ResetInstances();
	
	
	if(folder.ParentFolder ~= nil and #folder.Items > 8) then
		local item = g_InstanceManager:GetInstance();
		item.Icon:SetHide(true);
		item.Name:SetOffsetVal(10,0);
		item.Name:LocalizeAndSetText("[ICON_ARROW_LEFT] {TXT_KEY_SELECT_MAP_TYPE_BACK}");
		item.Button:LocalizeAndSetToolTip("TXT_KEY_SELECT_MAP_TYPE_BACK_HELP");
		item.Button:RegisterCallback(Mouse.eLClick, function() View(folder.ParentFolder); end);
	end
		
	for i, v in ipairs(folder.Items) do
		
		--Hide empty folders.
		if(v.Items == nil or #v.Items > 0) then
			local item = g_InstanceManager:GetInstance();
			
			
			if(v.Items and #v.Items >0) then
				item.Name:SetText(v.Name .. " [ICON_ARROW_RIGHT]");
			else
				item.Name:SetText(v.Name);
			end
			
			
			if(v.Icon ~= nil) then
				IconHookup(v.Icon[1], v.Icon[2], v.Icon[3], item.Icon);
				item.Icon:SetHide(false);
				item.Name:SetOffsetVal(64,0);
			else
				item.Icon:SetHide(true);
				item.Name:SetOffsetVal(10,0);
			end
			
			item.Button:SetToolTipString(v.Description);
			
			if(v.Callback ~= nil) then
				item.Button:RegisterCallback(Mouse.eLClick, v.Callback);
			end
		end
	end
	
	if(folder.ParentFolder ~= nil) then
		local item = g_InstanceManager:GetInstance();
		item.Icon:SetHide(true);
		item.Name:SetOffsetVal(10,0);
		item.Name:LocalizeAndSetText("[ICON_ARROW_LEFT] {TXT_KEY_SELECT_MAP_TYPE_BACK}");
		item.Button:LocalizeAndSetToolTip("TXT_KEY_SELECT_MAP_TYPE_BACK_HELP");
		item.Button:RegisterCallback(Mouse.eLClick, function() View(folder.ParentFolder); end);
	end

	Controls.Stack:ReprocessAnchoring();
	Controls.Stack:CalculateSize();

	Controls.ScrollPanel:CalculateInternalSize();
	Controls.ScrollPanel:SetScrollValue(0);

end

Refresh();
