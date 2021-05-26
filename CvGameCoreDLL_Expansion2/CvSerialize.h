#pragma once

#ifndef CVSERIALIZE_H
#define CVSERIALIZE_H

// Shared behavior for CvSyncVar specializations
template<typename SyncTraits, typename T>
class CvSyncVarBase
{
public:
	typedef SyncTraits Traits;
	typedef typename Traits::SyncObject SyncObject;
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

	inline ValueType& dirtyGet(SyncObject& syncObject)
	{
		markDirty(syncObject);
		return m_value;
	}

	inline void set(SyncObject& syncObject, const ValueType& value)
	{
		markDirty(syncObject);
		m_value = value;
	}

	inline operator const ValueType&() const
	{
		return m_value;
	}

	inline void markDirty(SyncObject& syncObject)
	{
		syncObject.getDeltas().set(Traits::INDEX);
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
	typedef typename std::vector<T, Alloc>::const_iterator const_iterator;
	typedef typename std::vector<T, Alloc>::const_reference const_reference;

	inline CvSyncVar() {}
	inline CvSyncVar(const ValueType& value)
		: CvSyncVarBase<SyncTraits, T>(value)
	{}

	inline void setAt(SyncObject& syncObject, size_t i, const_reference value)
	{
		markDirty(syncObject);
		m_value[i] = value;
	}

	inline const_reference getAt(size_t i) const
	{
		return m_value[i];
	}

	inline const_reference operator[](size_t i) const
	{
		return m_value[i];
	}

	inline size_t size() const { return m_value.size(); }
	inline const_iterator begin() const { return m_value.begin(); }
	inline const_iterator end() const { return m_value.end(); }

	inline void push_back(SyncObject& syncObject, const_reference src)
	{
		dirtyGet(syncObject).push_back(src);
	}
	inline void erase(SyncObject& syncObject, size_t i)
	{
		dirtyGet(syncObject).erase(src);
	}
	inline void insert(SyncObject& syncObject, size_t i, const_reference src)
	{
		dirtyGet(syncObject).insert(i, src);
	}
	inline void clear(SyncObject& syncObject)
	{
		dirtyGet(syncObject).clear();
	}
	inline void resize(SyncObject& syncObject, size_t n)
	{
		dirtyGet(syncObject).resize(n);
	}
	inline void resize(SyncObject& syncObject, size_t n, const_reference v)
	{
		dirtyGet(syncObject).resize(n, v);
	}
	inline void reserve(SyncObject& syncObject, size_t n)
	{
		dirtyGet(syncObject).reserve(n, v);
	}
};

// Write CvSyncVar's value to a FDataStream
template<typename SyncTraits>
inline FDataStream& operator<<(FDataStream& stream, const CvSyncVar<SyncTraits>& syncVar)
{
	return stream << syncVar.get();
}

// Virtual dispatch table for writing out CvSyncVar data by index
template<typename T, size_t N>
class CvSyncWriterTable
{
public:
	typedef void(*PFN)(FDataStream& stream, const T& container);

private:
	// Generates a writer function
	template<typename VarTraits>
	struct VarWriter
	{
		static inline void Write(FDataStream& stream, const T& container)
		{
			stream << VarTraits::template GetC<void>(container);
		}
	};

	// Visits each var in a CvSyncObject and saves a writer function pointer to the pfns array
	class InitVisitor
	{
	public:
		InitVisitor(PFN* pfn)
			: m_pfn(pfn)
		{}

		template<typename VarTraits>
		inline bool operator()(VarTraits)
		{
			*(m_pfn++) = &VarWriter<VarTraits>::Write;
			return true;
		}

	private:
		PFN* m_pfn;
	};
public:
	inline CvSyncWriterTable()
	{
		InitVisitor visitor(pfns);
		CvSyncObject<T>::template Visit(visitor);
	}

	inline PFN& operator[](size_t i)
	{
		return pfns[i];
	}

	inline const PFN& operator[](size_t i) const
	{
		return pfns[i];
	}

	inline void operator()(size_t i, FDataStream& stream, const T& container) const
	{
		pfns[i](stream, container);
	}
	
private:
	PFN pfns[N];
};

// Contains synchronization meta-data
template<typename T>
class CvSyncObject;

// Maintains a list of variables that need synchronization
template<typename T, size_t N>
class CvSyncDeltas
{
public:
	inline bool empty() const
	{
		return m_vars.empty();
	}

	inline void clear()
	{
		m_vars.clear();
	}

	inline void set(size_t i)
	{
		m_vars.insert(i);
	}

