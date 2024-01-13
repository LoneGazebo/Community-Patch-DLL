-------------------------------------------------
-- Tech Tree Popup
-------------------------------------------------
include( "InstanceManager" );

g_UseSmallIcons = true;

include( "TechButtonInclude" );
include( "TechHelpInclude" );

local m_PopupInfo = nil;
local stealingTechTargetPlayerID = -1;

local g_PipeManager = InstanceManager:new( "TechPipeInstance", "TechPipeIcon", Controls.TechTreeScrollPanel );
local g_EraManager = InstanceManager:new( "EraBlockInstance", "EraBlock", Controls.EraStack );
local g_TechInstanceManager = InstanceManager:new( "TechButtonInstance", "TechButton", Controls.TechTreeScrollPanel );

local g_NeedsFullRefreshOnOpen = false;
local g_NeedsFullRefresh = false;

local maxSmallButtons = 5;

-- I'll need these before I'm done
local bEspionageViewMode = false;
local playerID = Game.GetActivePlayer();	
local player = Players[playerID];
local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
local activeTeamID = Game.GetActiveTeam();
local activeTeam = Teams[activeTeamID];

-- textures I'll be using
local right1Texture = "TechBranchH.dds"
local right2Texture = "TechBranches.dds"
local right2BTexture = "TechBranches.dds"
local right2TTexture = "TechBranches.dds"
local right3Texture = "TechBranches.dds"

local left1Texture = "TechBranchH.dds"
local left2Texture = "TechBranches.dds"
local left2TTexture = "TechBranches.dds"
local left2BTexture = "TechBranches.dds"

local topRightTexture = "TechBranches.dds"
local topLeftTexture = "TechBranches.dds"
local bottomRightTexture = "TechBranches.dds"
local bottomLeftTexture = "TechBranches.dds"

local hTexture = "TechBranchH.dds";
local vTexture = "TechBranchV.dds";

local connectorSize = { x = 32; y = 42; };

local pinkColor = {x = 2, y = 0, z = 2, w = 1};

local blockSpacingX = 270 + 96;
local blockSizeX = 270;
local blockSpacingY = 68;
local extraYOffset = 32;


local maxTechNameLength = 22 - Locale.Length(Locale.Lookup("TXT_KEY_TURNS"));

-------------------------------------------------
-- Do initial setup stuff here
-------------------------------------------------

local techButtons = {};
local eraBlocks = {};
local eraColumns = {};

