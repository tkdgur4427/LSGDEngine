#pragma once

#include "HActor.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HHUD, HActor)
	class HHUD : public HActor
	{
	public:
		GENERATE_CLASS_BODY(HHUD)

		HHUD() {}
		virtual ~HHUD() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);
	};
}