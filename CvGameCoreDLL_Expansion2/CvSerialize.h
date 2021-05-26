#pragma once

#ifndef CVSERIALIZE_H
#define CVSERIALIZE_H

// Shared empty string
extern const std::string EmptyString;

// Shared behavior for CvSyncVar specializations
template<typename SyncTraits, typename T>
class CvSyncVarBase
{
public:
	typedef SyncTraits Traits;
	typedef typename Traits::SyncArchive SyncArchive;
	typedef typename Traits::ValueType ValueType;
	enum
	{
		INDEX = Traits::INDEX,
		SAVE = Traits::SAVE,
	};

	inline const ValueType& get() const
	{
		return m_value;
	}

	inline ValueType& dirtyGet(SyncArchive& syncArchive)
	{
		markDirty(syncArchive);
		return m_value;
	}

	inline ValueType& unsafeGet()
	{
		return m_value;
	}

	inline void set(SyncArchive& syncArchive, const ValueType& value)
	{
		markDirty(syncArchive);
		m_value = value;
	}

	inline operator const ValueType&() const
	{
		return m_value;
	}

	inline void markDirty(SyncArchive& syncArchive)
	{
		const std::vector<FAutoVariableBase*>& autoVars = syncArchive.getAutoVars();
		if (!autoVars.empty())
		{
			FAssert(Traits::INDEX < autoVars.size());
			syncArchive.touch(*autoVars[Traits::INDEX]);
		}
	}

protected:
	inline CvSyncVarBase() : m_value() {}
	inline CvSyncVarBase(const ValueType& value) : m_value(value) {}

	ValueType m_value;
};

// Holds a value that should maintain synchronization
// Please do not specify the second template parameter! It is used for specialization deduction.
template<typename SyncTraits, typename T = typename SyncTraits::ValueType>
class CvSyncVar : public CvSyncVarBase<SyncTraits, T>
{
public:
	inline CvSyncVar() {}
	inline CvSyncVar(const ValueType& value)
		: CvSyncVarBase<SyncTraits, T>(value)
	{}
};

// Specialization for std::vector
template<typename SyncTraits, typename T, typename Alloc>
class CvSyncVar<SyncTraits, std::vector<T, Alloc>> : public CvSyncVarBase<SyncTraits, std::vector<T, Alloc>>
{
public:
	typedef typename CvSyncVarBase<SyncTraits, std::vector<T, Alloc>>::ValueType ValueType;
	typedef typename CvSyncVarBase<SyncTraits, std::vector<T, Alloc>>::SyncArchive SyncArchive;
	typedef typename std::vector<T, Alloc>::const_iterator const_iterator;
	typedef typename std::vector<T, Alloc>::const_reference const_reference;

	inline CvSyncVar() {}
	inline CvSyncVar(const ValueType& value)
		: CvSyncVarBase<SyncTraits, T>(value)
	{}

	inline void setAt(SyncArchive& syncArchive, size_t i, const_reference value)
	{
		markDirty(syncArchive);
		this->m_value[i] = value;
	}

	inline const_reference getAt(size_t i) const
	{
		return this->m_value[i];
	}

	inline const_reference operator[](size_t i) const
	{
		return this->m_value[i];
	}

	inline size_t size() const { return this->m_value.size(); }
	inline const_iterator begin() const { return this->m_value.begin(); }
	inline const_iterator end() const { return this->m_value.end(); }

	inline void push_back(SyncArchive& syncArchive, const_reference src)
	{
		dirtyGet(syncArchive).push_back(src);
	}
	inline void erase(SyncArchive& syncArchive, size_t i)
	{
		dirtyGet(syncArchive).erase(i);
	}
	inline void insert(SyncArchive& syncArchive, size_t i, const_reference src)
	{
		dirtyGet(syncArchive).insert(i, src);
	}
	inline void clear(SyncArchive& syncArchive)
	{
		dirtyGet(syncArchive).clear();
	}
	inline void resize(SyncArchive& syncArchive, size_t n)
	{
		dirtyGet(syncArchive).resize(n);
	}
	inline void resize(SyncArchive& syncArchive, size_t n, const_reference v)
	{
		dirtyGet(syncArchive).resize(n, v);
	}
	inline void reserve(SyncArchive& syncArchive, size_t n)
	{
		dirtyGet(syncArchive).reserve(n);
	}
};

// Write CvSyncVar's value to a FDataStream
template<typename SyncTraits>
inline FDataStream& operator<<(FDataStream& stream, const CvSyncVar<SyncTraits>& syncVar)
{
	return stream << syncVar.get();
}

// Contains synchronization meta-data
template<typename T>
class CvSyncArchive;

