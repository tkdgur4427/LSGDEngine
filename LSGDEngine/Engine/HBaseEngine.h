#pragma once

#include "HPlatformDeviceMisc.h"

namespace lsgd {

	class HBaseEngine
	{
	public:
		HBaseEngine() {}
		virtual ~HBaseEngine() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		// game instance
		HObjectHandleUnique<class HGameInstance> GameInstance;

		// window frame
		unique_ptr<HWindowFrame> WindowFrame;
	};

}