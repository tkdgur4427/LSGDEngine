#pragma once

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HGameInstance, HObject)
	class HGameInstance : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HGameInstance)

		HGameInstance() {}
		virtual ~HGameInstance() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);		
	};
}