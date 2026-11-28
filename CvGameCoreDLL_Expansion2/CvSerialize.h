#pragma once

#ifndef CVSERIALIZE_H
#define CVSERIALIZE_H

#include "CvAssert.h"

#include "CvEnums.h"
#include "CvEnumSerialization.h"

#include "CvGameCoreEnums.h"
#include "CvGameCoreEnumSerialization.h"

#include "CvSpanSerialization.h"

#include <array>

// Include for debug logging functionality
#include <sstream>
#include "CvGlobals.h"

// Shared empty string
extern const std::string EmptyString;

// A serialize visitor that saves items it visits into a stream
class CvStreamSaveVisitor
{
public:
	CvStreamSaveVisitor(FDataStream& stream)
		: m_stream(stream)
	{}

	template<typename T>
	inline void operator()(const T& value)
	{
		m_stream << value;
	}

	template<typename To, typename From>
	inline void as(const From& value)
	{
		m_stream << static_cast<To>(value);
	}

	template<typename T>
	inline CvStreamSaveVisitor& operator<<(const T& value)
	{
		m_stream << value;
		return *this;
	}

	template<typename T>
	inline CvStreamSaveVisitor& operator>>(const T& value)
	{
		ASSERT(false, "CvStreamSaveVisitor is not meant for loading");
		return *this;
	}

	static inline bool isSaving() 
	{
		return true;
	}

	static inline bool isLoading() 
	{
		return false;
	}

	inline const FDataStream& stream() const
	{
		return m_stream;
	}
	inline FDataStream& stream()
	{
		return m_stream;
	}

	template<typename Dst, typename Src>
	inline void loadAssign(Dst& dst, const Src& src)
	{
		ASSERT(false, "CvStreamSaveVisitor is not meant for loading");
	}

	template<typename T>
	void loadIgnore()
	{
		ASSERT(false, "CvStreamSaveVisitor is not meant for loading");
	}

private:
	FDataStream& m_stream;
};

// A serialize visitor that loads items it visits from a stream
class CvStreamLoadVisitor
{
public:
	CvStreamLoadVisitor(FDataStream& stream)
		: m_stream(stream)
	{}

	template<typename T>
	inline void operator()(T& value)
	{
		m_stream >> value;
	}

	template<typename To, typename From>
	inline void as(From& value)
	{
		To to;
		m_stream >> to;
		value = static_cast<From>(to);
	}

	template<typename T>
	inline CvStreamLoadVisitor& operator<<(const T& value)
	{
		ASSERT(false, "CvStreamLoadVisitor is not meant for saving");
		return *this;
	}

	template<typename T>
	inline CvStreamLoadVisitor& operator>>(T& value)
	{
		m_stream >> value;
		return *this;
	}

	static inline bool isSaving() 
	{
		return false;
	}

	static inline bool isLoading() 
	{
		return true;
	}

	inline const FDataStream& stream() const
	{
		return m_stream;
	}
	inline FDataStream& stream()
	{
		return m_stream;
	}

	template<typename Dst, typename Src>
	inline void loadAssign(Dst& dst, const Src& src)
	{
		dst = src;
	}

	template<typename T>
	void loadIgnore()
	{
		T x;
		m_stream >> x;
	}

private:
	FDataStream& m_stream;
};

class ICvSyncVar : public FAutoVariableBase
{
public:
	ICvSyncVar(const std::string& name, FAutoArchive& owner)
		: FAutoVariableBase(name, owner)
	{}
	virtual bool hasDelta() const = 0;
};

// These are intentionally outside of CvSyncVar so that they are compiled once, not once per sync var.
// MSVC v90 limits the precompiled header size (/Zm), and every member body of a class that exists once per sync var
// (~1300 of them) is compiled into it once per sync var. Keep such code out of CvSyncVar.

// Logs a sync var mismatch to net_message_debug.log and marks the game as desynced.
void CvSyncVarLogDesync(const ICvSyncVar& var, const std::string& remoteValue, const std::string& containerContext);
// Builds the CvSyncVar::debugDump() text. remoteValue is NULL when the remote value is not tracked.
std::string CvSyncVarDebugDump(const ICvSyncVar& var, const std::vector<std::pair<std::string, std::string> >& callStacks, const std::string& containerContext, const std::string* remoteValue);

