#pragma once

#include "HActor.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HPawn, HActor)
	class HPawn : public HActor
	{
	public:
		GENERATE_CLASS_BODY(HPawn)

		HPawn() {}
		virtual ~HPawn() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);
	};
}