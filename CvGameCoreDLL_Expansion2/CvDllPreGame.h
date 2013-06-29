/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "CvDllInterfaces.h"

class CvDllPreGame : public ICvPreGame2
{
public:
	CvDllPreGame();
	~CvDllPreGame();

	void* DLLCALL QueryInterface(GUID guidInterface);

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	/************************************************************************/
	/* ICvPreGame1 Interface Methods                                        */
	/************************************************************************/

	PlayerTypes DLLCALL activePlayer();
	int DLLCALL advancedStartPoints();
	bool DLLCALL autorun();
	float DLLCALL autorunTurnDelay();
	int DLLCALL autorunTurnLimit();
	CalendarTypes DLLCALL calendar();
	bool DLLCALL canReadyLocalPlayer();
	CvString DLLCALL civilizationAdjective(PlayerTypes p);
	const CvString DLLCALL civilizationAdjectiveKey(PlayerTypes p);
	CvString DLLCALL civilizationDescription(PlayerTypes p);
	const CvString DLLCALL civilizationDescriptionKey(PlayerTypes p);
	CivilizationTypes DLLCALL civilization(PlayerTypes p);
	const CvString DLLCALL civilizationPassword(PlayerTypes p);
	CvString DLLCALL civilizationShortDescription(PlayerTypes p);
	const CvString DLLCALL civilizationShortDescriptionKey(PlayerTypes p);
	const CvString DLLCALL civilizationKey(PlayerTypes p);
	bool DLLCALL civilizationKeyAvailable(PlayerTypes p);
	const GUID DLLCALL  civilizationKeyPackageID(PlayerTypes p);
	void DLLCALL clearDLCAllowed();
	ClimateTypes DLLCALL climate();
	void DLLCALL closeAllSlots();
	void DLLCALL closeInactiveSlots();
	const CvString DLLCALL emailAddress(PlayerTypes p);
	const CvString DLLCALL emailAddress();
	float DLLCALL endTurnTimerLength();
	EraTypes DLLCALL era();
	PlayerTypes DLLCALL findPlayerByNickname(const char* const name);
	GameMode DLLCALL gameMode();
	const CvString DLLCALL  gameName();
	bool DLLCALL gameStarted();
	uint DLLCALL GetGameOptionCount();
	bool DLLCALL GetGameOption(uint uiIndex, char* szOptionNameBuffer, uint nOptionNameBufferSize, int& iValue);
	bool DLLCALL GetGameOption(const char* szOptionName, int& iValue);
	bool DLLCALL GetGameOption(GameOptionTypes eOption, int& iValue);
	uint DLLCALL GetMapOptionCount();
	bool DLLCALL GetMapOption(uint uiIndex, char* szOptionNameBuffer, uint nOptionNameBuffersize, int& iValue);
	bool DLLCALL GetMapOption(const char* szOptionName, int& iValue);
	bool DLLCALL GetPersistSettings();
	GameSpeedTypes DLLCALL gameSpeed();
	int DLLCALL gameTurn();
	GameTypes DLLCALL gameType();
	GameStartTypes DLLCALL gameStartType();
	HandicapTypes DLLCALL handicap(PlayerTypes p);
	bool DLLCALL isDLCAllowed(const GUID& kDLCID);
	bool DLLCALL isDLCAvailable(PlayerTypes p, const GUID& kDLCID);
	bool DLLCALL isEarthMap();
	bool DLLCALL isHotSeat();
	bool DLLCALL isHotSeatGame();
	bool DLLCALL isHuman(PlayerTypes p);
	bool DLLCALL isInternetGame();
	bool DLLCALL isMinorCiv(PlayerTypes p);
	bool DLLCALL isNetworkMultiplayerGame();
	bool DLLCALL isPitBoss();
	bool DLLCALL isPlayable(PlayerTypes p);
	bool DLLCALL isPlayByEmail();
	bool DLLCALL isReady(PlayerTypes p);
	bool DLLCALL isVictory(VictoryTypes v);
	bool DLLCALL isWBMapScript();
	bool DLLCALL isWhiteFlag(PlayerTypes p);
	LeaderHeadTypes DLLCALL leaderHead(PlayerTypes p);
	CvString DLLCALL leaderName(PlayerTypes p);
	const CvString DLLCALL leaderNameKey(PlayerTypes p);
	const CvString DLLCALL leaderKey(PlayerTypes p);
	bool DLLCALL leaderKeyAvailable(PlayerTypes p);
	const GUID DLLCALL leaderKeyPackageID(PlayerTypes p);
	const CvString DLLCALL loadFileName();
	void DLLCALL loadFromIni(FIGameIniParser& iniParser);
	bool DLLCALL mapNoPlayers();
	unsigned int DLLCALL mapRandomSeed();
	bool DLLCALL loadWBScenario();
	bool DLLCALL overrideScenarioHandicap();
	const CvString DLLCALL mapScriptName();
	int DLLCALL maxCityElimination();
	int DLLCALL maxTurns();
	MinorCivTypes DLLCALL minorCivType(PlayerTypes p);
	bool DLLCALL multiplayerAIEnabled();
	int DLLCALL netID(PlayerTypes p);
	const CvString DLLCALL nickname(PlayerTypes p);
	const CvString DLLCALL nicknameDisplayed(PlayerTypes p);
	int DLLCALL numMinorCivs();
	PlayerColorTypes DLLCALL playerColor(PlayerTypes p);
	bool DLLCALL privateGame();
	bool DLLCALL quickCombat();
	bool DLLCALL quickMovement();
	bool DLLCALL quickstart();
	bool DLLCALL randomWorldSize();
	bool DLLCALL randomMapScript();
	void DLLCALL read(FDataStream& loadFrom, bool bReadVersion);
	bool DLLCALL readPlayerSlotInfo(FDataStream& loadFrom, bool bReadVersion);
	void DLLCALL resetGame();
	void DLLCALL ResetGameOptions();
	void DLLCALL ResetMapOptions();
	void DLLCALL resetPlayer(PlayerTypes p);
	void DLLCALL resetPlayers();
	void DLLCALL resetSlots();
	void DLLCALL restoreSlots();
	void DLLCALL saveSlots();
	SeaLevelTypes DLLCALL seaLevel();
	void DLLCALL setActivePlayer(PlayerTypes p);
	void DLLCALL setAdminPassword(const CvString& p);
	void DLLCALL setAdvancedStartPoints(int a);
	void DLLCALL setAlias(const CvString& a);
	void DLLCALL setAutorun(bool isAutoStart);
	void DLLCALL setAutorunTurnDelay(float turnDelay);
	void DLLCALL setAutorunTurnLimit(int turnLimit);
	void DLLCALL setBandwidth(BandwidthType b);
	void DLLCALL setBandwidth(const CvString& b);
	void DLLCALL setCalendar(CalendarTypes c);
	void DLLCALL setCalendar(const CvString& c);
	void DLLCALL setCivilization(PlayerTypes p, CivilizationTypes c);
	void DLLCALL setCivilizationAdjective(PlayerTypes p, const CvString& a);
	void DLLCALL setCivilizationDescription(PlayerTypes p, const CvString& d);
	void DLLCALL setCivilizationPassword(PlayerTypes p, const CvString& pwd);
	void DLLCALL setCivilizationShortDescription(PlayerTypes p, const CvString& d);
	void DLLCALL setCivilizationKey(PlayerTypes p, const CvString& d);
	void DLLCALL setCivilizationKeyPackageID(PlayerTypes p, const GUID& kKey);
	void DLLCALL setClimate(ClimateTypes c);
	void DLLCALL setClimate(const CvString& c);
	void DLLCALL setCustomWorldSize(int iWidth, int iHeight, int iPlayers = 0, int iMinorCivs = 0);
	void DLLCALL setDLCAllowed(const GUID& kDLCID, bool bState);
	void DLLCALL setEarthMap(bool bIsEarthMap);
	void DLLCALL setEmailAddress(PlayerTypes p, const CvString& a);
	void DLLCALL setEmailAddress(const CvString& a);
	void DLLCALL setEndTurnTimerLength(float f);
	void DLLCALL setEra(EraTypes e);
	void DLLCALL setEra(const CvString& e);
	void DLLCALL setGameMode(GameMode g);
	void DLLCALL setGameName(const CvString& g);
	void DLLCALL setGameSpeed(GameSpeedTypes g);
	void DLLCALL setGameSpeed(const CvString& g);
	void DLLCALL setGameStarted(bool);
	void DLLCALL setGameTurn(int turn);
	bool DLLCALL SetGameOption(const char* szOptionName, int iValue);
	bool DLLCALL SetGameOption(GameOptionTypes eOption, int iValue);
	void DLLCALL setGameType(GameTypes g);
	void DLLCALL setGameType(GameTypes g, GameStartTypes eStartType);
	void DLLCALL setGameType(const CvString& g);
	void DLLCALL setGameStartType(GameStartTypes g);
	void DLLCALL setGameUpdateTime(int updateTime);
	void DLLCALL setHandicap(PlayerTypes p, HandicapTypes h);
	void DLLCALL setInternetGame(bool isInternetGame);
	void DLLCALL setLeaderHead(PlayerTypes p, LeaderHeadTypes l);
	void DLLCALL setLeaderName(PlayerTypes p, const CvString& n);
	void DLLCALL setLeaderKey(PlayerTypes p, const CvString& n);
	void DLLCALL setLeaderKeyPackageID(PlayerTypes p, const GUID& n);
	void DLLCALL setLoadFileName(const CvString& fileName);
	void DLLCALL setMapNoPlayers(bool p);
	void DLLCALL setMapRandomSeed(unsigned int newSeed);
	void DLLCALL setLoadWBScenario(bool b);
	void DLLCALL setOverrideScenarioHandicap(bool b);
	void DLLCALL setMapScriptName(const CvString& s);
	bool DLLCALL SetMapOption(const char* szOptionName, int iValue);
	void DLLCALL setMaxCityElimination(int m);
	void DLLCALL setMaxTurns(int maxTurns);
	void DLLCALL setMinorCivType(PlayerTypes p, MinorCivTypes m);
	void DLLCALL setMinorCiv(PlayerTypes p, bool isMinor);
	void DLLCALL setMultiplayerAIEnabled(bool isEnabled);
	void DLLCALL setNetID(PlayerTypes p, int id);
	void DLLCALL setNickname(PlayerTypes p, const CvString& n);
	void DLLCALL setNumMinorCivs(int n);
	void DLLCALL setPersistSettings(bool bPersist);
	void DLLCALL setPlayable(PlayerTypes p, bool playable);
	void DLLCALL setPlayerColor(PlayerTypes p, PlayerColorTypes c);
	void DLLCALL setPrivateGame(bool isPrivateGame);
	void DLLCALL setQuickCombat(bool isQuickCombat);
	void DLLCALL setQuickMovement(bool isQuickMovement);
	void DLLCALL setQuickHandicap(HandicapTypes h);
	void DLLCALL setQuickHandicap(const CvString& h);
	void DLLCALL setQuickstart(bool isQuickStart);
	void DLLCALL setRandomWorldSize(bool isRandomWorldSize);
	void DLLCALL setRandomMapScript(bool isRandomWorldScript);
	void DLLCALL setReady(PlayerTypes p, bool bIsReady);
	void DLLCALL setSeaLevel(SeaLevelTypes s);
	void DLLCALL setSeaLevel(const CvString& s);
	void DLLCALL setSlotClaim(PlayerTypes p, SlotClaim c);
	void DLLCALL setSlotStatus(PlayerTypes eID, SlotStatus eSlotStatus);
	void DLLCALL setSyncRandomSeed(unsigned int newSeed);
	void DLLCALL setTeamType(PlayerTypes p, TeamTypes t);
	void DLLCALL setTransferredMap(bool transferred);
	void DLLCALL setTurnTimer(TurnTimerTypes t);
	void DLLCALL setTurnTimer(const CvString& t);
	void DLLCALL SetCityScreenBlocked(bool bCityScreenBlocked);
	void DLLCALL setVersionString(const std::string& v);
	void DLLCALL setVictory(VictoryTypes v, bool isValid);
	void DLLCALL setVictories(const std::vector<bool>& v);
	void DLLCALL setWhiteFlag(PlayerTypes p, bool flag);
	void DLLCALL setWorldSize(WorldSizeTypes w, bool bResetSlots=true);
	void DLLCALL setWorldSize(const CvString& w);
	SlotClaim DLLCALL slotClaim(PlayerTypes p);
	SlotStatus DLLCALL slotStatus(PlayerTypes eID);
	const CvString DLLCALL smtpHost();
	unsigned int DLLCALL syncRandomSeed();
	bool DLLCALL transferredMap();
	TeamTypes DLLCALL teamType(PlayerTypes p);
	TurnTimerTypes DLLCALL turnTimer();
	const std::string DLLCALL versionString();
	WorldSizeTypes DLLCALL worldSize();
	ICvWorldInfo1* DLLCALL GetWorldInfo();
	void DLLCALL write(FDataStream& saveTo);
	int DLLCALL getActiveSlotCount();
	int DLLCALL readActiveSlotCountFromSaveGame(FDataStream& loadFrom, bool bReadVersion);

