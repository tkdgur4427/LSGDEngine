#pragma once

namespace lsgd
{
	class HObject;
	class HName;
}

namespace lsgd { namespace reflect {

	class HReflectionContext
	{
	public:
		HReflectionContext() {}
		virtual ~HReflectionContext() {}

		virtual HReflectionContext& operator<<(HName& Value)
		{
			return *this;
		}

		virtual HReflectionContext& operator<<(class HClass& Value)
		{
			return *this;
		}

		virtual HReflectionContext& operator<<(HObject*& Value)
		{
			return *this;
		}

		virtual void Serialize(void* Value, int64 Length) {}

		// inline methods
		bool IsSaving() const { return bIsSaving; }
		bool IsLoading() const { return !bIsSaving; }
		
		bool bIsSaving;
	};

	// serialization context
	class HSerializeContext : public HReflectionContext
	{
	public:
		HSerializeContext() {}
		virtual ~HSerializeContext() {}

		virtual HReflectionContext& operator<<(class HName& Value) override;
		virtual HReflectionContext& operator<<(class HObject*& Value) override;
	};

	// behavior context
	class HBehaviorContext : public HSerializeContext
	{
		HBehaviorContext() {}
		virtual ~HBehaviorContext() {}
	};

	// network reflection context
	class HNetworkContext : public HSerializeContext
	{
		HNetworkContext() {}
		virtual ~HNetworkContext() {}
	};
}
}