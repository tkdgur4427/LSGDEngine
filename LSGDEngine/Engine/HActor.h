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

		HArray<HObjectHandleUnique<HActorComponent>> ActorComponents;
	};
}