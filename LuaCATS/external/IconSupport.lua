include( "FLuaVector" );

---------------------------------------------------------------------------------------------------------
-- Global Variables
---------------------------------------------------------------------------------------------------------
g_IconAtlases = nil;

---------------------------------------------------------------------------------------------------------
function PopulateIconAtlases()
	g_IconAtlases = {};
	
	for row in GameInfo.IconTextureAtlases() do
		local atlas = g_IconAtlases[row.Atlas];
		if(atlas == nil) then
			atlas = {};
			g_IconAtlases[row.Atlas] = atlas;
		end
		
		atlas[row.IconSize] = {
			row.Filename,
			row.IconsPerRow,
			row.IconsPerColumn
		};
	end	
end
---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------
function IconLookup( offset, iconSize, atlas )    

	if (offset < 0) then
		--print("--------------------IconLookup---------------------------")
		--print("offset < 0 or imageControl == nil")
		--print("offset: "..tostring(offset))
		--print("iconSize: "..tostring(iconSize))
		--print("atlas: "..tostring(atlas))
		--print("---------------------------------------------------------")
		--print("")
		return nil, nil;
	end	
	
	if(g_IconAtlases == nil) then
		PopulateIconAtlases();
	end
	
	local atlas = g_IconAtlases and g_IconAtlases[atlas];
	if(atlas ~= nil) then
		local entry = atlas[iconSize];
		if(entry ~= nil) then
		
			local filename = entry[1];
			local numRows = entry[3];
			local numCols = entry[2];
			
			if (filename == nil or offset > (numRows * numCols) - 1) then
				--print("--------------------IconLookup---------------------------")
				--print("filename == nil or offset > (numRows * numCols) - 1")
				--print("offset: "..tostring(offset))
				--print("iconSize: "..tostring(iconSize))
				--print("atlas: "..tostring(atlas))
				--print("imageControl: "..tostring(imageControl))
				--print("filename: "..tostring(filename))
				--print("numRows: "..tostring(numRows))
				--print("numCols: "..tostring(numCols))
				--print("---------------------------------------------------------")
				--print("")
				return;
			end
			
			return Vector2( (offset % numCols) * iconSize, math.floor(offset / numCols) * iconSize ), filename;			
		end
	end	
end


---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------
function IconHookup( offset, iconSize, atlas, imageControl, debugPrint )
   
	if offset < 0 or imageControl == nil then
		--if debugPrint then
			--print("--------------------IconHookup---------------------------")
			--print("offset < 0 or imageControl == nil")
			--print("offset: "..tostring(offset))
			--print("iconSize: "..tostring(iconSize))
			--print("atlas: "..tostring(atlas))
			--print("imageControl: "..tostring(imageControl))
			--print("---------------------------------------------------------")
			--print("")
		--end
		return nil;
	end	
	
	if(g_IconAtlases == nil) then
		PopulateIconAtlases();
	end
	
	local atlas = g_IconAtlases and g_IconAtlases[atlas];
	if(atlas ~= nil) then
		local entry = atlas[iconSize];
		if(entry ~= nil) then
		
			local filename = entry[1];
			local numRows = entry[3];
			local numCols = entry[2];
			
			if (filename == nil or offset > (numRows * numCols) - 1) then
				--if debugPrint then
					--print("--------------------IconHookup---------------------------")
					--print("filename == nil or offset > (numRows * numCols) - 1")
					--print("offset: "..tostring(offset))
					--print("iconSize: "..tostring(iconSize))
					--print("atlas: "..tostring(atlas))
					--print("imageControl: "..tostring(imageControl))
					--print("filename: "..tostring(filename))
					--print("numRows: "..tostring(numRows))
					--print("numCols: "..tostring(numCols))
					--print("---------------------------------------------------------")
					--print("")
				--end
				return;
			end
			
			imageControl:SetTexture(filename);
			imageControl:SetTextureOffsetVal( (offset % numCols) * iconSize, math.floor(offset / numCols) * iconSize );	
		end
	end	
		
	return true;
