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
			- single-threaded LinkerSave like exactly UE3 or UE4
	*/

	struct HLinkerSaveContext
	{
		class HPackage* OutermostPkg;
		HArray<class HObject*> ObjectsToSave;
	};

	class HLinkerSave : public HLinker
	{
	public:
		HLinkerSave();
		~HLinkerSave();

		bool SavePackage(HLinkerSaveContext& InContext);

		// outermost package to save
		const HPackage* SrcPackageToSave;

		// file archive
		unique_ptr<fileIO::HFileArchive> FileArchive;
	};
}