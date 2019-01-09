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

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		// owner
		HObjectHandleUnique<class HActor> Owner;

		// primary tick function
		shared_ptr<class HTickFunction> PrimaryTickFunction;
	};
}