end

---------------------------------------------------------------------------------------------------------
-- Art Team Color Variables
---------------------------------------------------------------------------------------------------------
local teamSize80 = 64;
local teamSize64 = 48;
local teamSize45 = 32;
local teamSize32 = 24;
---------------------------------------------------------------------------------------------------------
-- This is a special case hookup for civilization icons that will take into account 
-- the fact that player colors are dynamically handed out
---------------------------------------------------------------------------------------------------------
function CivIconHookup( playerID, iconSize, iconControl, teamColorControl, shadowIconControl, alwaysUseComposite, shadowed, highlightControl )

    local thisCivType = PreGame.GetCivilization( playerID );
    local thisCiv = GameInfo.Civilizations[thisCivType];
    local thisPlayerColorIndex = nil;
    local defaultColorSet = nil;
    local nonDefaultColor = nil;
    
    if highlightControl ~= nil then
		highlightControl:SetHide( true );
	end
    
    if thisCiv then
		thisPlayerColorIndex = PreGame.GetCivilizationColor( playerID );
	    defaultColorSet = GameInfo.PlayerColors[thisCiv.DefaultPlayerColor];
		nonDefaultColor = ( defaultColorSet.ID ~= thisPlayerColorIndex );
		if playerID >= GameDefines.MAX_MAJOR_CIVS then
			nonDefaultColor = true;
		end
	end
	
	if  thisCiv and (alwaysUseComposite or nonDefaultColor) then
		if(iconSize == 80) then
			iconSize = teamSize80;
		elseif(iconSize == 64) then 
			iconSize = teamSize64;
		elseif(iconSize == 45) then
			iconSize = teamSize45;			
		elseif(iconSize == 32) then
			iconSize = teamSize32;
		end
		
		local textureOffset, textureAtlas = IconLookup( thisCiv.PortraitIndex, iconSize, thisCiv.AlphaIconAtlas );
		if textureOffset ~= nil then     
			iconControl:SetHide(false);  
			iconControl:SetTexture( textureAtlas );
			iconControl:SetTextureOffset( textureOffset );
	 		if shadowIconControl and shadowed then
				shadowIconControl:SetTexture( textureAtlas );
				shadowIconControl:SetTextureOffset( textureOffset );
	 		end
        end

		local thisPlayerColorSet;
		if thisPlayerColorIndex > -1  then
			thisPlayerColorSet = GameInfo.PlayerColors[thisPlayerColorIndex];
		else
			thisPlayerColorSet = defaultColorSet;
		end
		
		local primaryColor			= GameInfo.Colors[thisPlayerColorSet.PrimaryColor];
		local primaryColorVector	= Vector4( primaryColor.Red, primaryColor.Green, primaryColor.Blue, primaryColor.Alpha );
		local secondaryColor		= GameInfo.Colors[thisPlayerColorSet.SecondaryColor];
		local secondaryColorVector	= Vector4( secondaryColor.Red, secondaryColor.Green, secondaryColor.Blue, secondaryColor.Alpha );

        iconControl:SetColor( primaryColorVector );
			
	 	if teamColorControl then
	 		if(iconSize == teamSize80) then
	 			teamColorControl:SetTexture( "CivIconBGSizes.dds" );
				teamColorControl:SetTextureOffsetVal( 141, 0 );
	 		elseif(iconSize == teamSize64) then
	 			teamColorControl:SetTexture( "CivIconBGSizes.dds" );
				teamColorControl:SetTextureOffsetVal( 77, 0 );
			elseif(iconSize == teamSize45) then
				teamColorControl:SetTexture( "CivIconBGSizes.dds" );
				teamColorControl:SetTextureOffsetVal( 32, 0 );
			elseif(iconSize == teamSize32) then
				teamColorControl:SetTexture( "CivIconBGSizes.dds" );
				teamColorControl:SetTextureOffsetVal( 0, 0 );
			end

			if(thisCivType == GameInfo.Civilizations["CIVILIZATION_MINOR"].ID) then
				iconControl:SetColor( secondaryColorVector );
				teamColorControl:SetColor( primaryColorVector );
			else
				teamColorControl:SetColor( secondaryColorVector );
			end
	 		teamColorControl:SetHide( false );
	 	end
	 	if highlightControl ~= nil then
	 		if(iconSize == teamSize80) then
	 			highlightControl:SetTexture( "CivIconBGSizes_Highlight.dds" );
				highlightControl:SetTextureOffsetVal( 141, 0 );
	 		elseif(iconSize == teamSize64) then
	 			highlightControl:SetTexture( "CivIconBGSizes_Highlight.dds" );
				highlightControl:SetTextureOffsetVal( 77, 0 );
			elseif(iconSize == teamSize45) then
				highlightControl:SetTexture( "CivIconBGSizes_Highlight.dds" );
				highlightControl:SetTextureOffsetVal( 32, 0 );
			elseif(iconSize == teamSize32) then
				highlightControl:SetTexture( "CivIconBGSizes_Highlight.dds" );
				highlightControl:SetTextureOffsetVal( 0, 0 );
			end

			if(alwaysUseComposite or thisCivType == GameInfo.Civilizations["CIVILIZATION_MINOR"].ID) then
				highlightControl:SetHide( false );
			end
	 	end
	 	if shadowIconControl then
			if(shadowed) then
		 		shadowIconControl:SetHide( false );
		 	else
		 		shadowIconControl:SetHide( true );
		 	end
	 	end
	 	
	else
		if teamColorControl then
			if thisCiv then
				IconHookup( thisCiv.PortraitIndex, iconSize, thisCiv.IconAtlas, teamColorControl );
				local white = {x = 1, y = 1, z = 1, w = 1};
				teamColorControl:SetColor( white );
				iconControl:SetHide(true);
				if shadowIconControl then
					shadowIconControl:SetHide(true);
				end
			else
				IconHookup( 23, iconSize, "CIV_COLOR_ATLAS", teamColorControl );
				local white = {x = 1, y = 1, z = 1, w = 1};
				teamColorControl:SetColor( white );
				iconControl:SetHide(true);
				if shadowIconControl then
					shadowIconControl:SetHide(true);
				end	
			end
		else
			if thisCiv then
	 			IconHookup( thisCiv.PortraitIndex, iconSize, thisCiv.IconAtlas, iconControl );
	 		else
	 			IconHookup( 23, iconSize, "CIV_COLOR_ATLAS", iconControl );
	 			local white = {x = 1, y = 1, z = 1, w = 1};
				iconControl:SetColor( white );
	 		end
	 		if teamColorControl then
	 			teamColorControl:SetHide( true );
	 		end
	 		if shadowIconControl then
	 			shadowIconControl:SetHide( true );
	 		end
	 	end
	end
end


---------------------------------------------------------------------------------------------------------
-- This is a special case hookup for civilization icons that always uses the one-piece version
---------------------------------------------------------------------------------------------------------
function SimpleCivIconHookup( playerID, iconSize, iconControl )
    local thisCivType = PreGame.GetCivilization( playerID );
    local thisCiv = GameInfo.Civilizations[thisCivType];
    if thisCiv then
		return IconHookup( thisCiv.PortraitIndex, iconSize, thisCiv.IconAtlas, iconControl );
	else
		return IconHookup( 23, iconSize, "CIV_COLOR_ATLAS", iconControl );
	end
end

----------------------------------------------------------------------------------------------------------
-- These event listeners will invalidate the cache if mods are activated or deactivated.
----------------------------------------------------------------------------------------------------------
Events.AfterModsDeactivate.Add(function()
	g_IconAtlases = nil;
end);

Events.AfterModsActivate.Add(function()
	g_IconAtlases = nil;
end);