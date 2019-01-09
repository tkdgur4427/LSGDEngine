#pragma once

#include "HSceneComponent.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HPrimitiveComponent, HSceneComponent)
	class HPrimitiveComponent : public HSceneComponent
	{
	public:
		GENERATE_CLASS_BODY(HPrimitiveComponent);

		HPrimitiveComponent() {}
		virtual ~HPrimitiveComponent() {}
	};
}