function InitialSetup()

	-- make the scroll bar the correct size for the display size
	Controls.TechTreeScrollBar:SetSizeX( Controls.TechTreeScrollPanel:GetSize().x - 150 );
	
	-- gather info about this player's unique units and buldings
	GatherInfoAboutUniqueStuff( civType );

	-- add the Era panels to the background
	AddEraPanels();

	-- add the pipes
	local techPipes = {};
	for row in GameInfo.Technologies() do
		techPipes[row.Type] = 
		{
			leftConnectionUp = false;
			leftConnectionDown = false;
			leftConnectionCenter = false;
			leftConnectionType = 0;
			rightConnectionUp = false;
			rightConnectionDown = false;
			rightConnectionCenter = false;
			rightConnectionType = 0;
			xOffset = 0;
			techType = row.Type;
		};
	end
	
	local cnxCenter = 1
	local cnxUp = 2
	local cnxDown = 4
	
	-- Figure out which left and right adapters we need
	for row in GameInfo.Technology_PrereqTechs() do
		local prereq = GameInfo.Technologies[row.PrereqTech];
		local tech = GameInfo.Technologies[row.TechType];
		if tech and prereq then
			if tech.GridY < prereq.GridY then
				techPipes[tech.Type].leftConnectionDown = true;
				techPipes[prereq.Type].rightConnectionUp = true;
			elseif tech.GridY > prereq.GridY then
				techPipes[tech.Type].leftConnectionUp = true;
				techPipes[prereq.Type].rightConnectionDown = true;
			else -- tech.GridY == prereq.GridY
				techPipes[tech.Type].leftConnectionCenter = true;
				techPipes[prereq.Type].rightConnectionCenter = true;
			end
			
			local xOffset = (tech.GridX - prereq.GridX) - 1;
			if xOffset > techPipes[prereq.Type].xOffset then
				techPipes[prereq.Type].xOffset = xOffset;
			end
		end
	end

	for pipeIndex, thisPipe in pairs(techPipes) do
		if thisPipe.leftConnectionDown then
			thisPipe.leftConnectionType = thisPipe.leftConnectionType + cnxDown;
		end 
		if thisPipe.leftConnectionUp then
			thisPipe.leftConnectionType = thisPipe.leftConnectionType + cnxUp;
		end 
		if thisPipe.leftConnectionCenter then
			thisPipe.leftConnectionType = thisPipe.leftConnectionType + cnxCenter;
		end 
		if thisPipe.rightConnectionDown then
			thisPipe.rightConnectionType = thisPipe.rightConnectionType + cnxDown;
		end 
		if thisPipe.rightConnectionUp then
			thisPipe.rightConnectionType = thisPipe.rightConnectionType + cnxUp;
		end 
		if thisPipe.rightConnectionCenter then
			thisPipe.rightConnectionType = thisPipe.rightConnectionType + cnxCenter;
		end 
	end

	for row in GameInfo.Technology_PrereqTechs() do
		local prereq = GameInfo.Technologies[row.PrereqTech];
		local tech = GameInfo.Technologies[row.TechType];
		if tech and prereq then
		
			if tech.GridX - prereq.GridX > 1 then
				local hConnection = g_PipeManager:GetInstance();
				hConnection.TechPipeIcon:SetOffsetVal(prereq.GridX*blockSpacingX + blockSizeX + 96, (prereq.GridY-5)*blockSpacingY + 12 + extraYOffset);
				hConnection.TechPipeIcon:SetTexture(hTexture);
				local size = { x = (tech.GridX-prereq.GridX - 1)*blockSpacingX + 4; y = 42; };
				hConnection.TechPipeIcon:SetSize(size);
			end
			
			if tech.GridY - prereq.GridY == 1 or tech.GridY - prereq.GridY == -1 then
				local vConnection = g_PipeManager:GetInstance();
				vConnection.TechPipeIcon:SetOffsetVal((tech.GridX-1)*blockSpacingX + blockSizeX + 96, ((tech.GridY-5)*blockSpacingY) - (((tech.GridY-prereq.GridY) * blockSpacingY) / 2) + extraYOffset);
				vConnection.TechPipeIcon:SetTexture(vTexture);
				local size = { x = 32; y = (blockSpacingY / 2) + 8; };
				vConnection.TechPipeIcon:SetSize(size);
			end

			if tech.GridY - prereq.GridY == 2 or tech.GridY - prereq.GridY == -2 then
				local vConnection = g_PipeManager:GetInstance();
				vConnection.TechPipeIcon:SetOffsetVal((tech.GridX-1)*blockSpacingX + blockSizeX + 96, (tech.GridY-5)*blockSpacingY - (((tech.GridY-prereq.GridY) * blockSpacingY) / 2) + extraYOffset);
				vConnection.TechPipeIcon:SetTexture(vTexture);
				local size = { x = 32; y = (blockSpacingY * 3 / 2) + 8; };
				vConnection.TechPipeIcon:SetSize(size);
			end
			
			if tech.GridY - prereq.GridY == 3 or tech.GridY - prereq.GridY == -3 then
				local vConnection = g_PipeManager:GetInstance();
				vConnection.TechPipeIcon:SetOffsetVal((tech.GridX-1)*blockSpacingX + blockSizeX + 96, ((tech.GridY-5)*blockSpacingY) - (((tech.GridY-prereq.GridY) * blockSpacingY) / 2) + extraYOffset);
				vConnection.TechPipeIcon:SetTexture(vTexture);
				local size = { x = 32; y = blockSpacingY * 2 + 20; };
				vConnection.TechPipeIcon:SetSize(size);
			end
			
			if tech.GridY - prereq.GridY == 4 or tech.GridY - prereq.GridY == -4 then
				local vConnection = g_PipeManager:GetInstance();
				vConnection.TechPipeIcon:SetOffsetVal((tech.GridX-1)*blockSpacingX + blockSizeX + 96, ((tech.GridY-5)*blockSpacingY) - (((tech.GridY-prereq.GridY) * blockSpacingY) / 2) + extraYOffset);
				vConnection.TechPipeIcon:SetTexture(vTexture);
				local size = { x = 32; y = blockSpacingY * 3 + 8; };
				vConnection.TechPipeIcon:SetSize(size);
			end
		
		end
	end

	for pipeIndex, thisPipe in pairs(techPipes) do
	
		local tech = GameInfo.Technologies[thisPipe.techType];
		
		local yOffset = (tech.GridY-5)*blockSpacingY + 12 + extraYOffset;
		
		if thisPipe.rightConnectionType >= 1 then
			
			local startPipe = g_PipeManager:GetInstance();
			startPipe.TechPipeIcon:SetOffsetVal( tech.GridX*blockSpacingX + blockSizeX + 64, yOffset );
			startPipe.TechPipeIcon:SetTexture(right1Texture);
			startPipe.TechPipeIcon:SetSize(connectorSize);
			
			local pipe = g_PipeManager:GetInstance();			
			if thisPipe.rightConnectionType == 1 then
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX + blockSizeX + 96, yOffset );
				pipe.TechPipeIcon:SetTexture(right1Texture);
			elseif thisPipe.rightConnectionType == 2 then
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX+thisPipe.xOffset)*blockSpacingX + blockSizeX + 96 - 12, yOffset - 15 );
				pipe.TechPipeIcon:SetTexture(bottomRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72,72));
			elseif thisPipe.rightConnectionType == 3 then
				--pipe.TechPipeIcon:SetOffsetVal( (tech.GridX+thisPipe.xOffset)*blockSpacingX + blockSizeX + 96 - 6, yOffset - 15 );
				--pipe.TechPipeIcon:SetTexture(right2BTexture);
				--pipe.TechPipeIcon:SetTextureOffset(Vector2(36,72));
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX + blockSizeX + 96, yOffset );
				pipe.TechPipeIcon:SetTexture(right1Texture);
				pipe = g_PipeManager:GetInstance();			
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX+thisPipe.xOffset)*blockSpacingX + blockSizeX + 96 - 12, yOffset - 15 );
				pipe.TechPipeIcon:SetTexture(bottomRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72,72));
			elseif thisPipe.rightConnectionType == 4 then
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX+thisPipe.xOffset)*blockSpacingX + blockSizeX + 96 - 12, yOffset );
				pipe.TechPipeIcon:SetTexture(topRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72,0));
			elseif thisPipe.rightConnectionType == 5 then
				--pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX + blockSizeX + 96 - 6, yOffset );
				--pipe.TechPipeIcon:SetTexture(right2TTexture);
				--pipe.TechPipeIcon:SetTextureOffset(Vector2(36,0));
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX + blockSizeX + 96, yOffset );
				pipe.TechPipeIcon:SetTexture(right1Texture);
				pipe = g_PipeManager:GetInstance();			
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX + blockSizeX + 96 - 12, yOffset );
				pipe.TechPipeIcon:SetTexture(topRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72,0));
			elseif thisPipe.rightConnectionType == 6 then
				--pipe.TechPipeIcon:SetOffsetVal( (tech.GridX+thisPipe.xOffset)*blockSpacingX + blockSizeX + 96 - 12, yOffset - 6 );
				--pipe.TechPipeIcon:SetTexture(right2Texture);
				--pipe.TechPipeIcon:SetTextureOffset(Vector2(72,36));
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX + blockSizeX + 96 - 12, yOffset );
				pipe.TechPipeIcon:SetTexture(topRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72,0));
				pipe = g_PipeManager:GetInstance();			
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX+thisPipe.xOffset)*blockSpacingX + blockSizeX + 96 - 12, yOffset - 15 );
				pipe.TechPipeIcon:SetTexture(bottomRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72,72));
			else-- thisPipe.rightConnectionType == 7 then
				--pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX + blockSizeX + 96 - 6, yOffset - 6 );
				--pipe.TechPipeIcon:SetTexture(right3Texture);
				--pipe.TechPipeIcon:SetTextureOffset(Vector2(36,36));
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX + blockSizeX + 96, yOffset );
				pipe.TechPipeIcon:SetTexture(right1Texture);
				pipe = g_PipeManager:GetInstance();			
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX + blockSizeX + 96 - 12, yOffset );
				pipe.TechPipeIcon:SetTexture(topRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72,0));
				pipe = g_PipeManager:GetInstance();			
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX+thisPipe.xOffset)*blockSpacingX + blockSizeX + 96 - 12, yOffset - 15 );
				pipe.TechPipeIcon:SetTexture(bottomRightTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(72,72));
			end
		end
		
		if thisPipe.leftConnectionType >= 1 then
			
			local startPipe = g_PipeManager:GetInstance();
			startPipe.TechPipeIcon:SetOffsetVal( tech.GridX*blockSpacingX + 26, yOffset );
			startPipe.TechPipeIcon:SetTexture(left1Texture);
			startPipe.TechPipeIcon:SetSize(	Vector2(40, 42) );
			
			local pipe = g_PipeManager:GetInstance();			
			if thisPipe.leftConnectionType == 1 then
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset );
				pipe.TechPipeIcon:SetTexture(left1Texture);
			elseif thisPipe.leftConnectionType == 2 then
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0,72));
			elseif thisPipe.leftConnectionType == 3 then
				--pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX - 6, yOffset - 15 );
				--pipe.TechPipeIcon:SetTexture(left2BTexture);
				--pipe.TechPipeIcon:SetTextureOffset(Vector2(36,72));
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset );
				pipe.TechPipeIcon:SetTexture(left1Texture);
				pipe = g_PipeManager:GetInstance();	
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0,72));
			elseif thisPipe.leftConnectionType == 4 then
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset );
				pipe.TechPipeIcon:SetTexture(topLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0,0));
			elseif thisPipe.leftConnectionType == 5 then
				--pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX - 6, yOffset );
				--pipe.TechPipeIcon:SetTexture(left2TTexture);
				--pipe.TechPipeIcon:SetTextureOffset(Vector2(36,0));
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset );
				pipe.TechPipeIcon:SetTexture(left1Texture);
				pipe = g_PipeManager:GetInstance();	
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset );
				pipe.TechPipeIcon:SetTexture(topLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0,0));
			elseif thisPipe.leftConnectionType == 6 then
				--pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset - 6 );
				--pipe.TechPipeIcon:SetTexture(left2Texture);
				--pipe.TechPipeIcon:SetTextureOffset(Vector2(0,36));
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset );
				pipe.TechPipeIcon:SetTexture(topLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0,0));
				pipe = g_PipeManager:GetInstance();	
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0,72));
			else-- thisPipe.rightConnectionType == 7 then
				--pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX - 6, yOffset - 6 );
				--pipe.TechPipeIcon:SetTexture(left2Texture);
				--pipe.TechPipeIcon:SetTextureOffset(Vector2(36,36));
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset );
				pipe.TechPipeIcon:SetTexture(left1Texture);
				pipe = g_PipeManager:GetInstance();	
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset );
				pipe.TechPipeIcon:SetTexture(topLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0,0));
				pipe = g_PipeManager:GetInstance();	
				pipe.TechPipeIcon:SetOffsetVal( (tech.GridX)*blockSpacingX, yOffset - 15);
				pipe.TechPipeIcon:SetTexture(bottomLeftTexture);
				pipe.TechPipeIcon:SetTextureOffset(Vector2(0,72));
			end
		end
			
	end

	techPediaSearchStrings = {};

	-- add the instances of the tech panels
	for tech in GameInfo.Technologies() do
		AddTechButton( tech );
	end

	-- resize the panel to fit the contents
	Controls.EraStack:CalculateSize();
	Controls.EraStack:ReprocessAnchoring();
    Controls.TechTreeScrollPanel:CalculateInternalSize();
    
    --initialized = true;		
