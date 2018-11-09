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
		HString SrcFilename;
	};

	class HLinkerLoad : public HLinker
	{
	public:
		bool LoadPackage(HLinkerLoadContext& InContext);

		// outermost package to save
		const HPackage* PackageToLoad;

		// file archive
		fileIO::HFileArchive* FileArchive;
	};
}