// The container's description of a sync var for desync logs (CvCity::debugDump etc.), once per container type.
template<typename Container>
std::string CvSyncVarContainerDump(const void* container, const FAutoVariableBase& var)
{
	return static_cast<const Container*>(container)->debugDump(var);
}

// One string per distinct sync var name, shared by every unit, city and player (a copy per object costs 28 bytes plus a
// heap block for names longer than 15 characters, for each of their ~350-570 sync vars). Called when a sync var is
// constructed, on the game thread; the string stays at its address for the rest of the process.
const std::string& CvSyncVarSharedName(const char* name);

// The sync var's name, as a base class declared before ICvSyncVar: bases are built in declaration order, so the name
// exists before FAutoVariableBase receives it, and it keeps that address for the sync var's lifetime.
struct CvSyncVarName
{
	explicit CvSyncVarName(const char* name)
		: m_pName(&CvSyncVarSharedName(name))
	{}
	const std::string* m_pName;
};

// Everything a sync var does, once per value type (~50 types) instead of once per sync var. It refers to the synced
// member of the container directly; CvSyncVar below only finds that member and the container.
template<typename T>
class CvSyncVarBase : private CvSyncVarName, public ICvSyncVar
{
public:
	typedef T ValueType;
	typedef std::string (*ContainerDumpFn)(const void* container, const FAutoVariableBase& var);

	CvSyncVarBase(const char* name, FAutoArchive& owner, ValueType& value, const void* container, ContainerDumpFn containerDump,
		bool bApplyRemote)
		: CvSyncVarName(name)
		, ICvSyncVar(*m_pName, owner)
		, m_prevValue(value)
		, m_value(value)
		, m_container(container)
		, m_containerDump(containerDump)
		, m_bApplyRemote(bApplyRemote)
	{}

	inline const ValueType& previousValue() const
	{
		return m_prevValue;
	}
	inline ValueType& previousValue()
	{
		return m_prevValue;
	}
	inline const ValueType& currentValue() const
	{
		return m_value;
	}
	inline ValueType& currentValue()
	{
		return m_value;
	}
	inline void updateValue()
	{
		m_prevValue = m_value;
	}

	// ICvSyncVar Interface
	virtual bool hasDelta() const
	{
		return m_prevValue != m_value;
	}

	// FAutoVariableBase interface
	virtual void load(FDataStream& loadFrom)
	{
		loadFrom >> m_value;
	}
	virtual void loadDelta(FDataStream& loadFrom)
	{
		// Called for a received sync check: take the sender's value, unless the var is compare-only
		if (m_bApplyRemote)
		{
			load(loadFrom);
		}
		else
		{
			ValueType remoteValue;
			loadFrom >> remoteValue;
		}
	}
	virtual void save(FDataStream& saveTo) const
	{
		saveTo << m_value;
	}
	virtual void saveDelta(FDataStream& saveTo) const
	{
		save(saveTo);
	}
	virtual void clearDelta()
	{
		updateValue();
	}
	virtual bool compare(FDataStream& otherValue) const
	{
		ValueType other;
		otherValue >> other;
#if defined(VPDEBUG)
		m_remoteValue = other;
#endif
		const bool result = other == m_value;
		if (!result)
		{
			CvSyncVarLogDesync(*this, FSerialization::toString(other), m_containerDump(m_container, *this));
		}

		return result; // Place a conditional breakpoint here to help debug sync errors.
	}
	virtual void reset()
	{
		m_value = m_prevValue = ValueType();
	}
	virtual const std::string& name() const
	{
		return *m_pName;
	}
	virtual void setStackTraceRemark()
	{
	}
	virtual std::string debugDump(const std::vector<std::pair<std::string, std::string> >& callStacks) const
	{
#if !defined(FINAL_RELEASE) || defined(VPDEBUG)
		const std::string remoteValue = FSerialization::toString(m_remoteValue);
		return CvSyncVarDebugDump(*this, callStacks, m_containerDump(m_container, *this), &remoteValue);
#else
		return CvSyncVarDebugDump(*this, callStacks, m_containerDump(m_container, *this), NULL);
#endif
	}
	virtual std::string toString() const
	{
		return FSerialization::toString(m_value);
	}

private:
	// This contains the variable's last known value at the time of the most recent
	// synchronization event. It is compared against the most recent value to determine
	// if we should send it.
	ValueType m_prevValue;
	ValueType& m_value;
	const void* m_container;
	ContainerDumpFn m_containerDump;
	// false for SYNC_ARCHIVE_VAR_COMPARE_ONLY: a received sync check reads the sender's value but does not write it
	bool m_bApplyRemote;
#if defined(VPDEBUG)
	mutable ValueType m_remoteValue;
#endif
};

