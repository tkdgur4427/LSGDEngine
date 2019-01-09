#pragma once

#include "HActorComponent.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HActor, HObject)
	class HActor : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HActor);

		HActor() {}
		virtual ~HActor() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		HArray<HObjectHandleUnique<HActorComponent>> ActorComponents;

		shared_ptr<class HTickFunction> PrimaryTickFunction;
	};
}