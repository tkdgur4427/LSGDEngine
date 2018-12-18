#pragma once

namespace lsgd
{
	class HObject;

	// object array data (common data)
	struct HObjectArrayData
	{
		void Reset()
		{
			SerialNumber = -1;
			Index = -1;
		}

		uint32 SerialNumber;	// object's unique number
		uint32 Index;			// GObjectArray's index
	};

	// object helper class 
	struct HObjectHelper
	{
		static HObject* GetObject(uint32 Index, uint32 SerialNumber);
		static void SetAsDestroyed(const HObjectArrayData& InData);
	};

	// object handle	
	class HObjectHandle
	{
	public:
		// whether the handle has ownership of a object
		bool IsOwned() const { return bIsOwned; }
		// whether object handle is valid or not
		bool IsValid() const { return HObjectHelper::GetObject(Data.Index, Data.SerialNumber) != nullptr; }

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
			return (ObjectType*)HObjectHelper::GetObject(Data.Index, Data.SerialNumber);
		}

	protected:
		HObjectHandle(bool InbIsOwned)
			: bIsOwned(InbIsOwned)
		{
			Data.Reset();
		}

		HObjectHandle(const HObjectArrayData& InData, bool InbIsOwned)
			: Data(InData)
			, bIsOwned(InbIsOwned)
		{}

		virtual ~HObjectHandle()
		{}

		HObjectArrayData Data;
		bool bIsOwned;
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
			check(IsValid()); // for explicit constructor, object should be valid
		}

		virtual ~HObjectHandleUnique()
		{
			HObjectHelper::SetAsDestroyed(Data);
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
			return Result;
		}

		void Swap(HObjectHandleUnique& InUnqiueHandle) noexcept
		{
			HSwap(Data, InUnqiueHandle.Data);
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

		HObjectHandleWeak(HObjectHandleWeak const&) = delete;
		HObjectHandleWeak& operator=(HObjectHandleWeak const&) = delete;

		ObjectType* operator->() const { return GetObject<ObjectType>(); }
		ObjectType& operator*() const { return *GetObject<ObjectType>(); }
		ObjectType* Get() const { return GetObject<ObjectType>(); }
		explicit operator bool() const { return (Get() != nullptr); }
	};
}