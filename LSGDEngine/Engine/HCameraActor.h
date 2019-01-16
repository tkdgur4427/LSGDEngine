#pragma once

#include "HActor.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HCameraActor, HActor)
	class HCameraActor : public HActor
	{
	public:
		GENERATE_CLASS_BODY(HCameraActor)

		HCameraActor() {}
		virtual ~HCameraActor() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		HObjectHandleUnique<class HCameraComponent> CameraComponent;
		// TWeakObjectPtr<class UCameraAnim> PreviewedCameraAnim;
	};
}