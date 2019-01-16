#pragma once

#include "HController.h"

namespace lsgd
{
	DECLARE_CLASS_TYPE1(HPlayerController, HController)
	class HPlayerController : public HController
	{
	public:
		GENERATE_CLASS_BODY(HPlayerController)

		HPlayerController() {}
		virtual ~HPlayerController() {}

		virtual void Initialize();
		virtual void Destroy();
		virtual void Tick(float DeltaTime);

		// could be a local player or net connection
		HObjectHandleWeak<class HPlayer> Player; 
		// possessed pawn
		//	- note that we can possess/unpossess the pawn, so use objecthandleweak
		HObjectHandleWeak<class HPawn> Pawn;

		// HUD
		HObjectHandleUnique<class HHUD> HUD;
		// camera manager associated with this player controller
		HObjectHandleUnique<class HPlayerCameraManager> PlayerCameraManager;
		// object that manages player input
		HObjectHandleUnique<class HPlayerInput> PlayerInput;
	};
}