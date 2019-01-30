#pragma once

namespace lsgd
{
	// base class of reference counted objects
	class HRefCountedObject
	{
	public:
		HRefCountedObject()
			: NumRefs(0)
		{}

		virtual ~HRefCountedObject() { check(NumRefs == 0); }

		uint32 AddRef() const
		{
			return uint32(++NumRefs);
		}

		uint32 Release() const
		{
			uint32 Refs = uint32(--NumRefs);
			if (Refs == 0)
			{
				delete this;
			}
			return Refs;
		}

		uint32 GetRefCount() const
		{
			return uint32(NumRefs);
		}

	private:
		mutable int32 NumRefs;
	};

	// smart pointer to an object which implements AddRef/Release (think of COM ptr)
	template <typename ReferencedType>
	class HRefCountPtr
	{
		typedef ReferencedType* ReferenceType;

	public:
		HRefCountPtr()
			: Reference(nullptr)
		{}

		HRefCountPtr(ReferencedType* InReference, bool bAddRef = true)
		{
			Reference = InReference;
			if (Reference && bAddRef)
			{
				Reference->AddRef();
			}
		}

		HRefCountPtr(const HRefCountPtr& Copy)
		{
			Reference = Copy.Reference;
			if (Reference)
			{
				Reference->AddRef();
			}
		}

		HRefCountPtr(HRefCountPtr&& Copy)
		{
			Reference = Copy.Reference;
			Copy.Reference = nullptr;
		}

		~HRefCountPtr()
		{
			if (Reference)
			{
				Reference->Release();
			}
		}

		HRefCountPtr& operator=(ReferenceType* InReference)
		{
			// add ref before release, in case the new reference is the same as the old reference
			ReferenceType* OldReference = Reference;
			Reference = InReference;
			if (Reference)
			{
				Reference->AddRef();
			}
			if (OldReference)
			{
				OldReference->Release();
			}
			return *this;
		}

		HRefCountPtr& operator=(const HRefCountPtr& InPtr)
		{
			return *this = InPtr.Reference;
		}

		HRefCountPtr& operator=(HRefCountPtr&& InPtr)
		{
			if (this != &InPtr)
			{
				ReferencedType* OldReference = Reference;
				Reference = InPtr.Reference;
				InPtr.Reference = nullptr;
				if (OldReference)
				{
					OldReference->Release();
				}
			}
			return *this;
		}

		ReferencedType* operator->() const { return Reference; }
		operator ReferenceType() const { return Reference; }

		friend uint32 GetTypeHash(const HRefCountPtr& InPtr)
		{
			return GetTypeHash(InPtr.Reference);
		}

		ReferenceType** GetInitReference()
		{
			*this = nullptr;
			return &Reference;
		}

		ReferencedType* GetReference() const
		{
			return Reference;
		}

		bool IsValid() const { return Reference != nullptr; }
		void SafeRelease() { *this = nullptr; }

		uint32 GetRefCount()
		{
			uint32 Result = 0;
			if (Reference)
			{
				Result = Reference->GetRefCount();
				check(Result > 0);
			}
			return Result;
		}

		void Swap(HRefCountPtr& InPtr)
		{
			ReferencedType* OldReference = Reference;
			Reference = InPtr.Reference;
			InPtr.Reference = OldReference;
		}
		
	private:
		ReferencedType* Reference;
	};

	template <typename ReferencedType>
	bool operator== (const HRefCountPtr<ReferencedType>& A, const HRefCountPtr<ReferencedType>& B)
	{
		return A.GetReference() == B.GetReference();
	}

	template <typename ReferencedType>
	bool operator== (const HRefCountPtr<ReferencedType>& A, const ReferencedType* B)
	{
		return A.GetReference() == B;
	}

	template <typename ReferencedType>
	bool operator== (const ReferencedType* A, const HRefCountPtr<ReferencedType>& B)
	{
		return A == B.GetReference();
	}
}