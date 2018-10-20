#pragma once

// forward declarations
namespace lsgd { namespace reflect {
	class HClass;
} }

namespace lsgd
{
	class HObjectInitializer
	{

	};

	class HCoreObject
	{
	public:
		HCoreObject();

	protected:
		void GenerateName();

		// object's unique name
		HName Name;
		// class definition
		reflect::HClass* Class;
	};

	class HObject : public HCoreObject
	{

	};
}