#pragma once

#include "HPlayer.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HNetConnection, HPlayer)
	class HNetConnection : public HPlayer
	{
	public:
		GENERATE_CLASS_BODY(HNetConnection)

		HNetConnection() {}
		virtual ~HNetConnection() {}

		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void Tick(float DeltaTime) override;
	};
}