end

function AddEraPanels()
	-- find the range of columns that each era takes
	for tech in GameInfo.Technologies() do
		local eraID = GameInfo.Eras[tech.Era].ID;
		if not eraColumns[eraID] then
			eraColumns[eraID] = { minGridX = tech.GridX; maxGridX = tech.GridX; researched = false; };
		else
			if tech.GridX < eraColumns[eraID].minGridX then
				eraColumns[eraID].minGridX = tech.GridX;
			end
			if tech.GridX > eraColumns[eraID].maxGridX then
				eraColumns[eraID].maxGridX = tech.GridX;
			end
		end
	end

	-- add the era panels
	for era in GameInfo.Eras() do
	
		local thisEraBlockInstance = g_EraManager:GetInstance();
		-- store this panel off for later
		eraBlocks[era.ID] = thisEraBlockInstance;
		
		-- add the correct text for this era panel
		local textString = era.Description;
		local localizedLabel = Locale.ConvertTextKey( textString );
		thisEraBlockInstance.OldLabel:SetText( localizedLabel );
		thisEraBlockInstance.CurrentLabel:SetText( localizedLabel );
		thisEraBlockInstance.FutureLabel:SetText( localizedLabel );
		
		-- adjust the sizes of the era panels
		local blockWidth;
		if (eraColumns[era.ID] ~= nil) then
			blockWidth = (eraColumns[era.ID].maxGridX - eraColumns[era.ID].minGridX + 1);
		else
			blockWidth = 1;
		end
			
		blockWidth = (blockWidth * blockSpacingX);
		if era.ID == 0 then
			blockWidth = blockWidth + 32;
		end
		local blockSize = thisEraBlockInstance.EraBlock:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.EraBlock:SetSize( blockSize );
		blockSize = thisEraBlockInstance.FrameBottom:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.FrameBottom:SetSize( blockSize );	
		
		blockSize = thisEraBlockInstance.OldBar:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.OldBar:SetSize( blockSize );
		blockSize = thisEraBlockInstance.OldBlock:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.OldBlock:SetSize( blockSize );
		
		blockSize = thisEraBlockInstance.CurrentBlock:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.CurrentBlock:SetSize( blockSize );
		blockSize = thisEraBlockInstance.CurrentBlock1:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.CurrentBlock1:SetSize( blockSize );
		blockSize = thisEraBlockInstance.CurrentBlock2:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.CurrentBlock2:SetSize( blockSize );
				
		blockSize = thisEraBlockInstance.CurrentTop:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.CurrentTop:SetSize( blockSize );
		blockSize = thisEraBlockInstance.CurrentTop1:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.CurrentTop1:SetSize( blockSize );
		blockSize = thisEraBlockInstance.CurrentTop2:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.CurrentTop2:SetSize( blockSize );
				
		blockSize = thisEraBlockInstance.FutureBlock:GetSize();
		blockSize.x = blockWidth;
		thisEraBlockInstance.FutureBlock:SetSize( blockSize );
	end
