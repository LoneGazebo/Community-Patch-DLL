if CodeBuddy.vpRegisterContext and not CodeBuddy.vpRegisterContext() then return end
-------------------------------------------------
-- Hall of Fame
-------------------------------------------------
include( "IconSupport" );
include( "InstanceManager" );
include( "MapUtilities" );
----------------------------------------------------------------        
----------------------------------------------------------------        
local m_bIsEndGame = (ContextPtr:GetID() == "EndGameHallOfFame");

local g_GamesIM = InstanceManager:new( "GameInstance", "GameItem", Controls.GameStack );

----------------------------------------------------------------        
----------------------------------------------------------------        
function OnBack()
	UIManager:DequeuePopup( ContextPtr );
end
Controls.BackButton:RegisterCallback( Mouse.eLClick, OnBack );

----------------------------------------------------------------        
----------------------------------------------------------------
function PopulateGameResults()
	g_GamesIM:ResetInstances();
	
	local gameEntries = {};
	
	local games = UI.GetHallofFameData();
	
	for i, v in pairs(games) do
	
		local civ = GameInfo.Civilizations[v.PlayerCivilizationType];
		if(civ ~= nil) then
			
			-- Use the Civilization_Leaders table to cross reference from this civ to the Leaders table
			local leader = GameInfo.Leaders[GameInfo.Civilization_Leaders( "CivilizationType = '" .. civ.Type .. "'" )().LeaderheadType];
			if(leader ~= nil) then
			
				local controlTable = g_GamesIM:GetInstance();
				table.insert(gameEntries, controlTable);
				
				-- Fill In Player Info
				if(v.PlayerTeamWon)then
					controlTable.DidWin:LocalizeAndSetText("TXT_KEY_VICTORY_BANG");
				else
					controlTable.DidWin:LocalizeAndSetText("TXT_KEY_DEFEAT_BANG");
				end
								
				local leaderDescription = leader.Description;

				IconHookup( leader.PortraitIndex, 64, leader.IconAtlas, controlTable.LeaderPortrait );
				IconHookup( civ.PortraitIndex, 32, civ.IconAtlas, controlTable.CivSymbol );
				
				--If no custom name
				if(v.LeaderName == nil or v.LeaderName == "")then
					controlTable.LeaderName:LocalizeAndSetText(leaderDescription);
					controlTable.CivName:LocalizeAndSetText(civ.ShortDescription);
				else
					controlTable.LeaderName:LocalizeAndSetText(v.LeaderName);
					controlTable.CivName:LocalizeAndSetText(v.CivilizationName);
				end
				
				local info = GameInfo.HandicapInfos[ v.PlayerHandicapType ];
				if( info ~= nil) then
					IconHookup( info.PortraitIndex, 32, info.IconAtlas, controlTable.Difficulty );
					controlTable.Difficulty:SetToolTipString( Locale.ConvertTextKey( info.Description ) );	
				else
					controlTable.Difficulty:SetHide(true);
				end
				controlTable.Score:SetText(v.Score);

				-- Fill In Game Info
				local civ = nil;
				if(v.WinningTeamLeaderCivilizationType ~= nil) then
					civ = GameInfo.Civilizations[v.WinningTeamLeaderCivilizationType];
				end
				
				if(v.PlayerTeamWon)then
					controlTable.WinningCivName:LocalizeAndSetText("TXT_KEY_POP_VOTE_RESULTS_YOU");
				
					--Need Front End solution for Team Icons ~khirsch
					IconHookup( civ.PortraitIndex, 64, civ.IconAtlas, controlTable.CivIconBG );
					
					controlTable.CivIconShadow:SetHide(true);
					controlTable.WinCivIcon:SetHide(true);
					local white = {x = 1, y = 1, z = 1, w = 1};
					controlTable.CivIconBG:SetColor( white );
				else
					if(civ)then
						controlTable.WinningCivName:LocalizeAndSetText(civ.ShortDescription);
						
						defaultColorSet = GameInfo.PlayerColors[civ.DefaultPlayerColor];
						if(defaultColorSet.PrimaryColor ~= v.WinningTeamPrimaryColor or defaultColorSet.SecondaryColor ~= WinningTeamSecondaryColor)then
							local textureOffset, textureAtlas = IconLookup( civ.PortraitIndex, 48, civ.AlphaIconAtlas );
							if textureOffset ~= nil then     
								controlTable.WinCivIcon:SetHide(false);  
								controlTable.WinCivIcon:SetTexture( textureAtlas );
								controlTable.WinCivIcon:SetTextureOffset( textureOffset );
									controlTable.CivIconShadow:SetTexture( textureAtlas );
									controlTable.CivIconShadow:SetTextureOffset( textureOffset );		
							end	
							
							local primaryColor = GameInfo.Colors[v.WinningTeamPrimaryColor];
							if(primaryColor ~= nil) then
								local primaryColorVector = Vector4( primaryColor.Red, primaryColor.Green, primaryColor.Blue, primaryColor.Alpha );
								controlTable.WinCivIcon:SetColor( primaryColorVector );
							end
							
							local secondaryColor = GameInfo.Colors[v.WinningTeamSecondaryColor];
							if(secondaryColor ~= nil) then
								local secondaryColorVector = Vector4( secondaryColor.Red, secondaryColor.Green, secondaryColor.Blue, secondaryColor.Alpha );
								controlTable.CivIconBG:SetColor( secondaryColorVector );
							end
							
						else
							IconHookup( civ.PortraitIndex, 64, civ.IconAtlas, controlTable.CivIconBG );
							controlTable.CivIconShadow:SetHide(true);
							controlTable.WinCivIcon:SetHide(true);
						end
					else
						controlTable.WinningCivName:LocalizeAndSetText("TXT_KEY_CITY_STATE_NOBODY");
						IconHookup( 23, 64, "CIV_COLOR_ATLAS", controlTable.CivIconBG );
						controlTable.CivIconShadow:SetHide(true);
						controlTable.WinCivIcon:SetHide(true);
					end
					
				end
						
				if(v.VictoryType)then
					controlTable.WinType:LocalizeAndSetText(GameInfo.Victories[v.VictoryType].VictoryStatement);
				else
					controlTable.WinType:LocalizeAndSetText("");
				end
				
				local mapInfo = MapUtilities.GetBasicInfo(v.MapName);
				IconHookup( mapInfo.IconIndex, 32, mapInfo.IconAtlas, controlTable.MapType );        
				controlTable.MapType:SetToolTipString( Locale.ConvertTextKey( mapInfo.Name ) );
				
				info = GameInfo.Worlds[ v.WorldSize ];
				if ( info ~= nil ) then
					IconHookup( info.PortraitIndex, 32, info.IconAtlas, controlTable.MapSize );
					controlTable.MapSize:SetToolTipString( Locale.ConvertTextKey( info.Description ) );
				end
				
				info = GameInfo.GameSpeeds[ v.GameSpeedType ];
				if ( info ~= nil ) then
					IconHookup( info.PortraitIndex, 32, info.IconAtlas, controlTable.SpeedIcon );
					controlTable.SpeedIcon:SetToolTipString( Locale.ConvertTextKey( info.Description ) );
				end
				
				--controlTable.MaxTurns:LocalizeAndSetText("TXT_KEY_MAX_TURN_FORMAT", v.WinningTurn);
				
				info = GameInfo.Eras[ v.StartEraType ];
				if ( info ~= nil ) then
					controlTable.StartEraTurns:LocalizeAndSetText("TXT_KEY_ERA_TURNS_FORMAT", Locale.ConvertTextKey(info.Description), v.WinningTurn);
					controlTable.TimeFinished:SetText(v.GameEndTime);
				end
				
				controlTable.IconStack:CalculateSize();
				controlTable.IconStack:ReprocessAnchoring();
				
				controlTable.SettingStack:CalculateSize();
				controlTable.SettingStack:ReprocessAnchoring();
				
				local itemY = controlTable.SettingStack:GetSizeY();
				controlTable.GameItem:SetSizeY(itemY);
			end
		end
	end
	
	Controls.HallOfFameEmpty:SetHide(table.count(gameEntries) > 0);
	
	Controls.GameStack:CalculateSize();
	Controls.GameStack:ReprocessAnchoring();
	Controls.ScrollPanel:CalculateInternalSize();
