#include "HEnginePCH.h"
#include "HLevel.h"

#include "HTickTaskLevel.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HLevel, HObject)

void HLevel::Reflect() 
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField<HLevel>("Actors", &HLevel::Actors);
}
