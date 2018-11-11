#include "HCorePCH.h"
#include "HLinkerLoad.h"

// file archive
#include "HFileArchive.h"
// package
#include "HPackage.h"

using namespace lsgd;

HLinkerLoad::HLinkerLoad()
{

}

HLinkerLoad::~HLinkerLoad()
{

}

bool HLinkerLoad::LoadPackage(HLinkerLoadContext& InContext)
{
	// create file archive
	HString FilePath = HGenericPlatformMisc::GetGameDir();
	if (InContext.SrcPackage != nullptr)
	{
		FilePath += InContext.SrcPackage->Name.ToString();
		LinkerRoot = InContext.SrcPackage;
	}
	else
	{
		// if there is no src package, set it as GTransientPackage
		FilePath += GTransientPackage->Name.ToString();
		LinkerRoot = GTransientPackage;
	}

	// create file archive to read
	FileArchive = make_unique<fileIO::HFileArchive, fileIO::HFileArchiveRead>(FilePath);

	int64 TOCOffset = FileArchive->Tell();
	(*FileArchive) << TOC;

	check(FileArchive->Tell() == TOC.ExportOffset);
	(*FileArchive) << ExportMap;

	check(FileArchive->Tell() == TOC.ImportOffset);
	(*FileArchive) << ImportMap;

	check(FileArchive->Tell() == TOC.NameOffset);
	(*FileArchive) << NameMap;

	check(FileArchive->Tell() == TOC.DependsOffset);
	(*FileArchive) << DependsMap;

	check(FileArchive->Tell() == TOC.TotalHeaderSize);

	// real object export serialization
	for (HObjectExport& ObjectExport : ExportMap)
	{
		check(FileArchive->Tell() == ObjectExport.SerialOffset);

		// create new HObject
		// @todo - need to make GC for HObject!
		HObject* NewObject = AllocateHObject(ObjectExport.TypeInfo.TypeName);

		// serialize real object
		(*FileArchive) << NewObject;
	}

	return true;
}