#pragma once

#include "HLinker.h"
#include "HReflectionContext.h"

namespace lsgd
{
	/*
		** 2018.11.3
			- single-threaded LinkerSave like exactly UE3 or UE4
	*/
	class HLinkerSave : public HLinker , public reflect::HReflectionContext
	{
	public:
		HLinkerSave();

		const HPackage* SrcPackageToSave;
	};
}