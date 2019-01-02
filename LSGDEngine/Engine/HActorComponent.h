#pragma once

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HActorComponent, HObject)
	class HActorComponent : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HActorComponent);

		HActorComponent() {}
		virtual ~HActorComponent() {}
	};
}