end

function TechSelected( eTech, iDiscover)
	if bEspionageViewMode then
		return;
	end
	--print("eTech:"..tostring(eTech));
	print("stealingTechTargetPlayerID: " .. stealingTechTargetPlayerID);
	print("player:GetNumFreeTechs(): " ..  player:GetNumFreeTechs());
	if eTech > -1 then
		if (stealingTechTargetPlayerID ~= -1) then
			Network.SendResearch(eTech, 0, stealingTechTargetPlayerID, UIManager:GetShift());
		else
	   		Network.SendResearch(eTech, player:GetNumFreeTechs(), -1, UIManager:GetShift());
		end
   	end
end

function AddTechButton( tech )
	local thisTechButtonInstance = g_TechInstanceManager:GetInstance();
	if thisTechButtonInstance then
		
		-- store this instance off for later
		techButtons[tech.ID] = thisTechButtonInstance;
		
		-- add the input handler to this button
		thisTechButtonInstance.TechButton:SetVoid1( tech.ID ); -- indicates tech to add to queue
		thisTechButtonInstance.TechButton:SetVoid2( 0 ); -- how many free techs
		techPediaSearchStrings[tostring(thisTechButtonInstance.TechButton)] = tech.Description;
		thisTechButtonInstance.TechButton:RegisterCallback( Mouse.eRClick, GetTechPedia );

		local scienceDisabled = Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE);
 		if (not scienceDisabled) then
			thisTechButtonInstance.TechButton:RegisterCallback( Mouse.eLClick, TechSelected );
		end
		
		-- position this instance
		thisTechButtonInstance.TechButton:SetOffsetVal( tech.GridX*blockSpacingX + 64, (tech.GridY-5)*blockSpacingY + extraYOffset);
		
		-- update the name of this instance
		local techName = Locale.ConvertTextKey( tech.Description );
		
		techName = Locale.TruncateString(techName, maxTechNameLength, true);
		thisTechButtonInstance.AlreadyResearchedTechName:SetText( techName );
		thisTechButtonInstance.CurrentlyResearchingTechName:SetText( techName );
		thisTechButtonInstance.AvailableTechName:SetText( techName );
		thisTechButtonInstance.UnavailableTechName:SetText( techName );
		thisTechButtonInstance.LockedTechName:SetText( techName );
		thisTechButtonInstance.FreeTechName:SetText( techName );
		
		thisTechButtonInstance.TechButton:SetToolTipString( GetHelpTextForTech(tech.ID, false, playerID) );
		
		-- update the picture
		if IconHookup( tech.PortraitIndex, 64, tech.IconAtlas, thisTechButtonInstance.TechPortrait ) then
			thisTechButtonInstance.TechPortrait:SetHide( false );
		else
			thisTechButtonInstance.TechPortrait:SetHide( true );
		end
		
		-- add the small pictures and their tooltips
		AddSmallButtonsToTechButton( thisTechButtonInstance, tech, maxSmallButtons, 45, playerID );
		
	end
