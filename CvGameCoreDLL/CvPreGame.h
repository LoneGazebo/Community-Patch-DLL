/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#ifndef _CvPreGame_H
#define _CvPreGame_H

class FIGameIniParser;

// There would never conceivably be more than one set of
// pre-game data active at once.
namespace CvPreGame
{
	typedef std::list<GUID> PackageIDList;

	enum Version
	{

	};

	//NOTE: Should this use VARIANTs instead of ints?
	class CustomOption
	{
	public:
		CustomOption();
		CustomOption(const char* szOptionName, int iVal);
		CustomOption(const CustomOption& copy);
		CustomOption& operator=(const CustomOption&);
		bool operator ==(const CustomOption& b) const;

		const char* GetName() const;
		const char* GetName(size_t& bytes) const;
		int GetValue() const;

		friend FDataStream & operator<<(FDataStream & saveTo, const CustomOption & readFrom);
		friend FDataStream & operator>>(FDataStream & loadFrom, CustomOption & writeTo);

	private:
		char m_szOptionName[64];
		int m_iValue;
	};

	FDataStream & operator>>(FDataStream&, CustomOption&);
	FDataStream & operator<<(FDataStream&, const CustomOption&);


	PlayerTypes                                activePlayer                     ();
	const CvString &                           adminPassword                    ();
	int                                        advancedStartPoints              ();
	const CvString &                           alias                            (); // setupdata
	ArtStyleTypes                              artStyle                         (PlayerTypes p);
	bool                                       autorun                          (); // setupdata
	float                                      autorunTurnDelay                 (); // setupdata
	int                                        autorunTurnLimit                 (); // setupdata
	CvString                                   bandwidthDescription             (); // setupdata
	CalendarTypes                              calendar                         ();
	bool										 canReadyLocalPlayer			  ();
	const CvString &                           civilizationAdjective            (PlayerTypes p);
	const CvString &                           civilizationAdjectiveKey         (PlayerTypes p);
	const CvString &                           civilizationDescription          (PlayerTypes p);
	const CvString &                           civilizationDescriptionKey       (PlayerTypes p);
	CivilizationTypes                          civilization                     (PlayerTypes p);
	const CvString &                           civilizationPassword             (PlayerTypes p);
	const CvString &                           civilizationShortDescription     (PlayerTypes p);
	const CvString &                           civilizationShortDescriptionKey  (PlayerTypes p);
	const CvString &							 civilizationKey				  (PlayerTypes p);
	bool										 civilizationKeyAvailable		  (PlayerTypes p);
	const GUID &								 civilizationKeyPackageID		  (PlayerTypes p);
	void										 clearDLCAllowed				  ();
	void										 clearDLCAvailable				  (PlayerTypes p);
	ClimateTypes                               climate                          ();
	const CvClimateInfo &                      climateInfo                      ();
	void                                       closeAllSlots					  ();
	void                                       closeInactiveSlots               ();
	const CvString &                           emailAddress                     (PlayerTypes p);
	const CvString &                           emailAddress                     (); // setupdata
	float                                      endTurnTimerLength               (); // setupdata
	EraTypes                                   era                              ();
	PlayerTypes                                findPlayerByNickname             (const char * const name);
	void                                       ReseatConnectedPlayers           ();
	GameMode                                   gameMode                         ();
	const CvString &                           gameName                         ();
	bool                                       gameStarted                      ();
	bool										 GetGameOption					  (const char* szOptionName, int& iValue);
	bool									     GetGameOption					  (GameOptionTypes eOption, int& iValue);
	const std::vector<CustomOption>&			 GetGameOptions					  ();
	bool										 GetMapOption					  (const char* szOptionName, int& iValue);
	const std::vector<CustomOption>&			 GetMapOptions					  ();
	bool										 GetPersistSettings				  ();
	const std::vector<SlotStatus>&			 GetSlotStatus   				  ();
	uint										 getDLCAllowedCount				  ();
	const PackageIDList&						 getDLCAllowed					  ();
	const PackageIDList&						 getDLCAvailable					  (PlayerTypes p);
	void										 setDLCAvailable				  (PlayerTypes p, const PackageIDList& kList);
	GameSpeedTypes                             gameSpeed                        ();
	int                                        gameTurn                         ();
	GameTypes                                  gameType                         ();
	GameMapTypes                               gameMapType                      ();
	GameStartTypes                             gameStartType                    ();
	int                                        gameUpdateTime                   (); // setupdata
	HandicapTypes                              handicap                         (PlayerTypes p);
	HandicapTypes                              lastHumanHandicap				(PlayerTypes p);
	bool										 isDLCAllowed					  (const GUID& kDLCID);
	bool										 isDLCAvailable				      (PlayerTypes p, const GUID& kDLCID);
	bool                                       isHotSeat                        ();
	bool                                       isHotSeatGame                    ();
	bool                                       isHuman                          (PlayerTypes p);
	bool                                       isInternetGame                   ();
	bool                                       isMinorCiv                       (PlayerTypes p);
	bool                                       isNetworkMultiplayerGame         ();
	bool                                       isPitBoss                        ();
	bool                                       isPlayable                       (PlayerTypes p);
	bool                                       isPlayByEmail                    ();
	bool                                       isReady                          (PlayerTypes p);
	bool										 IsCityScreenBlocked              ();
	bool                                       isVictory                        (VictoryTypes v);
	bool                                       isWBMapScript                    ();
	bool                                       isWhiteFlag                      (PlayerTypes p);
	bool									   									 isTurnNotifySteamInvite					(PlayerTypes p); 
	bool																			 isTurnNotifyEmail								(PlayerTypes p);
	const CvString&														 getTurnNotifyEmailAddress				(PlayerTypes p);             
	LeaderHeadTypes                            leaderHead                       (PlayerTypes p);
	const CvString &                           leaderName                       (PlayerTypes p);
	const CvString &                           leaderNameKey                    (PlayerTypes p);
	const CvString &							 leaderKey						  (PlayerTypes p);
	bool										 leaderKeyAvailable				  (PlayerTypes p);
	const GUID &								 leaderKeyPackageID				  (PlayerTypes p);
	const CvString &                           loadFileName                     (); // setupdata
	StorageLocation                            loadFileStorage                  (); // setupdata
	void                                       loadFromIni                      (FIGameIniParser & iniParser);
	bool                                       mapNoPlayers                     ();
	unsigned int                               mapRandomSeed                    ();
	bool                                       loadWBScenario                   ();
	bool                                       overrideScenarioHandicap         ();
	const CvString &                           mapScriptName                    ();
	int                                        maxCityElimination               ();
	int                                        maxTurns                         ();
	MinorCivTypes                              minorCivType                     (PlayerTypes p);
	bool                                       multiplayerAIEnabled             ();
	bool                                       multiplayerOptionEnabled         (MultiplayerOptionTypes o);
	const std::vector<bool> &                  multiplayerOptions               ();
	int                                        netID                            (PlayerTypes p);
	const CvString &                           nickname                         (PlayerTypes p);
	const CvString &                           nicknameDisplayed                (PlayerTypes p);
	int                                        numDefinedPlayers                ();
	int                                        numHumans                        ();
	int                                        numMinorCivs                     ();
	int                                        pitBossTurnTime                  ();
	PlayerColorTypes                           playerColor                      (PlayerTypes p);
	bool                                       privateGame                      ();
	bool                                       quickCombat                      ();
	bool                                       quickCombatDefault               ();
	HandicapTypes                              quickHandicap                    (); // setupdata
	bool                                       quickMovement                    ();
	bool                                       quickstart                       (); // setupdata
	bool                                       randomWorldSize                  (); // setupdata
	bool                                       randomMapScript                  (); // setupdata
	void                                       read                             (FDataStream & loadFrom, bool bReadVersion);
	bool                                       readPlayerSlotInfo               (FDataStream & loadFrom, bool bReadVersion);
	void                                       resetGame                        ();
	void										 ResetGameOptions				  ();
	void							             ResetMapOptions                  ();
	void                                       resetPlayer                      (PlayerTypes p);
	void                                       resetPlayers                     ();
	void                                       resetSlots                       ();
	void                                       restoreSlots                     ();
	void                                       saveSlots                        ();
	SeaLevelTypes                              seaLevel                         ();
	const CvSeaLevelInfo &                     seaLevelInfo                     ();
	void                                       setActivePlayer                  (PlayerTypes p);
	void                                       setAdminPassword                 (const CvString & p);
	void                                       setAdvancedStartPoints           (int a);
	void                                       setAlias                         (const CvString & a); // setupdata
	void                                       setArtStyle                      (PlayerTypes p, ArtStyleTypes a);
	void                                       setAutorun                       (bool isAutoStart); // setupdata
	void                                       setAutorunTurnDelay              (float turnDelay); // setupdata
	void                                       setAutorunTurnLimit              (int turnLimit); // setupdata
	void                                       setBandwidth                     (BandwidthType b); // setupdata
	void                                       setBandwidth                     (const CvString & b); // setupdata
	void                                       setCalendar                      (CalendarTypes c);
	void                                       setCalendar                      (const CvString & c);
	void                                       setCivilization                  (PlayerTypes p, CivilizationTypes c);
	void                                       setCivilizationAdjective         (PlayerTypes p, const CvString & a);
	void                                       setCivilizationDescription       (PlayerTypes p, const CvString & d);
	void                                       setCivilizationPassword          (PlayerTypes p, const CvString & pwd);
	void                                       setCivilizationShortDescription  (PlayerTypes p, const CvString & d);
	void                                       setCivilizationKey				  (PlayerTypes p, const CvString & d);
	void										 setCivilizationKeyPackageID      (PlayerTypes p, const GUID & kKey);
	void                                       setClimate                       (ClimateTypes c);
	void                                       setClimate                       (const CvString & c);
	void                                       setCustomWorldSize               (int iWidth, int iHeight, int iPlayers = 0, int iMinorCivs = 0);
	void										 setDLCAllowed					  (const GUID& kDLCID, bool bState);
	void                                       setEmailAddress                  (PlayerTypes p, const CvString & a);
	void                                       setEmailAddress                  (const CvString & a); // setupdata
	void                                       setEndTurnTimerLength            (float f); // setupdata
	void                                       setEra                           (EraTypes e);
	void                                       setEra                           (const CvString & e);
	void                                       setFlagDecal                     (PlayerTypes p, const CvString & flagDecal);
	void                                       setGameMode                      (GameMode g);
	void                                       setGameName                      (const CvString & g);
	void                                       setGameSpeed                     (GameSpeedTypes g);
	void                                       setGameSpeed                     (const CvString & g);
	void                                       setGameStarted                   (bool);
	void                                       setGameTurn                      (int turn);
	bool										 SetGameOption					  (const char* szOptionName, int iValue);
	bool                                       SetGameOption                    (GameOptionTypes eOption, int iValue);
	bool										 SetGameOptions					  (const std::vector<CustomOption>& gameOptions);
	void                                       setGameType                      (GameTypes g);
	void                                       setGameType                      (GameTypes g, GameStartTypes eStartType);
	void                                       setGameType                      (const CvString & g);
	void                                       setGameStartType                 (GameStartTypes g);
	void                                       setGameUpdateTime                (int updateTime); // setupdata
	void                                       setHandicap                      (PlayerTypes p, HandicapTypes h);
	void									   setLastHumanHandicap				(PlayerTypes p, HandicapTypes h);
	void                                       setInternetGame                  (bool isInternetGame);
	void                                       setLeaderHead                    (PlayerTypes p, LeaderHeadTypes l);
	void                                       setLeaderName                    (PlayerTypes p, const CvString & n);
	void                                       setLeaderKey                     (PlayerTypes p, const CvString & n);
	void                                       setLeaderKeyPackageID            (PlayerTypes p, const GUID & n);
	void										 setLoadFileName				  (const CvString & fileName);
	void										 setLoadFileName				  (const CvString & fileName, StorageLocation eStorage);
	void                                       setMapNoPlayers                  (bool p);
	void                                       setMapRandomSeed                 (unsigned int newSeed);
	void                                       setLoadWBScenario                (bool b);
	void                                       setOverrideScenarioHandicap      (bool b);
	void                                       setMapScriptName                 (const CvString & s);
	bool										 SetMapOption					  (const char* szOptionName, int iValue);
	bool										 SetMapOptions					  (const std::vector<CustomOption>& mapOptions);
	void                                       setMaxCityElimination            (int m);
	void                                       setMaxTurns                      (int maxTurns);
	void                                       setMinorCivType                  (PlayerTypes p, MinorCivTypes m);
	void                                       setMinorCiv                      (PlayerTypes p, bool isMinor);
	void                                       setMultiplayerAIEnabled          (bool isEnabled);
	void                                       setMultiplayerOption             (MultiplayerOptionTypes o, bool enabled);
	void                                       setMultiplayerOptions            (const std::vector<bool> & o);
	void                                       setNetID                         (PlayerTypes p, int id);
	void                                       setNickname                      (PlayerTypes p, const CvString & n); // setupdata
	void                                       setNumMinorCivs                  (int n);
	void										 setPersistSettings				  (bool bPersist);
	void                                       setPitBossTurnTime               (int t);
	void                                       setPlayable                      (PlayerTypes p, bool playable);
	void                                       setPlayerColor                   (PlayerTypes p, PlayerColorTypes c);
	void                                       setPrivateGame                   (bool isPrivateGame);
	void                                       setQuickCombat                   (bool isQuickCombat); // setupdata
	void                                       setQuickCombatDefault            (bool isQuickCombat);
	void                                       setQuickHandicap                 (HandicapTypes h); // setupdata
	void                                       setQuickHandicap                 (const CvString & h); // setupdata
	void                                       setQuickMovement                 (bool isQuickMovement); // setupdata
	void                                       setQuickstart                    (bool isQuickStart); // setupdata
	void                                       setRandomWorldSize               (bool isRandomWorldSize); // setupdata
	void                                       setRandomMapScript               (bool isRandomWorldScript); // setupdata
	void                                       setReady                         (PlayerTypes p, bool bIsReady);
	void                                       setSeaLevel                      (SeaLevelTypes s);
	void                                       setSeaLevel                      (const CvString & s);
	void                                       setSlotClaim                     (PlayerTypes p, SlotClaim c);
	void                                       setSlotStatus                    (PlayerTypes eID, SlotStatus eSlotStatus);
	void									   setAllSlotStatus				  (const std::vector<SlotStatus> & vSlotStatus);
	void                                       setSyncRandomSeed                (unsigned int newSeed);
	void                                       setTargetScore                   (int s);
	void                                       setTeamType                      (PlayerTypes p, TeamTypes t);
	void                                       setTransferredMap                (bool transferred); // setupdata
	void                                       setTurnTimer                     (TurnTimerTypes t);
	void                                       setTurnTimer                     (const CvString & t);
	void										 SetCityScreenBlocked             (bool bCityScreenBlocked);
	void                                       setVersionString                 (const std::string & v);
	void                                       setVictory                       (VictoryTypes v, bool isValid);
	void                                       setVictories                     (const std::vector<bool> & v);
	void                                       setWhiteFlag                     (PlayerTypes p, bool flag);
	void									   									 setTurnNotifySteamInvite					(PlayerTypes p, bool flag);
	void																		 	 setTurnNotifyEmail								(PlayerTypes p, bool flag);
	void																		 	 setTurnNotifyEmailAddress				(PlayerTypes p, const CvString& emailAddress);
	void									   									 VerifyHandicap										(PlayerTypes p);
	void                                       setWorldSize                     (WorldSizeTypes w, bool bResetSlots=true);
	void                                       setWorldSize                     (const CvString & w);
	SlotClaim                                  slotClaim                        (PlayerTypes p);
	SlotStatus                                 slotStatus                       (PlayerTypes eID);
	unsigned int                               syncRandomSeed                   ();
	int                                        targetScore                      ();
	bool                                       transferredMap                   (); // setupddata
	TeamTypes                                  teamType                         (PlayerTypes p);
	const CvTurnTimerInfo &                    turnTimerInfo                    ();
	TurnTimerTypes                             turnTimer                        ();
	const std::string &                        versionString                    ();
	const std::vector<bool> &                  victories                        ();
	const CvWorldInfo &                        worldInfo                        ();
	WorldSizeTypes                             worldSize                        ();
	void                                       write                            (FDataStream & saveTo);
	int										 getActiveSlotCount				  ();
	int										 readActiveSlotCountFromSaveGame  (FDataStream & loadFrom, bool bReadVersion);

    extern const std::vector<TeamTypes>& sr_TeamTypes;
	static TeamTypes     teamType(PlayerTypes p)
	{
		if(p >= 0 && p < MAX_PLAYERS)
			return sr_TeamTypes[p];
		return NO_TEAM;
	}

};

#endif//_CvPreGame_H
