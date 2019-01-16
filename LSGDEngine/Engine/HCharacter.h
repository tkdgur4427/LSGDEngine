#pragma once

#include "HPawn.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HCharacter, HPawn)
	class HCharacter : public HPawn
	{
	public:
		GENERATE_CLASS_BODY(HCharacter)

		HCharacter() {}
		virtual ~HCharacter() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);
	};
}