#pragma once

#include "HActor.h"

namespace lsgd
{
	struct HMinimalViewInfo
	{
		HVector3 Location;
		HQuaternion Rotation;

		float FOV;
		float OrthoWidth;
		float OrthoNearPlane;
		float OrthoFarPlane;
	};

	struct HTViewTarget
	{
		HObjectHandleWeak<class HActor> Target; // target actor to compute POV

		// computed point of view
		HMinimalViewInfo POV;
		// class APlayerState* PlayerState // used to follow same player through pawn transition etc, when spectating
	};

	DECLARE_CLASS_TYPE1(HPlayerCameraManager, HActor)
	class HPlayerCameraManager : public HActor
	{
	public:
		GENERATE_CLASS_BODY(HPlayerCameraManager)

		HPlayerCameraManager() {}
		virtual ~HPlayerCameraManager() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		HObjectHandleWeak<class HPlayerController> Owner;

		// current view target
		HTViewTarget ViewTarget;

		// internal; receives the output of individual camera animation
		HObjectHandleUnique<class HCameraActor> AnimCameraActor;
	};
}