#pragma once

#include "HLocalPlayer.h"

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

		HArray<HObjectHandleUnique<HLocalPlayer>> LocalPlayers;

		//@todo... ha... omg...
		bool bExecuteHttpClientTest = false;
	};
}