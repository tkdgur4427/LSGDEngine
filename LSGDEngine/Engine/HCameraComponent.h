#pragma once

#include "HSceneComponent.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HCameraComponent, HSceneComponent)
	class HCameraComponent : public HSceneComponent
	{
	public:
		GENERATE_CLASS_BODY(HCameraComponent);

		HCameraComponent() {}
		virtual ~HCameraComponent() {}

		float FieldOfView;
		float OrthoWidth;
		float OrthoNearClipPlane;
		float OrthoFarClipPlane;
		float AspectRatio;

		//ECameraProjectMode ProjectionMode

		//FTransform AdditiveOffset;
		//float AdditionFOVOffset;

		//HArray<HPostProcessSettings> ExtraPostProcessBlends
		//HArray<float> ExtraPostProcessBlendWeights;

		//UDrawFrstumComponent* DrawFrustum;
		//class UStaticMesh* CameraMesh;
		//class UStaticMeshComponent* ProxyMeshComponent;
	};
}