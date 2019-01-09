#include "HEnginePCH.h"
#include "HActorComponent.h"

#include "HTickFunction.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HActorComponent, HObject)

void HActorComponent::Reflect() 
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField<HActorComponent>("Owner", &HActorComponent::Owner);
}

// HActorComponentTickFunction
HActorComponentTickFunction::HActorComponentTickFunction()
{

}

HActorComponentTickFunction::~HActorComponentTickFunction()
{

}

void HActorComponentTickFunction::ExecuteTick()
{
	Target->Tick(0.0f);
}

// HActorComponent

void HActorComponent::Initialize()
{

}

void HActorComponent::Destroy()
{

}

void HActorComponent::Tick(float DeltaTime)
{

}