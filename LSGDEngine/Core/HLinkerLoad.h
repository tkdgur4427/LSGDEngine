#pragma once

#include "HLinker.h"
#include "HReflectionContext.h"

// forward declaration
namespace lsgd { namespace fileIO {

		class HFileArchive;
} }

namespace lsgd
{
	/*
		** 2018.11.3
			- single-threaded LinkerLoad like exactly UE3 or UE4
	*/

	struct HLinkerLoadContext
	{
		HLinkerLoadContext()
			: SrcPackage(nullptr)
		{}

		class HPackage* SrcPackage;
	};

	class HLinkerLoad : public HLinker
	{
	public:
		HLinkerLoad();
		~HLinkerLoad();

		bool LoadPackage(HLinkerLoadContext& InContext);

		// file archive
		unique_ptr<fileIO::HFileArchive> FileArchive;
	};
}