	inline void writeAndClear(FDataStream& stream, const T& container)
	{
		const CvSyncWriterTable<T, N>& writerTable = CvSyncObject<T>::s_SyncWriterTable;
		stream << m_vars.size();
		for (std::set<size_t>::const_iterator it = m_vars.begin(); it != m_vars.end(); ++it)
		{
			const size_t varIndex = *it;
			stream << varIndex;
			writerTable(varIndex, stream, container);
		}
		clear();
	}
private:
	// Set of var indices referencing the variables that have changed.
	std::set<size_t> m_vars;
};

template<typename T>
class CvSyncObjectBase;

template<typename Container>
class CvSyncObjectBase<CvSyncObject<Container>>
{
	typedef CvSyncObject<Container> Derived;
	template<typename Visitor>
	struct VisitLooper
	{
		template<size_t N>
		static inline void Step(Visitor& visitor)
		{
			typedef typename Derived::template VarTraits<N>::Type Traits;
			if (visitor(Traits()))
			{
				Step<N + 1>(visitor);
			}
		}

		template<>
		static inline void Step<Derived::VAR_COUNT>(Visitor& visitor)
		{
			// Intentionally empty to terminate recursion
		}
	};

	struct WriteVisitor
	{

		inline WriteVisitor(FDataStream& stream, const Container& container)
			: stream(stream)
			, container(container)
		{}

		template<typename VarTraits>
		inline bool operator()(VarTraits)
		{
			if (VarTraits::SAVE)
			{
				stream << VarTraits::template GetC<void>(container);
			}
			return true;
		}

		FDataStream& stream;
		const Container& container;
	};

	struct ReadVisitor
	{

		inline ReadVisitor(Derived& syncObject, FDataStream& stream, Container& container)
			: syncObject(syncObject)
			, stream(stream)
			, container(container)
		{}

		template<typename VarTraits>
		inline bool operator()(VarTraits)
		{
			if (VarTraits::SAVE)
			{
				typename VarTraits::VarType& var = VarTraits::template Get<void>(container);
				stream >> var.dirtyGet(syncObject);
			}
			return true;
		}

		Derived& syncObject;
		FDataStream& stream;
		Container& container;
	};

public:
	// Sends a visitor to all the CvSyncVar traits
	// The visitor may cancel at any time by returning false
	template<typename Visitor>
	static inline void Visit(Visitor& visitor)
	{
		VisitLooper<Visitor>::template Step<0>(visitor);
	}

	inline void write(FDataStream& stream, const Container& container) const
	{
		WriteVisitor visitor(stream, container);
		Visit(visitor);
	}

	inline void read(FDataStream& stream, Container& container)
	{
		ReadVisitor visitor(*static_cast<Derived*>(this), stream, container);
		Visit(visitor);
	}
};

// Automagically begins a CvSyncObject specialization
#define SYNC_OBJECT_BEGIN(type) \
class type; \
template<> class CvSyncObject<type> : public CvSyncObjectBase<CvSyncObject<type>> { \
	typedef type Container; \
	enum { INIT_VAR_N_ = __COUNTER__ + 1 }; \
public: \
	template<size_t> struct VarTraits { typedef void Type; };

// Gets the next index for a sync var
// Useful if you don't want to use the SYNC_OBJECT_VAR macro!
#define SYNC_OBJECT_VAR_NEXT_INDEX __COUNTER__ - INIT_VAR_N_

// Expands into what you pass it
// Useful if you need to pass multiple template parameters since the preprocessor wont cooperate otherwise
// Feel free to move this to another file and give it a better name
#define SYNC_OBJECT_VAR_TYPE(...) __VA_ARGS__

// Automagically creates a traits entry for a CvSyncVar
#define SYNC_OBJECT_VAR(type, name, save) \
	struct name { \
		typedef CvSyncObject<Container> SyncObject; \
		typedef type ValueType; \
		typedef CvSyncVar<name> VarType; \
		enum { INDEX = SYNC_OBJECT_VAR_NEXT_INDEX, SAVE = size_t(save) }; \
		template<typename> static inline const VarType& GetC(const Container& container) { return container.name; } \
		template<typename> static inline VarType& Get(Container& container) { return container.name; } \
	}; \
	template<> struct VarTraits<name::INDEX> { \
		typedef name Type; \
	};

// Automagically ends a CvSyncObject specialization
#define SYNC_OBJECT_END() \
	enum { VAR_COUNT = __COUNTER__ - INIT_VAR_N_ }; \
	inline CvSyncDeltas<Container, VAR_COUNT>& getDeltas() { return m_deltas; } \
	inline const CvSyncDeltas<Container, VAR_COUNT>& getDeltas() const { return m_deltas; } \
	static const CvSyncWriterTable<Container, VAR_COUNT> s_SyncWriterTable; \
private: \
	CvSyncDeltas<Container, VAR_COUNT> m_deltas; \
};

// Helper to declare a CvSyncObject member for a type
#define SYNC_OBJECT_MEMBER(type) \
	typedef CvSyncObject<CvUnit> SyncObject; \
	friend SyncObject; \
	CvSyncObject<CvUnit> m_syncObject;

// Helper to declare a CvSyncVar member for a type
#define SYNC_VAR_MEMBER(name) CvSyncVar<SyncObject::name> name;

#endif