// One per sync var: only finds the synced member and its container (VarTraits, from SYNC_ARCHIVE_VAR).
template<typename VarTraits>
class CvSyncVar : public CvSyncVarBase<typename VarTraits::ValueType>
{
public:
	typedef typename VarTraits::ValueType ValueType;
	typedef typename VarTraits::ContainerType ContainerType;

	CvSyncVar()
		: CvSyncVarBase<ValueType>(VarTraits::name(), container().getSyncArchive(), VarTraits::Get(container()), &container(),
			&CvSyncVarContainerDump<ContainerType>,
			VarTraits::SyncVarsType::AppliesRemoteValue(static_cast<const VarTraits*>(NULL)))
	{}
	// User-declared, as before: an implicit virtual destructor is defined wherever the class is complete, which makes clang
	// instantiate every CvSyncVarBase<T> virtual in every file that includes the header.
	~CvSyncVar() {}

private:
	inline ContainerType& container()
	{
		return VarTraits::GetStorage(*this).getContainer();
	}
};

// This is intentionally outside of CvSyncArchive so that the compiler generates only one code path
void CvSyncArchiveCollectDeltas(FAutoArchive& syncArchive, const std::vector<FAutoVariableBase*>& vars);

template<typename Container>
class CvSyncArchive : public FAutoArchive
{
public:
	typedef Container ContainerType;
	friend Container;
	class SyncVars;

private:
	CvSyncArchive()
		: m_syncVarsStorage(NULL)
	{}

public:
	~CvSyncArchive()
	{
		destroySyncVars();
	}

	const ContainerType& getContainer() const;
	ContainerType& getContainer();

	// Call once at initialization in multiplayer
	inline void initSyncVars(SyncVars& syncVars)
	{
		m_syncVarsStorage = &syncVars;
	}
	inline void destroySyncVars()
	{
		SAFE_DELETE(m_syncVarsStorage);
	}
	inline const std::vector<FAutoVariableBase*>& getContents() const
	{
		return m_contents;
	}
	inline std::vector<FAutoVariableBase*>& getContents()
	{
		return m_contents;
	}
	inline void collectDeltas()
	{
		CvSyncArchiveCollectDeltas(*this, getContents());
	}

	// FAutoArchive Interface
	virtual const std::string* getVariableName(const FAutoVariableBase&) const
	{
		return NULL;
	}
	virtual void setVariableName(const FAutoVariableBase&, const std::string&) const
	{
	}
	virtual std::string debugDump(const FAutoVariableBase& var) const
	{
		return std::string("\n") + getContainer().debugDump(var);
	}
	virtual std::string stackTraceRemark(const FAutoVariableBase& var) const
	{
		return getContainer().stackTraceRemark(var);
	}
private:
	SyncVars* m_syncVarsStorage;
};

// Gets the next index for a sync var
// Useful if you don't want to use the SYNC_ARCHIVE_VAR macro!
#define SYNC_ARCHIVE_VAR_NEXT_INDEX (__COUNTER__ - INIT_VAR_INDEX_)

// Helper to declare a CvSyncArchive member for a type
#define SYNC_ARCHIVE_MEMBER(containerType) \
	friend CvSyncArchive<containerType>; \
	CvSyncArchive<containerType> m_syncArchive;

