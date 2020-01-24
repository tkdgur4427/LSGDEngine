#include "HEnginePCH.h"
#include "HLevel.h"

#include "HTickTaskLevel.h"

using namespace lsgd;

IMPLEMENT_CLASS_TYPE1(HLevel, HObject)

void HLevel::Reflect() 
{
	reflect::HTypeDatabase::GetSingleton()->AddClassField<HLevel>("Actors", &HLevel::Actors);
}

HLevel::HLevel()
{
	// create tick task level
	TickTaskLevel = HMakeShared<HTickTaskLevel>();
}

HObjectHandleWeak<HActor> HLevel::AddActor(HObjectHandleUnique<HActor>& InActor)
{
	int32 NewIndex = Actors.size();
	Actors.push_back(HMove(InActor));

	HObjectHandleWeak<HActor> NewActor(Actors[NewIndex]->GetObjectArrayData());

	// add tick function
	shared_ptr<HTickFunction> TickFunction = NewActor->PrimaryTickFunction;
	TickTaskLevel->AllEnabledTickFunctions.push_back(TickFunction);

	return NewActor;
}

void HLevel::RemoveActor(HObjectHandleWeak<HActor>& InActor)
{
	int32 RemoveIndex = FindActor(InActor);
	Actors.erase(Actors.begin() + RemoveIndex);

	// remove tick function
	shared_ptr<HTickFunction> TickFunction = InActor->PrimaryTickFunction;
	TickTaskLevel->RemoveTickFunction(TickFunction);
}

int32 HLevel::FindActor(HObjectHandleWeak<HActor>& InActor)
{
	int32 Index = 0;
	int32 RemoveIndex = -1;
	for (auto& Actor : Actors)
	{
		if (Actor->State.ObjectArrayData == InActor->State.ObjectArrayData)
		{
			RemoveIndex = Index;
			break;
		}
		Index++;
	}

	return RemoveIndex;
}