end


-------------------------------------------------
-- On Display
-------------------------------------------------
local g_isOpen = false;

function OnDisplay( popupInfo )

	if popupInfo.Type ~= ButtonPopupTypes.BUTTONPOPUP_TECH_TREE then
		return;
	end

	m_PopupInfo = popupInfo;

	print("popupInfo.Data1: " .. popupInfo.Data1);
	print("popupInfo.Data2: " .. popupInfo.Data2);
	print("popupInfo.Data3: " .. popupInfo.Data3);

    g_isOpen = true;
    if not g_NeedsFullRefresh then
		g_NeedsFullRefresh = g_NeedsFullRefreshOnOpen;
	end
	g_NeedsFullRefreshOnOpen = false;

	if( m_PopupInfo.Data1 == 1 ) then
    	if( ContextPtr:IsHidden() == false ) then
    	    OnCloseButtonClicked();
    	    return;
    	else
        	UIManager:QueuePopup( ContextPtr, PopupPriority.eUtmost );
    	end
	else
        UIManager:QueuePopup( ContextPtr, PopupPriority.TechTree );
    end
    
    stealingTechTargetPlayerID = popupInfo.Data2;
	
	if(popupInfo.Data4 > 0) then
		-- Espionage view mode
		bEspionageViewMode = true;
		playerID = popupInfo.Data5;
		g_NeedsFullRefresh = true;
		print("Espionage View Mode, showing Tech Tree for Player " .. playerID)	
	else
		bEspionageViewMode = false;
		playerID = Game.GetActivePlayer();
	end
	player = Players[playerID];
	civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
	activeTeamID = player:GetTeam();
	activeTeam = Teams[activeTeamID];
	if(g_NeedsFullRefresh) then
		GatherInfoAboutUniqueStuff( civType );
	end
    
	Events.SerialEventGameMessagePopupShown(m_PopupInfo);
		
  	RefreshDisplay();
	
end
Events.SerialEventGameMessagePopup.Add( OnDisplay );

