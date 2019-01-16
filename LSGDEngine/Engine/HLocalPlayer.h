#pragma once

#include "HPlayer.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HLocalPlayer, HPlayer)
	class HLocalPlayer : public HPlayer
	{
	public:
		GENERATE_CLASS_BODY(HLocalPlayer)

		HLocalPlayer() {}
		virtual ~HLocalPlayer() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		// the master viewport containing this player's view
		// HGameViewportClient* ViewportClient;

		// viewport's origin and size consisting of 2d box
		HVector2 Origin; 
		HVector2 Size;
	};
}