	StorageLocation DLLCALL LoadFileStorage();
	void DLLCALL SetLoadFileName(_In_z_ const char* szFileName, StorageLocation eStorage);

	ICvEnumerator* DLLCALL GetDLCAllowed();
	ICvEnumerator* DLLCALL GetDLCAvailable(PlayerTypes p);
	void DLLCALL SetDLCAvailable(PlayerTypes p, ICvEnumerator* pList);

	/************************************************************************/
	/* ICvPreGame2 Interface Methods                                        */
	/************************************************************************/
	int DLLCALL pitBossTurnTime();
	void DLLCALL setPitBossTurnTime(int turnTime);

	bool DLLCALL isTurnNotifySteamInvite(PlayerTypes p) const;
	void DLLCALL setTurnNotifySteamInvite(PlayerTypes p, bool wantsSteamInvite);

	bool DLLCALL isTurnNotifyEmail(PlayerTypes p) const;
	void DLLCALL setTurnNotifyEmail(PlayerTypes p, bool wantsEmail);

	const CvString& DLLCALL getTurnNotifyEmailAddress(PlayerTypes p) const;
	void DLLCALL setTurnNotifyEmailAddress(PlayerTypes p, const CvString& emailAddress);

	void DLLCALL VerifyHandicap(PlayerTypes p);
	void DLLCALL ReseatConnectedPlayers();

private:
	void DLLCALL Destroy();
};