function RefreshDisplay()

	-- reset the era panels
	for thisEra = 0, #eraBlocks, 1  do
		if eraColumns[thisEra] then
			eraColumns[thisEra].researched = false;
		end
	end
	
	for tech in GameInfo.Technologies() do
		RefreshDisplayOfSpecificTech( tech );
	end
	
	-- update the era panels
	local highestEra = 0;
	for thisEra = 0, #eraBlocks, 1  do
		if eraColumns[thisEra] then
			if eraColumns[thisEra].researched == true then
				highestEra = thisEra;
			end
		end
	end
	for thisEra = 0, #eraBlocks, 1  do
		local thisEraBlockInstance = eraBlocks[thisEra];
		if thisEra < highestEra then
			thisEraBlockInstance.OldBar:SetHide( false );
			thisEraBlockInstance.CurrentBlock:SetHide( true );
			thisEraBlockInstance.CurrentTop:SetHide( true );
			thisEraBlockInstance.FutureBlock:SetHide( true );
		elseif thisEra == highestEra then
			thisEraBlockInstance.OldBar:SetHide( true );
			thisEraBlockInstance.CurrentBlock:SetHide( false );
			thisEraBlockInstance.CurrentTop:SetHide( false );
			thisEraBlockInstance.FutureBlock:SetHide( true );
		else
			thisEraBlockInstance.OldBar:SetHide( true );
			thisEraBlockInstance.CurrentBlock:SetHide( true );
			thisEraBlockInstance.CurrentTop:SetHide( true );
			thisEraBlockInstance.FutureBlock:SetHide( false );
		end
	end
	
	g_NeedsFullRefresh = false;
end

