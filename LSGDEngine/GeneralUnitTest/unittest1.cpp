#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../Core/HCore.h"

using namespace lsgd;
using namespace lsgd::reflect;

// std allocator override test
#include <vector>

class A
{
public:
	DECLARE_CLASS_TYPE(A, );

	double ADouble;
};

namespace GeneralUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			HTypeDatabase* TypeDB = HTypeDatabase::GetSingleton();
			TypeDB->AddClassType("A", "");
			TypeDB->AddClassField<A, double>("ADouble", &A::ADouble);
		}

	};
}