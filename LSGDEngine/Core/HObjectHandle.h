#pragma once

namespace lsgd
{
	class HObject;

	// object array data (common data)
	struct HObjectArrayData
	{
		HObjectArrayData()
		{
			Reset();
		}

		HObjectArrayData(uint32 InSerialNumber, uint32 InIndex)
			: SerialNumber(InSerialNumber)
			, Index(InIndex)
		{}

		void Reset()
		{
			SerialNumber = -1;
			Index = -1;
		}

		friend bool operator==(const HObjectArrayData& ObjectArrayData0, const HObjectArrayData& ObjectArrayData1);

		bool IsValid() const { return !(SerialNumber == -1 && Index == -1); }

		uint32 SerialNumber;	// object's unique number
		uint32 Index;			// GObjectArray's index
	};

	// object helper class 
	class HObjectHelper
	{
	public:
		// wtf... msbuild..
		static HObject* GetObjectZ(uint32 Index, uint32 SerialNumber);
		static void SetAsDestroyed(const HObjectArrayData& InData);
		static void SetAsRootSet(const HObjectArrayData& InData);
		static void UnsetAsRootSet(const HObjectArrayData& InData);
		static void MarkGC(const HObjectArrayData& InData);
		static void UnMarkGC(const HObjectArrayData& InData);
	};

	// object handle	
	class HObjectHandle
	{
	public:
		// whether the handle has ownership of a object
		bool IsOwned() const { return bIsOwned; }
		// whether object handle is valid or not
		bool IsValid() const { return Data.IsValid() && HObjectHelper::GetObjectZ(Data.Index, Data.SerialNumber) != nullptr; }

		// get the object
		template <typename ObjectType>
		ObjectType* GetObject() const
		{
			// early validation out
			if (Data.Index == -1)
			{
				return nullptr;
			}

			// @todo - HCast<>
			return (ObjectType*)HObjectHelper::GetObjectZ(Data.Index, Data.SerialNumber);
		}

		void SetRoot()
		{
			bIsRootSet = true;
			HObjectHelper::SetAsRootSet(Data);
		}

		void UnsetRoot()
		{
			HObjectHelper::UnsetAsRootSet(Data);
			bIsRootSet = false;
		}

		void Invalidate()
		{
			bIsRootSet = false;
			bIsInvalidate = true;
		}

	protected:
		HObjectHandle(bool InbIsOwned)
			: bIsOwned(InbIsOwned)
			, bIsRootSet(false)
			, bIsInvalidate(false)
		{
			Data.Reset();
		}

		HObjectHandle(const HObjectArrayData& InData, bool InbIsOwned)
			: Data(InData)
			, bIsRootSet(false)
			, bIsOwned(InbIsOwned)
		{}

		virtual ~HObjectHandle()
		{}

		HObjectArrayData Data;		
		uint32 bIsOwned : 1;
		uint32 bIsRootSet : 1;
		uint32 bIsInvalidate : 1;
	};

	template <typename ObjectType>
	class HObjectHandleUnique : public HObjectHandle
	{
	public:
		HObjectHandleUnique()
			: HObjectHandle(true)
		{}

		// explicit constructor
		explicit HObjectHandleUnique(const HObjectArrayData& InData)
			: HObjectHandle(InData, true)
		{
			
		}

		virtual ~HObjectHandleUnique()
		{
			check(!bIsRootSet);
			if (!bIsInvalidate)
			{
				HObjectHelper::SetAsDestroyed(Data);
			}				
		}

		// constructor/assignment that allows move semantics
		HObjectHandleUnique(HObjectHandleUnique&& Moving) noexcept
			: HObjectHandle(true)
		{
			Moving.Swap(*this);
		}

		HObjectHandleUnique& operator=(HObjectHandleUnique&& Moving) noexcept
		{
			HObjectHandleUnique<ObjectType> Tmp(Moving.Release());
			Tmp.Swap(*this);
			return *this;
		}

		HObjectHandleUnique(HObjectHandleUnique const&) = delete;
		HObjectHandleUnique& operator=(HObjectHandleUnique const&) = delete;