function RefreshDisplayOfSpecificTech( tech )
	local techID = tech.ID;
	local thisTechButton = techButtons[techID];
  	local numFreeTechs = player:GetNumFreeTechs();
 	local researchTurnsLeft = player:GetResearchTurnsLeft( techID, true );
 	local turnText =  Locale.Lookup("TXT_KEY_BUILD_NUM_TURNS", researchTurnsLeft );
	local isAllowedToStealTech = false;
	local isAllowedToGetTechFree = false;
	
	if( bEspionageViewMode) then
		numFreeTechs = 0;
		thisTechButton.TechButton:SetDisabled(true);
	else
		thisTechButton.TechButton:SetDisabled(false);
	end
	
	-- Espionage - stealing a tech!
 	if stealingTechTargetPlayerID ~= -1 then
 		if player:CanResearch( techID ) then
			opponentPlayer = Players[stealingTechTargetPlayerID];
			local opponentTeam = Teams[opponentPlayer:GetTeam()];
			if (opponentTeam:IsHasTech(techID)) then
				isAllowedToStealTech = true;
			end
		end
	end
	
	-- Choosing a free tech - extra conditions may apply
	if (numFreeTechs > 0) then
		if (player:CanResearchForFree(techID)) then
			isAllowedToGetTechFree = true;
		end
	end
 	
 	local potentiallyBlockedFromStealing = (stealingTechTargetPlayerID ~= -1) and ((not isAllowedToStealTech) or player:GetNumTechsToSteal(stealingTechTargetPlayerID) <= 0);
 	
 	-- Rebuild the small buttons if needed
 	if (g_NeedsFullRefresh) then
		AddSmallButtonsToTechButton( thisTechButton, tech, maxSmallButtons, 45, playerID );
 	end
 	
 	thisTechButton.TechButton:SetToolTipString( GetHelpTextForTech(techID, false, playerID) );
 	
 	local scienceDisabled = Game.IsOption(GameOptionTypes.GAMEOPTION_NO_SCIENCE);
 	if (scienceDisabled) then
 		turnText = "";
 	end
 	
 	
	if(not scienceDisabled) then
		thisTechButton.TechButton:SetVoid1( techID ); -- indicates tech to add to queue
		thisTechButton.TechButton:SetVoid2( numFreeTechs ); -- how many free techs
		AddCallbackToSmallButtons( thisTechButton, maxSmallButtons, techID, numFreeTechs, Mouse.eLClick, TechSelected );
	end
	
 	if activeTeam:GetTeamTechs():HasTech(techID) then -- the player (or more accurately his team) has already researched this one
 		thisTechButton.AlreadyResearched:SetHide( false );
 		thisTechButton.FreeTech:SetHide( true );
 		thisTechButton.CurrentlyResearching:SetHide( true );
 		thisTechButton.Available:SetHide( true );
 		thisTechButton.Unavailable:SetHide( true );
		thisTechButton.Locked:SetHide( true );
 
 		-- figure out if we need the first place dingus
 
 		-- update the era marker for this tech
 		local eraID = GameInfo.Eras[tech.Era].ID;
		if eraColumns[eraID] then
			eraColumns[eraID].researched = true;
		end
		
  		-- hide advisor icon
 		--thisTechButton.AdvisorIcon:SetHide( true );
 				
		if(not scienceDisabled) then
			thisTechButton.TechQueue:SetHide( true );
			thisTechButton.TechButton:SetVoid2( 0 ); -- num free techs
			thisTechButton.TechButton:SetVoid1( -1 ); -- indicates tech is invalid
			AddCallbackToSmallButtons( thisTechButton, maxSmallButtons, -1, 0, Mouse.eLClick, TechSelected );
 		end
 		
 	elseif player:GetCurrentResearch() == techID and (not potentiallyBlockedFromStealing) then -- the player is currently researching this one
 		thisTechButton.AlreadyResearched:SetHide( true );
 		thisTechButton.Available:SetHide( true );
 		thisTechButton.Unavailable:SetHide( true );
		thisTechButton.Locked:SetHide( true );
		-- deal with free tech
		if (isAllowedToGetTechFree) or (stealingTechTargetPlayerID ~= -1 and isAllowedToStealTech) then
  			thisTechButton.FreeTech:SetHide( false );
 			thisTechButton.CurrentlyResearching:SetHide( true );
			-- update number of turns to research
 			if 	player:GetScience() > 0 and stealingTechTargetPlayerID == -1 then
  				thisTechButton.FreeTurns:SetText( turnText );
  				thisTechButton.FreeTurns:SetHide( false );
  			else
  				thisTechButton.FreeTurns:SetHide( true );
  			end
			thisTechButton.TechQueueLabel:SetText( freeString );
			thisTechButton.TechQueue:SetHide( false );
		else
  			thisTechButton.FreeTech:SetHide( true );
 			thisTechButton.CurrentlyResearching:SetHide( false );
			-- update number of turns to research
 			if 	player:GetScience() > 0 then
  				thisTechButton.CurrentlyResearchingTurns:SetText( turnText );
  				thisTechButton.CurrentlyResearchingTurns:SetHide( false );
  			else
  				thisTechButton.CurrentlyResearchingTurns:SetHide( true );
  			end
			thisTechButton.TechQueue:SetHide( true );
		end
 		-- turn on the meter
		local teamTechs = activeTeam:GetTeamTechs();
		local researchProgressPercent = 0;
		local researchProgressPlusThisTurnPercent = 0;
		local researchTurnsLeft = player:GetResearchTurnsLeft(techID, true);
		local currentResearchProgress = player:GetResearchProgress(techID);
		local researchNeeded = player:GetResearchCost(techID);
		local researchPerTurn = player:GetScience();
		local currentResearchPlusThisTurn = currentResearchProgress + researchPerTurn;		
		researchProgressPercent = currentResearchProgress / researchNeeded;
		researchProgressPlusThisTurnPercent = currentResearchPlusThisTurn / researchNeeded;		
		if (researchProgressPlusThisTurnPercent > 1) then
			researchProgressPlusThisTurnPercent = 1
		end
 		-- update advisor icon if needed
 		--thisTechButton.AdvisorIcon:SetHide( true );
 	elseif (player:CanResearch( techID ) and not scienceDisabled and (not potentiallyBlockedFromStealing)) then -- the player research this one right now if he wants
 		thisTechButton.AlreadyResearched:SetHide( true );
 		thisTechButton.CurrentlyResearching:SetHide( true );
 		thisTechButton.Unavailable:SetHide( true );
		thisTechButton.Locked:SetHide( true );
 		-- deal with free 		
		if (isAllowedToGetTechFree)  or (stealingTechTargetPlayerID ~= -1 and isAllowedToStealTech) then
 			thisTechButton.FreeTech:SetHide( false );
 			thisTechButton.Available:SetHide( true );
			-- update number of turns to research
 			if 	player:GetScience() > 0 and stealingTechTargetPlayerID == -1 then
  				thisTechButton.FreeTurns:SetText( turnText );
  				thisTechButton.FreeTurns:SetHide( false );
  			else
  				thisTechButton.FreeTurns:SetHide( true );
  			end
			-- update queue number to say "FREE"
			thisTechButton.TechQueueLabel:SetText( freeString );
			thisTechButton.TechQueue:SetHide( false );
		else
 			thisTechButton.FreeTech:SetHide( true );
 			thisTechButton.Available:SetHide( false );
			-- update number of turns to research
 			if 	player:GetScience() > 0 then
  				thisTechButton.AvailableTurns:SetText( turnText );
  				thisTechButton.AvailableTurns:SetHide( false );
  			else
  				thisTechButton.AvailableTurns:SetHide( true );
  			end
			-- update queue number if needed
			local queuePosition = player:GetQueuePosition( techID );
			if queuePosition == -1 then
				thisTechButton.TechQueue:SetHide( true );
			else
				thisTechButton.TechQueueLabel:SetText( tostring( queuePosition-1 ) );
				thisTechButton.TechQueue:SetHide( false );
			end
		end
  		-- update advisor icon if needed
 		--thisTechButton.AdvisorIcon:SetHide( true );
 	elseif (not player:CanEverResearch( techID ) or isAllowedToGetTechFree or stealingTechTargetPlayerID ~= -1) then
 		thisTechButton.AlreadyResearched:SetHide( true );
 		thisTechButton.CurrentlyResearching:SetHide( true );
 		thisTechButton.Available:SetHide( true );
 		thisTechButton.Unavailable:SetHide( true );
		thisTechButton.Locked:SetHide( false );
  		thisTechButton.FreeTech:SetHide( true );
		-- have queue number say "LOCKED"
		thisTechButton.TechQueueLabel:SetText( lockedString );
		thisTechButton.TechQueue:SetHide( false );
		-- hide the advisor icon
 		--thisTechButton.AdvisorIcon:SetHide( true );
		if(not scienceDisabled) then
			thisTechButton.TechButton:SetVoid1( -1 ); 
			thisTechButton.TechButton:SetVoid2( 0 ); -- num free techs
			AddCallbackToSmallButtons( thisTechButton, maxSmallButtons, -1, 0, Mouse.eLClick, TechSelected );
 		end
 	else -- currently unavailable
 		thisTechButton.AlreadyResearched:SetHide( true );
 		thisTechButton.CurrentlyResearching:SetHide( true );
 		thisTechButton.Available:SetHide( true );
 		thisTechButton.Unavailable:SetHide( false );
		thisTechButton.Locked:SetHide( true );
  		thisTechButton.FreeTech:SetHide( true );
 		-- update number of turns to research
 		if 	player:GetScience() > 0 then
  			thisTechButton.UnavailableTurns:SetText( turnText );
  			thisTechButton.UnavailableTurns:SetHide( false );
  		else
  			thisTechButton.UnavailableTurns:SetHide( true );
  		end
  		
		-- update queue number if needed
		local queuePosition = player:GetQueuePosition( techID );
		if queuePosition == -1 then
			thisTechButton.TechQueue:SetHide( true );
		else
			thisTechButton.TechQueueLabel:SetText( tostring( queuePosition-1 ) );
			thisTechButton.TechQueue:SetHide( false );
		end
		
 		-- update advisor icon if needed
 		--thisTechButton.AdvisorIcon:SetHide( true );

		if (isAllowedToGetTechFree) then
			thisTechButton.TechButton:SetVoid1( -1 ); 
			AddCallbackToSmallButtons( thisTechButton, maxSmallButtons, -1, 0, Mouse.eLClick, OnTechnologyButtonClicked );
		else
			if(not scienceDisabled) then
				thisTechButton.TechButton:SetVoid1( tech.ID );
				AddCallbackToSmallButtons( thisTechButton, maxSmallButtons, techID, numFreeTechs, Mouse.eLClick, TechSelected );
			end
		end
	end
