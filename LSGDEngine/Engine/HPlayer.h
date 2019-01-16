#pragma once

#include "HPlayerController.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HPlayer, HObject)
	class HPlayer : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HPlayer)

		HPlayer() {}
		virtual ~HPlayer() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		HObjectHandleUnique<HPlayerController> PlayerController;
	};
}