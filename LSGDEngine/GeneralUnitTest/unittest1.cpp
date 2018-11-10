#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../Core/HCore.h"

using namespace lsgd;
using namespace lsgd::reflect;

// std allocator override test
#include <vector>

DECLARE_CLASS_TYPE(A)
class A : public lsgd::HObject
{
public:
	GENERATE_CLASS_BODY(A);

	int TestMethod(int a) { return a; }
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
			
			A* AInstance = AllocateHObject<A>();

			HLinkerSaveContext SaveContext;
			SaveContext.ObjectsToSave = ((HTranscientPackageForTest*)GTransientPackage)->TrackedObjects;
			SaveContext.OutermostPkg = GTransientPackage;

			HLinkerSave LinkerSave;
			LinkerSave.SavePackage(SaveContext);

			HLinkerLoad LinkerLoad;
			LinkerLoad.LoadPackage(HLinkerLoadContext());

			int Param0 = 10;
			lsgd::reflect::HNativeFunction* Func0 = (lsgd::reflect::HNativeFunction*)(TypeDB->GetClass("A")->Methods[0].get());

			lsgd::reflect::HNativeFunctionFrame Frame;
			Frame.SetFrame((uint8*)&AInstance, Param0);
			Func0->GetNativeFunctionObject()->CallFunction(Frame);

			int Output = TypeDB->GetClass("A")->Methods[0]->Invoke<int>(&AInstance, Param0);
		}
	};
}