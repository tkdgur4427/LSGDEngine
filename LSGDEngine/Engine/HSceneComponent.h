#pragma once

#include "HActorComponent.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HSceneComponent, HActorComponent)
	class HSceneComponent : public HActorComponent
	{
	public:
		GENERATE_CLASS_BODY(HSceneComponent);

		HSceneComponent() {}
		virtual ~HSceneComponent() {}


	};
}