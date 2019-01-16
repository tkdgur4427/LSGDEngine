#pragma once

namespace lsgd
{
	struct HKey 
	{
		HName KeyName;
	};

	struct HKeyState {};

	DECLARE_CLASS_TYPE1(HPlayerInput, HObject)
	class HPlayerInput : public HObject
	{
	public:
		GENERATE_CLASS_BODY(HPlayerInput)

		HPlayerInput() {}
		virtual ~HPlayerInput() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		//HHashMap<HKey, HKeyState> KeyStateMap;
	};
}