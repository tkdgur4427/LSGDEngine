#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../Core/HCore.h"

using namespace lsgd;
using namespace lsgd::reflect;

// std allocator override test
#include <vector>

DECLARE_CLASS_TYPE(A)
class A
{
public:
	GENERATE_CLASS_BODY(A);

	int TestMethod(int a) { return 0; }
	int TestMethodConst(double a, float b) const { return 0; }

	double ADouble;
};

IMPLEMENT_CLASS_TYPE(A);

typedef int(*FunctionTest)(int, double);

namespace GeneralUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			HTypeDatabase* TypeDB = HTypeDatabase::GetSingleton();			
			TypeDB->AddClassField("ADouble", &A::ADouble);
			TypeDB->AddClassMethod("TestMethod", &A::TestMethod);

			lsgd::reflect::HNativeFunctionFrame Frame;
			int Param0 = 124;
			double Param1 = 2.3;
			float Param2 = 1.1f;
			Frame.SetFrame(nullptr, Param0, Param1, Param2);
		}
	};
}