// Utility for visiting var traits of a CvSyncArchive
template<typename SyncArchive, typename Visitor, size_t N, size_t VarCount>
struct CvSyncArchiveVarsVisit
{
	static __forceinline void Step(Visitor& visitor)
	{
		typedef typename SyncArchive::SyncVars SyncVars;
		typedef typename SyncVars::template VarTraits<N>::Type Traits;
		if (visitor(Traits()))
		{
			CvSyncArchiveVarsVisit<SyncArchive, Visitor, N + 1, VarCount>::Step(visitor);
		}
	}
};

// Utility for visiting var traits of a CvSyncArchive
template<typename SyncArchive, typename Visitor, size_t N>
struct CvSyncArchiveVarsVisit<SyncArchive, Visitor, N, N>
{
	static __forceinline void Step(Visitor& visitor)
	{
		// Intentionally empty to terminate recursion
	}
};

template<typename T>
class CvSyncArchiveBase;

template<typename Container>
class CvSyncArchiveBase<CvSyncArchive<Container>> : public FAutoArchive
{
	typedef CvSyncArchive<Container> Derived;

	struct WriteVisitor
	{
		inline WriteVisitor(FDataStream& stream, const Container& container)
			: stream(stream)
			, container(container)
		{}

		template<typename VarTraits>
		__forceinline bool operator()(VarTraits)
		{
			if (VarTraits::SAVE)
			{
				stream << VarTraits::GetC(container);
			}
			return true;
		}

		FDataStream& stream;
		const Container& container;
	};

	struct ReadVisitor
	{
		inline ReadVisitor(FDataStream& stream, Container& container)
			: stream(stream)
			, container(container)
		{}

		template<typename VarTraits>
		__forceinline bool operator()(VarTraits)
		{
			if (VarTraits::SAVE)
			{
				typename VarTraits::VarType& var = VarTraits::Get(container);
				stream >> var.unsafeGet();
			}
			return true;
		}

		FDataStream& stream;
		Container& container;
	};

public:
	CvSyncArchiveBase(Container& container)
		: m_container(container)
	{}
	virtual ~CvSyncArchiveBase() {}

	//
	// FAutoArchive Interface
	//
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

	// Sends a visitor to all the CvSyncVar traits
	// The visitor may cancel at any time by returning false
	template<typename Visitor>
	static inline void Visit(Visitor& visitor)
	{
		CvSyncArchiveVarsVisit<Derived, Visitor, 0, Derived::VAR_COUNT>::Step(visitor);
	}

	inline void write(FDataStream& stream) const
	{
		WriteVisitor visitor(stream, getContainer());
		Visit(visitor);
	}

	inline void read(FDataStream& stream)
	{
		ReadVisitor visitor(stream, getContainer());
		Visit(visitor);
	}

	inline const Container& getContainer() const
	{
		return m_container;
	}

	inline Container& getContainer()
	{
		return m_container;
	}

	inline const std::vector<FAutoVariableBase*>& getAutoVars() const
	{
		return m_contents;
	}

private:
	Container& m_container;
};

// Implements Fraxis' auto variable base interface for a sync var in an archive
template<typename Traits>
struct CvSyncVarAutoVariableBase : public FAutoVariableBase
{
	inline CvSyncVarAutoVariableBase()
		: FAutoVariableBase("", static_cast<typename Traits::AutoVar*>(this)->getSyncArchive())
	{}

	virtual ~CvSyncVarAutoVariableBase() {}
	virtual void load(FDataStream& loadFrom)
	{
		typename Traits::SyncArchive& syncArchive = static_cast<typename Traits::AutoVar*>(this)->getSyncArchive();
		loadFrom >> Traits::Get(syncArchive.getContainer()).unsafeGet();
	}
	virtual void loadDelta(FDataStream& loadFrom)
	{
		load(loadFrom);
	}
	virtual void save(FDataStream& saveTo) const
	{
		const typename Traits::SyncArchive& syncArchive = static_cast<const typename Traits::AutoVar*>(this)->getSyncArchive();
		saveTo << Traits::GetC(syncArchive.getContainer());
	}
	virtual void saveDelta(FDataStream& saveTo) const
	{
		save(saveTo);
	}
	virtual void clearDelta()
	{
	}
	virtual bool compare(FDataStream& otherValue) const
	{
		typedef typename Traits::VarType VarType;

		const typename Traits::SyncArchive& syncArchive = static_cast<const typename Traits::AutoVar*>(this)->getSyncArchive();
		const VarType& var = Traits::GetC(syncArchive.getContainer());
		
		typename VarType::ValueType value;
		otherValue >> value;
		return var.get() == value;
	}
	virtual void reset()
	{
		typedef typename Traits::VarType VarType;
		typename Traits::SyncArchive& syncArchive = static_cast<typename Traits::AutoVar*>(this)->getSyncArchive();
		VarType& var = Traits::Get(syncArchive.getContainer());
		var.unsafeGet() = typename VarType::ValueType();
	}
	virtual const std::string& name() const
	{
		// We could technically declare names somewhere just for this function.
		// Without inline statics it would become a headache, so empty for now.
		return EmptyString;
	}
	virtual void setStackTraceRemark()
	{
	}
	virtual std::string debugDump(const std::vector<std::pair<std::string, std::string> >& callStacks) const
	{
		const typename Traits::SyncArchive& syncArchive = static_cast<const typename Traits::AutoVar*>(this)->getSyncArchive();
		std::string result = FAutoVariableBase::debugDump(callStacks);
		result += std::string("\n") + syncArchive.getContainer().debugDump(*this) + std::string("\n");
		return result;
	}
	virtual std::string toString() const
	{
		const typename Traits::SyncArchive& syncArchive = static_cast<const typename Traits::AutoVar*>(this)->getSyncArchive();
		return FSerialization::toString(Traits::GetC(syncArchive.getContainer()).get());
	}
};