// Automagically begins a CvSyncArchive specialization
#define SYNC_ARCHIVE_BEGIN(containerType) \
template<> inline const containerType& CvSyncArchive<containerType>::getContainer() const { \
	return *reinterpret_cast<const containerType*>(reinterpret_cast<const char*>(this) - offsetof(containerType, m_syncArchive)); \
} \
template<> inline containerType& CvSyncArchive<containerType>::getContainer() { \
	return const_cast<containerType&>(const_cast<const CvSyncArchive<containerType>*>(this)->getContainer()); \
} \
template<> class CvSyncArchive<containerType>::SyncVars { \
	enum { INIT_VAR_INDEX_ = (__COUNTER__ + 1) }; \
public: \
	typedef containerType ContainerType; \
	const ContainerType& getContainer() const { return m_container; } \
	ContainerType& getContainer() { return m_container; } \
	SyncVars(ContainerType& container) : m_container(container) {} \
	template<typename VarTraits> static inline bool AppliesRemoteValue(const VarTraits*) { return true; } \
private: \
	ContainerType& m_container; \
public:

// Automagically creates a traits entry for a CvSyncVar
#define SYNC_ARCHIVE_VAR(memberType, memberName) \
	struct memberName##_Traits { \
		enum { INDEX = SYNC_ARCHIVE_VAR_NEXT_INDEX }; \
		typedef memberType ValueType; \
		static inline const char* name() { return #memberName; } \
		SYNC_ARCHIVE_VAR_TRAITS_TAIL_(memberName)

// Same as SYNC_ARCHIVE_VAR, but the sync var reports varName as its name() instead of the member name.
// Use it only where the game identifies a sync var by name.
#define SYNC_ARCHIVE_VAR_NAMED(memberType, memberName, varName) \
	struct memberName##_Traits { \
		enum { INDEX = SYNC_ARCHIVE_VAR_NEXT_INDEX }; \
		typedef memberType ValueType; \
		static inline const char* name() { return varName; } \
		SYNC_ARCHIVE_VAR_TRAITS_TAIL_(memberName)

// Rest of a SYNC_ARCHIVE_VAR traits entry and the member. memberType stays in the macros above, so a type
// with commas (SYNC_ARCHIVE_VAR_TYPE) is never passed on to another macro.
#define SYNC_ARCHIVE_VAR_TRAITS_TAIL_(memberName) \
		typedef ContainerType ContainerType; \
		typedef CvSyncArchive<ContainerType>::SyncVars SyncVarsType; \
		static inline const ValueType& Get(const ContainerType& container) { return container.memberName; } \
		static inline ValueType& Get(ContainerType& container) { return container.memberName; } \
		static inline const SyncVarsType& GetStorage(const CvSyncVar<memberName##_Traits>& var) { \
			return *reinterpret_cast<const SyncVarsType*>(reinterpret_cast<const char*>(&var) - offsetof(SyncVarsType, memberName)); \
		} \
		static inline SyncVarsType& GetStorage(CvSyncVar<memberName##_Traits>& var) { \
			return const_cast<SyncVarsType&>(GetStorage(const_cast<const CvSyncVar<memberName##_Traits>&>(var))); \
		} \
	}; \
	CvSyncVar<memberName##_Traits> memberName;

// Same as SYNC_ARCHIVE_VAR, but a received sync check only compares the value and does not write it.
// Use it for counters that the game keeps up to date from other state which is not in the sync check
// (worked plots, production queues, city-state friendships...): a written value would no longer match
// that state, and the difference would stay when the state itself agrees again.
#define SYNC_ARCHIVE_VAR_COMPARE_ONLY(memberType, memberName) \
	SYNC_ARCHIVE_VAR(memberType, memberName) \
	static inline bool AppliesRemoteValue(const memberName##_Traits*) { return false; }

// Automagically ends a CvSyncArchive specialization
#define SYNC_ARCHIVE_END() };

// Expands into what you pass it
// Useful if you need to pass multiple template parameters since the preprocessor wont cooperate otherwise
// Feel free to move this to another file and give it a better name
#define SYNC_ARCHIVE_VAR_TYPE(...) __VA_ARGS__

template <typename T, std::size_t N>
FDataStream& operator<<(FDataStream& saveTo, const tr1::array<T, N>& readFrom) {
	const CvSpan<const T, N> span(readFrom.begin(), readFrom.end());
	return saveTo << span;
}
template <typename T, std::size_t N>
FDataStream& operator>>(FDataStream& loadFrom, tr1::array<T, N>& writeTo) {
	const CvSpan<T, N> span(writeTo.begin(), writeTo.end());
	return loadFrom >> span;
}


#endif // CVSERIALIZE_H
