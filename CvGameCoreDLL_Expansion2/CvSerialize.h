#pragma once

#ifndef CVSERIALIZE_H
#define CVSERIALIZE_H

#include "CvAssert.h"

#include "CvEnums.h"
#include "CvEnumSerialization.h"

#include "CvGameCoreEnums.h"
#include "CvGameCoreEnumSerialization.h"

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

	template<typename InfoType, typename T>
	inline void infoHash(const T& value, bool* bValid = NULL)
	{
		CvInfosSerializationHelper::WriteHashed(m_stream, InfoType(value));
		if (bValid != NULL) { *bValid = true; }
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
		CvAssertMsg(false, "CvStreamSaveVisitor is not meant for loading");
		return *this;
	}

	inline bool isSaving() const
	{
		return true;
	}

	inline bool isLoading() const
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
		CvAssertMsg(false, "CvStreamSaveVisitor is not meant for loading");
	}

	template<typename T>
	void loadIgnore()
	{
		CvAssertMsg(false, "CvStreamSaveVisitor is not meant for loading");
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

	template<typename InfoType, typename T>
	inline void infoHash(T& value, bool* bValid = NULL)
	{
		value = static_cast<T>(CvInfosSerializationHelper::ReadHashed(m_stream, bValid));
	}

	template<typename T>
	inline CvStreamLoadVisitor& operator<<(const T& value)
	{
		CvAssertMsg(false, "CvStreamSaveVisitor is not meant for saving");
		return *this;
	}

	template<typename T>
	inline CvStreamLoadVisitor& operator>>(T& value)
	{
		m_stream >> value;
		return *this;
	}

	inline bool isSaving() const
	{
		return false;
	}

	inline bool isLoading() const
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

template<typename T>
class CvSyncVarBase : public ICvSyncVar
{
public:
	typedef T ValueType;

	CvSyncVarBase(const std::string& name, FAutoArchive& owner, const ValueType& value)
		: ICvSyncVar(name, owner)
		, m_prevValue(value)
	{}

	inline const ValueType& previousValue() const
	{
		return m_prevValue;
	}

	inline ValueType& previousValue()
	{
		return m_prevValue;
	}

private:
	// This contains the variable's last known value at the time of the most recent
	// synchronization event. It is compared againstthe most recent value to determine 
	// if we should send it.
	ValueType m_prevValue;
};

template<typename VarTraits>
class CvSyncVar : public CvSyncVarBase<typename VarTraits::ValueType>
{
public:
	typedef typename VarTraits::ValueType ValueType;
	typedef typename VarTraits::ContainerType ContainerType;
	typedef typename VarTraits::SyncVarsType SyncVarsType;

	CvSyncVar()
		: CvSyncVarBase<ValueType>(EmptyString, getContainer().getSyncArchive(), currentValue())
	{}
	~CvSyncVar() {}

	inline const SyncVarsType& getStorage() const
	{
		return VarTraits::GetStorage(*this);
	}
	inline SyncVarsType& getStorage() 
	{
		return VarTraits::GetStorage(*this);
	}

	inline const ContainerType& getContainer() const
	{
		return getStorage().getContainer();
	}
	inline ContainerType& getContainer()
	{
		return getStorage().getContainer();
	}

	inline const ValueType& currentValue() const
	{
		return VarTraits::Get(getContainer());
	}
	inline ValueType& currentValue()
	{
		return VarTraits::Get(getContainer());
	}

	inline void updateValue()
	{
		this->previousValue() = currentValue();
	}

	// ICvSyncVar Interface
	virtual bool hasDelta() const
	{
		return this->previousValue() != currentValue();
	}

	// FAutoVariableBase interface
	virtual void load(FDataStream& loadFrom)
	{
		loadFrom >> currentValue();
	}
	virtual void loadDelta(FDataStream& loadFrom)
	{
		load(loadFrom);
	}
	virtual void save(FDataStream& saveTo) const
	{
		saveTo << currentValue();
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
		const bool result = other == currentValue();
		return result; // Place a conditional breakpoint here to help debug sync errors.
	}
	virtual void reset()
	{
		currentValue() = this->previousValue() = ValueType();
	}
	virtual const std::string& name() const
	{
		return EmptyString;
	}
	virtual void setStackTraceRemark()
	{
	}
	virtual std::string debugDump(const std::vector<std::pair<std::string, std::string> >& callStacks) const
	{
		std::string result = FAutoVariableBase::debugDump(callStacks);
		result += std::string("\n") + getContainer().debugDump(*this) + std::string("\n");
#if !defined(FINAL_RELEASE)
		result += std::string("local value=") + FSerialization::toString(m_value) + "\n";
		result += std::string("remote value=") + FSerialization::toString(m_remoteValue) + "\n";
#endif
		return result;
	}
	virtual std::string toString() const
	{
		return FSerialization::toString(currentValue());
	}
};

// This is intentionally outside of CvSyncArchive so that the compiler generates only one code path
void CvSyncArchiveCollectDeltas(FAutoArchive& syncArchive, const std::vector<FAutoVariableBase*>& vars);

template<typename Container>
class CvSyncArchive : public FAutoArchive
{
public:
	typedef typename Container ContainerType;
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
		CvAssert(m_syncVarsStorage == NULL);
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
private: \
	ContainerType& m_container; \
public:

// Automagically creates a traits entry for a CvSyncVar
#define SYNC_ARCHIVE_VAR(memberType, memberName) \
	struct memberName##_Traits { \
		enum { INDEX = SYNC_ARCHIVE_VAR_NEXT_INDEX }; \
		typedef memberType ValueType; \
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

// Automagically ends a CvSyncArchive specialization
#define SYNC_ARCHIVE_END() };

// Expands into what you pass it
// Useful if you need to pass multiple template parameters since the preprocessor wont cooperate otherwise
// Feel free to move this to another file and give it a better name
#define SYNC_ARCHIVE_VAR_TYPE(...) __VA_ARGS__

#endif CVSERIALIZE_H
