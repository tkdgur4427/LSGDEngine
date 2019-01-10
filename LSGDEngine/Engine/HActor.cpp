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

HActorTickFunction::HActorTickFunction()
{

}

HActorTickFunction::~HActorTickFunction()
{

}

void HActorTickFunction::Execute()
{
	Target->Tick(0.0f);
}

// HActor

void HActor::Initialize()
{

}

void HActor::Destroy()
{

}

void HActor::Tick(float DeltaTime)
{

}