end

----------------------------------------------------------------        
-- Key Down Processing
----------------------------------------------------------------        
function InputHandler( uiMsg, wParam, lParam )
    if( uiMsg == KeyEvents.KeyDown )
    then
        if( wParam == Keys.VK_RETURN or wParam == Keys.VK_ESCAPE ) then
			OnBack();
        end
    end
    return true;
end
if( not m_bIsEndGame ) then
	ContextPtr:SetInputHandler( InputHandler );
	Controls.Background:SetHide(false);
end

-------------------------------------------------
-------------------------------------------------
function ShowHideHandler( bIsHide )
	if(not bIsHide)then
		PopulateGameResults();
	end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );

-------------------------------------------------
-- If we hear a multiplayer game invite was sent, exit
-- so we don't interfere with the transition
-------------------------------------------------
function OnMultiplayerGameInvite()
   	if( ContextPtr:IsHidden() == false ) then
		OnBack();
	end
end

Events.MultiplayerGameLobbyInvite.Add( OnMultiplayerGameInvite );
Events.MultiplayerGameServerInvite.Add( OnMultiplayerGameInvite );
----------------------------------------------------------------        
----------------------------------------------------------------        

if( m_bIsEndGame ) then
    Controls.FrontEndFrame:SetHide( true );
    Controls.Background:SetHide(true);
end