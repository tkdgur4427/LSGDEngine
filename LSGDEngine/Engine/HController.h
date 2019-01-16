#pragma once

#include "HActor.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HController, HActor)
	class HController : public HActor
	{
	public:
		GENERATE_CLASS_BODY(HController)

		HController() {}
		virtual ~HController() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);
	};
}