		ObjectType* operator->() const { return GetObject<ObjectType>(); }
		ObjectType& operator*() const { return *GetObject<ObjectType>(); }
		ObjectType* Get() const { return GetObject<ObjectType>(); }
		explicit operator bool() const { return (Get() != nullptr); }

		HObjectArrayData Release() noexcept
		{
			HObjectArrayData Result;
			HSwap(Result, Data);
			Invalidate();
			return Result;
		}

		void Swap(HObjectHandleUnique& InUnqiueHandle) noexcept
		{			
			HSwap(Data, InUnqiueHandle.Data);
			// after swap, invalidate the source unique
			Invalidate();
		}

		void Reset()
		{
			HObjectArrayData Tmp = Release();
			HObjectHelper::SetAsDestroyed(Tmp);
		}
	};

	template <typename ObjectType>
	class HObjectHandleWeak : public HObjectHandle
	{
	public:
		HObjectHandleWeak()
			: HObjectHandle(false)
		{}

		explicit HObjectHandleWeak(const HObjectArrayData& InData)
			: HObjectHandle(InData, false)
		{
			check(IsValid());
		}

		virtual ~HObjectHandleWeak()
		{
			// do nothing (done by GC)
		}

		HObjectHandleWeak(HObjectHandleWeak const& InOther)
			: HObjectHandle(InOther.Data, false)
		{
			
		}

		HObjectHandleWeak& operator=(HObjectHandleWeak const& InOther)
		{
			Data = InOther.Data;
			bIsRootSet = InOther.bIsRootSet;
			bIsOwned = InOther.bIsOwned;
			bIsInvalidate = InOther.bIsInvalidate;

			return *this;
		}

		ObjectType* operator->() const { return GetObject<ObjectType>(); }
		ObjectType& operator*() const { return *GetObject<ObjectType>(); }
		ObjectType* Get() const { return GetObject<ObjectType>(); }
		explicit operator bool() const { return (Get() != nullptr); }
	};

	template <class ObjectType>
	struct HIsObjectHandleUnique
	{
		static const bool Value = false;
	};

	template <class ObjectType>
	struct HIsObjectHandleUnique<HObjectHandleUnique<ObjectType>>
	{
		static const bool Value = true;
	};

	template <class ObjectType>
	struct HIsObjectHandleWeak
	{
		static const bool Value = false;
	};

	template <class ObjectType>
	struct HIsObjectHandleWeak<HObjectHandleWeak<ObjectType>>
	{
		static const bool Value = true;
	};

	template <class ObjectType>
	struct HIsObjectHandle
	{
		static const bool Value = (HIsObjectHandleUnique<ObjectType>::Value || HIsObjectHandleWeak<ObjectType>::Value);
	};

	template <class ObjectType>
	struct HRemoveObjectHandleUnique
	{
		using Type = ObjectType;
	};

	template <class ObjectType>
	struct HRemoveObjectHandleUnique<HObjectHandleUnique<ObjectType>>
	{
		using Type = ObjectType;
	};

	template <class ObjectType>
	struct HRemoveObjectHandleWeak
	{

	};

	template <class ObjectType>
	struct HRemoveObjectHandleWeak<HObjectHandleWeak<ObjectType>>
	{
		using Type = ObjectType;
	};

	inline bool operator==(const lsgd::HObjectArrayData& ObjectArrayData0, const lsgd::HObjectArrayData& ObjectArrayData1)
	{
		return (ObjectArrayData0.Index == ObjectArrayData1.Index) && (ObjectArrayData0.SerialNumber == ObjectArrayData1.SerialNumber);
	}
}

template <class ObjectType>
lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HObjectHandleUnique<ObjectType>& InObjectHandleUnique)
{
	// only serialize for unique object handle
	HObject* ObjectThis = InObjectHandleUnique.Get();
	InContext << ObjectThis;
	return InContext;
}

template <class ObjectType>
lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HObjectHandleWeak<ObjectType>& InObjectHandleWeak)
{
	// do nothing
}