// Automagically begins a CvSyncArchive specialization
#define SYNC_ARCHIVE_BEGIN(type) \
class type; \
template<> class CvSyncArchive<type> : public CvSyncArchiveBase<CvSyncArchive<type>> { \
	typedef type Container; \
	enum { INIT_VAR_N_ = __COUNTER__ + 1 }; \
public: \
	CvSyncArchive(Container& container) : CvSyncArchiveBase<CvSyncArchive<type>>(container), m_syncVars(NULL) {} \
	struct SyncVars { \
		SyncVars(CvSyncArchive<type>& archive) : archive(archive) {} \
		CvSyncArchive<type>& archive; \
		template<size_t, typename = void> struct VarTraits { typedef void Type; };

// Gets the next index for a sync var
// Useful if you don't want to use the SYNC_ARCHIVE_VAR macro!
#define SYNC_ARCHIVE_VAR_NEXT_INDEX __COUNTER__ - INIT_VAR_N_

// Expands into what you pass it
// Useful if you need to pass multiple template parameters since the preprocessor wont cooperate otherwise
// Feel free to move this to another file and give it a better name
#define SYNC_ARCHIVE_VAR_TYPE(...) __VA_ARGS__

// Automagically creates a traits entry for a CvSyncVar
#define SYNC_ARCHIVE_VAR(type, name, save) \
		struct name { \
			typedef CvSyncArchive<Container> SyncArchive; \
			typedef type ValueType; \
			typedef CvSyncVar<name> VarType; \
			enum { INDEX = SYNC_ARCHIVE_VAR_NEXT_INDEX, SAVE = size_t(save) }; \
			template<typename T> static inline const VarType& GetC(const T& container) { return container.name; } \
			template<typename T> static inline VarType& Get(T& container) { return container.name; } \
			class AutoVar : public CvSyncVarAutoVariableBase<name> { \
				friend SyncVars; \
				inline AutoVar() {} \
			public: \
				typedef name Traits; \
				static __forceinline const AutoVar& GetC(const SyncVars& archive) { return archive.syncVar_##name; } \
				static __forceinline AutoVar& Get(SyncVars& archive) { return archive.syncVar_##name; } \
				__forceinline const SyncArchive& getSyncArchive() const { /* This is technically not standard compliant, but I promise it is safe */ \
					return (reinterpret_cast<const SyncVars*>(reinterpret_cast<const char*>(this) - offsetof(SyncVars, syncVar_##name))->archive); \
				} \
				__forceinline SyncArchive& getSyncArchive() { return const_cast<SyncArchive&>(const_cast<const AutoVar*>(this)->getSyncArchive()); } \
			}; \
		}; \
		template<typename Dummy> struct VarTraits<name::INDEX, Dummy> { \
			typedef name Type; \
		}; \
		name::AutoVar syncVar_##name;

// Automagically ends a CvSyncArchive specialization
#define SYNC_ARCHIVE_END() \
	}; \
	enum { VAR_COUNT = __COUNTER__ - INIT_VAR_N_ }; \
	inline SyncVars* getSyncVars() { return m_syncVars; } \
	inline const SyncVars* getSyncVars() const { return m_syncVars; } \
	inline void setSyncVars(SyncVars& syncVars) { FAssert(m_syncVars == NULL); m_syncVars = &syncVars; } \
private: \
	SyncVars* m_syncVars; \
};

// Helper to declare a CvSyncArchive member for a type
#define SYNC_ARCHIVE_MEMBER(type) \
	typedef CvSyncArchive<type> SyncArchive; \
	friend SyncArchive; \
	CvSyncArchive<type> m_syncArchive;

// Helper to declare a CvSyncVar member for a type
#define SYNC_VAR_MEMBER(name) CvSyncVar<SyncArchive::SyncVars::name> name;

#endif