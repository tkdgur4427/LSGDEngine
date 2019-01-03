#include "HEnginePCH.h"
#include "HActor.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HActor, HObject)

void HActor::Reflect() 
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("ActorComponents", &HActor::ActorComponents);
}