end

----------------------------------------------------------------        
-- Input processing
----------------------------------------------------------------        

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnCloseButtonClicked ()
	UIManager:DequeuePopup( ContextPtr );
    Events.SerialEventGameMessagePopupProcessed.CallImmediate(ButtonPopupTypes.BUTTONPOPUP_TECH_TREE, 0);
    g_isOpen = false;
	if(bEspionageViewMode) then
		g_NeedsFullRefreshOnOpen = true;
	end
end
Controls.CloseButton:RegisterCallback( Mouse.eLClick, OnCloseButtonClicked );

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

function InputHandler( uiMsg, wParam, lParam )
    if g_isOpen and uiMsg == KeyEvents.KeyDown then
        if wParam == Keys.VK_ESCAPE or wParam == Keys.VK_RETURN then
            OnCloseButtonClicked();
            return true;
        end
    end
end
ContextPtr:SetInputHandler( InputHandler );


function ShowHideHandler( bIsHide, bIsInit )
    if( not bIsInit ) then
        if( not bIsHide ) then
        	UI.incTurnTimerSemaphore();
        else
        	UI.decTurnTimerSemaphore();
        end
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

----------------------------------------------------------------
-- 'Active' (local human) player has changed
----------------------------------------------------------------
function OnTechTreeActivePlayerChanged( iActivePlayer, iPrevActivePlayer )
	playerID = Game.GetActivePlayer();	
	player = Players[playerID];
	civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
	activeTeamID = Game.GetActiveTeam();
	activeTeam = Teams[activeTeamID];	
	-- Rebuild some tables	
	GatherInfoAboutUniqueStuff( civType );	
	
	-- So some extra stuff gets re-built on the refresh call
	if not g_isOpen then
		g_NeedsFullRefreshOnOpen = true;	
	else
		g_NeedsFullRefresh = true;
	end
	
	-- Close it, so the next player does not have to.
	OnCloseButtonClicked();
end
Events.GameplaySetActivePlayer.Add(OnTechTreeActivePlayerChanged);

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
function OnEventResearchDirty()
	if (g_isOpen) then
		RefreshDisplay();
	end
end
Events.SerialEventResearchDirty.Add(OnEventResearchDirty);

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

-- One time initialization
InitialSetup()