#include "CvGameCoreDLLPCH.h"
#include "CvSaveController.h"

// must be included after all other headers
#include "LintFree.h"


enum AutoSaveModeTypes
{
	NO_AUTOSAVE = -1,

	AUTOSAVE_MODE_NONE,
	AUTOSAVE_MODE_INITIAL,
	AUTOSAVE_MODE_NORMAL,
	AUTOSAVE_MODE_POST,


	NUM_AUTOSAVE_MODE,
};


struct ManualSaveArgs
{
	ICvEngineScriptSystem1* pkScriptSystem;
	const char* filename;
	bool bSuccess;
};

namespace {
	int UnsafeUISave(lua_State* L) {

		ManualSaveArgs* pArgs = (ManualSaveArgs*)lua_touserdata(L, 1);

		pArgs->bSuccess = false;
		lua_getglobal(L, "UI");
		lua_getfield(L, -1, "SaveGame");
		lua_pushstring(L, pArgs->filename);
		lua_pcall(L, 1, 0, 0);

		pArgs->bSuccess = true;

		return 0;
	}


	bool ManualSave(const char* filename)
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		lua_State* l = pkScriptSystem->CreateLuaThread("Manual Save");

		ManualSaveArgs args;
		args.pkScriptSystem = pkScriptSystem;
		args.filename = filename;
		args.bSuccess = false;

		pkScriptSystem->CallCFunction(l, UnsafeUISave, &args);

		pkScriptSystem->FreeLuaThread(l);
		return args.bSuccess;
	}
}

CvSaveController::CvSaveController()
{
	for (int i = 0; i < NUM_AUTOSAVE_POINT; i++)
	{
		iLastTurnSaved[i] = -1;
		iTurnChecked[i] = -1;
	}

	m_bSkipFirstNetworkGameHumanTurnsStartSave = false;
	m_eSavedPoint = AUTOSAVE_POINT_EXTERNAL;
	m_eLastSavedPoint = NO_AUTOSAVE_POINT;

}

CvSaveController::~CvSaveController()
{
}

bool CvSaveController::SavePoint(AutoSavePointTypes eSavePoint) {
	if (iTurnChecked[eSavePoint] == GC.getGame().getGameTurn())	{
		return false;
	}
	else {
		iTurnChecked[eSavePoint] = GC.getGame().getGameTurn();
	}
	
	if (m_bSkipFirstNetworkGameHumanTurnsStartSave && eSavePoint == AUTOSAVE_POINT_NETWORK_GAME_TURN_POST)
	{
		m_bSkipFirstNetworkGameHumanTurnsStartSave = false;
		return false;
	}
	
	if (iLastTurnSaved[eSavePoint] == GC.getGame().getGameTurn())
	{
		return false;
	}
	
	bool isInitial = false;
	bool isPost = false;
	switch (eSavePoint)
	{

		case AUTOSAVE_POINT_MAP_GEN:
		case AUTOSAVE_POINT_INITIAL:
			isInitial = true;	
			isPost = false;
			break;

		case AUTOSAVE_POINT_LOCAL_GAME_TURN:
		case AUTOSAVE_POINT_NETWORK_GAME_TURN:
			isInitial = false;
			isPost = false;
			break;

		case AUTOSAVE_POINT_LOCAL_GAME_TURN_POST:
		case AUTOSAVE_POINT_NETWORK_GAME_TURN_POST:
			isInitial = false;
			isPost = true;
			break;
	
	}
	bool bPostAutoSavesDefault = GC.getGame().isOption(GAMEOPTION_POST_AUTOSAVES);
	AutoSave(eSavePoint, isInitial || !isPost || bPostAutoSavesDefault, isInitial, isPost);
	
	return true;

}

AutoSavePointTypes CvSaveController::getLastAutoSavePoint() const
{
	return m_eLastSavedPoint;
}

int CvSaveController::getLastAutoSaveTurn() const
{
	if (m_eLastSavedPoint == NO_AUTOSAVE_POINT)
		return -1;
	return iLastTurnSaved[m_eLastSavedPoint];
}

bool CvSaveController::AutoSave(AutoSavePointTypes eSavePoint, bool default, bool initial, bool post)
{
	bool save = initial;

	if(!save)
		save = FireWantAutoSaveEvent(eSavePoint, default);
	if (save) {
		m_eSavedPoint = eSavePoint;					
		gDLL->AutoSave(initial, post);
		m_eSavedPoint = AUTOSAVE_POINT_EXTERNAL;
		m_eLastSavedPoint = eSavePoint;

		iLastTurnSaved[eSavePoint] = GC.getGame().getGameTurn();	
	}
	FireAutoSaveEvent(eSavePoint, save); // sending whether saved or not because it might be useful information to listeners and allow more functionality, especially since listening to the "want" message has side effects
	return save;
}

bool CvSaveController::FireWantAutoSaveEvent(AutoSavePointTypes eSavePoint, bool default)
{
	int result =  GAMEEVENTINVOKE_TESTANY(GAMEEVENT_WantAutoSave, eSavePoint);
	bool save = false;
	
	if (result == GAMEEVENTRETURN_TRUE) save = true;
	else if (result == GAMEEVENTRETURN_FALSE) save = false;
	else save = default;

	return save;
}

void CvSaveController::FireAutoSaveEvent(AutoSavePointTypes eSavePoint, bool saved)
{
	GAMEEVENTINVOKE_HOOK(GAMEEVENT_AutoSaved, eSavePoint, saved);
}

void CvSaveController::Save(const char* filename)
{
	NamedSave(filename, AUTOSAVE_POINT_EXPLICIT);
}

void CvSaveController::NamedSave(const char* filename, AutoSavePointTypes type) {
	m_eSavedPoint = type;
	ManualSave(filename);
	m_eSavedPoint = AUTOSAVE_POINT_EXTERNAL;
	FireAutoSaveEvent(type, true);

}

void CvSaveController::Read(FDataStream& kStream)
{
	int p;
	kStream >> p;
	AutoSavePointTypes eLoadedSavePoint = (AutoSavePointTypes)p;
	if (eLoadedSavePoint == AUTOSAVE_POINT_NETWORK_GAME_TURN_POST)
		m_bSkipFirstNetworkGameHumanTurnsStartSave = true;

}

void CvSaveController::Write(FDataStream& kStream) const
{
	kStream << (int)m_eSavedPoint;
}

FDataStream& operator>>(FDataStream& kStream, CvSaveController& kAutoSave)
{
	kAutoSave.Read(kStream);
	return kStream;
}
FDataStream& operator<<(FDataStream& kStream, const CvSaveController& kAutoSave)
{
	kAutoSave.Write(kStream);
	return kStream;
}