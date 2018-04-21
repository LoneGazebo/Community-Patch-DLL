#pragma once

#ifndef CV_AUTOSAVE2_H
#define CV_AUTOSAVE2_H

#include <string>

enum AutoSavePointTypes
{
	NO_AUTOSAVE_POINT = -1,

	AUTOSAVE_POINT_EXTERNAL,

	AUTOSAVE_POINT_EXPLICIT,

	AUTOSAVE_POINT_MAP_GEN,
	AUTOSAVE_POINT_INITIAL,

	AUTOSAVE_POINT_LOCAL_GAME_TURN,
	AUTOSAVE_POINT_NETWORK_GAME_TURN,

	AUTOSAVE_POINT_LOCAL_GAME_TURN_POST,
	AUTOSAVE_POINT_NETWORK_GAME_TURN_POST,

	NUM_AUTOSAVE_POINT,
};

class CvSaveController
{
public:
	CvSaveController();
	virtual ~CvSaveController();

	bool SavePoint(AutoSavePointTypes eSavePoint);
	
	AutoSavePointTypes getLastAutoSavePoint() const;
	int getLastAutoSaveTurn() const;

	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	// Save games using this method are not guaranteed to be in a good state since they could be made at any point during processing
	// Provided for developement and debugging purposes...and for the adventurous!
	void Save(const char* filename);


protected:
	bool AutoSave(AutoSavePointTypes eSavePoint, bool default, bool initial, bool post);

	void NamedSave(const char* filename, AutoSavePointTypes type);

	bool FireWantAutoSaveEvent(AutoSavePointTypes eSavePoint, bool default);
	void FireAutoSaveEvent(AutoSavePointTypes eSavePoint, bool saved);

	int iLastTurnSaved[NUM_AUTOSAVE_POINT];
	int iTurnChecked[NUM_AUTOSAVE_POINT];

	AutoSavePointTypes m_eSavedPoint;
	AutoSavePointTypes  m_eLastSavedPoint;

	bool m_bSkipFirstNetworkGameHumanTurnsStartSave;	
};

FDataStream& operator>>(FDataStream&, CvSaveController&);
FDataStream& operator<<(FDataStream&, const CvSaveController&);

#endif
