#include "HEnginePCH.h"
#include "HActor.h"

#include "HTickFunction.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HActor, HObject)

void HActor::Reflect() 
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField("ActorComponents", &HActor::ActorComponents);
}

// HActorTickFunction

HActorTickFunction::HActorTickFunction(HObjectHandleWeak<HActor>& InTarget)
{
	Target = InTarget;
}

HActorTickFunction::~HActorTickFunction()
{

}

void HActorTickFunction::Execute()
{
	Target->Tick(0.0f);
}

// HActor

HActor::HActor()
{
	
}

void HActor::Initialize()
{
	// create primary tick function for action tick function
	HObjectHandleWeak<HActor> Actor(GetObjectArrayData());
	PrimaryTickFunction = HMakeShared<HActorTickFunction>(Actor);
}

void HActor::Destroy()
{

}

void HActor::Tick(float DeltaTime)
{

}