#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../Memory/HMemoryPCH.h"
#include "../Core/HCore.h"
#include "../Core/HTypeDatabase.h"
#include "../Core/HReflect.h"

using namespace lsgd;
using namespace lsgd::reflect;

namespace GeneralUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			HTypeDatabase* TypeDB = HTypeDatabase::GetSingleton();
			TypeDB->AddClassType("Super", "SuperSuper");
		}

	};
}