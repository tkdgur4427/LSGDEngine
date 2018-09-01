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

	int TestMethod(int a) { return 0; }
	int TestMethodConst(double a, float b) const { return 0; }

	double ADouble;
};

typedef int(*FunctionTest)(int, double);

namespace GeneralUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			HTypeDatabase* TypeDB = HTypeDatabase::GetSingleton();
			TypeDB->AddClassType("A", "");
			TypeDB->AddClassField("ADouble", &A::ADouble);
			TypeDB->AddClassMethod("TestMethod", &A::TestMethod);

			HTypeDatabaseUtils::DecomposeFunction<FunctionTest>();
		}
	};
}