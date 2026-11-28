#pragma once
#ifndef INCLUDED_SCRIPTING_UID
#define INCLUDED_SCRIPTING_UID


namespace GlobalUIDSystem {

		
	typedef DWORD ttag_t;
	typedef DWORD index_t;
	typedef size_t serial_t;

	template<class T> struct TYPES;

	struct UID_RECORD {
		ttag_t type_tag;
		index_t index;
		serial_t serial_number;
		void* obj;
	};

	bool isShutdown();

	// Always check isValid() first, optionally isType() and finally call retrieve()
	struct UID {
		bool isValid() const { return !isShutdown() && record && record->serial_number == serial_number; }
		bool isType(ttag_t type_tag) const { return record->type_tag == type_tag; }
		void* retrieve() const { return record->obj; }
		index_t index() const { return record ? record->index : 0; }
		template<class T>
		static UID from(const T* obj) { return construct(obj ? obj->m_GlobalUID.data : NULL); }
	protected:
		static UID construct(UID_RECORD* rec) { UID ret(rec); return ret; }
		UID_RECORD* record;
		serial_t serial_number;
		UID(UID_RECORD* rec) : record((!rec || isShutdown()) ? NULL : rec), serial_number(rec ? rec->serial_number : 0) {}
	};

	struct UID_MEMBER_BASE {
		void releaseNow() { releaseUID(&data); }
	protected:
		friend struct UID;
		UID_MEMBER_BASE(void* obj, ttag_t type_tag) { acquireUID(&data, obj, type_tag); }
		~UID_MEMBER_BASE() { releaseNow(); }
		UID_MEMBER_BASE(const UID_MEMBER_BASE&);
		UID_MEMBER_BASE& operator=(const UID_MEMBER_BASE&);
		UID_RECORD* data;
		static void acquireUID(UID_RECORD**,void*,DWORD);
		static void releaseUID(UID_RECORD**);
	};

	template<class T>
	struct UID_MEMBER : UID_MEMBER_BASE {
		UID_MEMBER() : UID_MEMBER_BASE(owner(),TYPES<T>::ID) {} //TODO: RECONSIDER THIS
		UID_MEMBER(const UID_MEMBER&) : UID_MEMBER_BASE(owner(),TYPES<T>::ID) {}
		UID_MEMBER& operator=(const UID_MEMBER&) { return *this; }
	private:
		void* owner() { return ((char*)this) - offsetof(T, m_GlobalUID); }
	};
}

typedef GlobalUIDSystem::UID ScriptingUID;
typedef GlobalUIDSystem::ttag_t uid_ttag_t;
typedef GlobalUIDSystem::index_t uid_index_t;
typedef GlobalUIDSystem::serial_t uid_serial_t;

#define SCRIPTING_UID_MEMBER(CLASS) friend GlobalUIDSystem::UID_MEMBER<CLASS>; friend GlobalUIDSystem::UID; GlobalUIDSystem::UID_MEMBER<CLASS> m_GlobalUID
#define SCRIPTING_UID_INVALIDATE_NOW() m_GlobalUID.releaseNow()

#define DeclareType(TAG,NAME,CLASS) class CLASS; template<> struct GlobalUIDSystem::TYPES<CLASS> { static const ttag_t ID = TAG; };

DeclareType(0x41524541 /*AREA*/, Area, CvArea);
DeclareType(0x43495459 /*CITY*/, City, CvCity);
DeclareType(0x4445414C /*DEAL*/, Deal, CvDeal);
DeclareType(0x4C454147 /*LEAG*/, League, CvLeague);

DeclareType(0x504C5952 /*PLYR*/, PlayerBase, CvPlayer);
DeclareType(0x504C5952 /*PLYR*/, Player, CvPlayerAI);

DeclareType(0x504C4F54 /*PLOT*/, Plot, CvPlot);
DeclareType(0x5445414D /*TEAM*/, Team, CvTeam);
DeclareType(0x54455445 /*TETE*/, TeamTechs, CvTeamTechs);
DeclareType(0x554E4954 /*UNIT*/, Unit, CvUnit);

#undef DeclareType

#endif