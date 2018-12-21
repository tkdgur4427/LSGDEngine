#pragma once

#include "HObjectHandle.h"

namespace lsgd
{
	/*
		Package Index
			- if < 0, it means import package (-PackageIndex - 1)
			- if > 0, it means export package (PackageIndex - 1)
			- if == 0, root package
	*/
	typedef int32 HPackageIndex;

	class HObjectResource
	{
	public:
		// the name of HObject 
		HName ObjectName;

		// location of the resource for this resource's outer
		HPackageIndex OuterIndex;
	};

	// forward declaration
	class HObject;
	class HLinkerLoad;

	/*
		this part is different from UE4
			- I try to be independent from type and object instance
			- need to serialize separately for type info
	*/
	class HTypeInfo
	{
	public:
		// currently, used as class name for HObject
		HString TypeName;
	};

	class HObjectExport : public HObjectResource
	{
	public:	
		int64 SerialSize;
		int64 SerialOffset;

		// type info to serialize
		HTypeInfo TypeInfo;
		HObject* Object;
	};

	class HObjectImport : public HObjectResource
	{
	public:
		// the linker that contains the original HObjectExport resource associated with this import
		HLinkerLoad* SourceLinker;

		// index into SourceLinker's ExportMap index
		int32 SourceIndex;
	};

	class HLinkerTables
	{
	public:
		virtual ~HLinkerTables() {}

		HArray<HObjectImport> ImportMap;
		HArray<HObjectExport> ExportMap;

		// list of dependency lists for each export
		HArray<HArray<HPackageIndex>> DependsMap;
	};

	class HPackageTOC
	{
	public:
		int32 TotalHeaderSize;
		
		int32 NameCount;
		int32 NameOffset;

		int32 ExportCount;
		int32 ExportOffset;

		int32 ImportCount;
		int32 ImportOffset;

		int32 DependsOffset;
	};

	class HLinker : public HLinkerTables
	{
	public:
		virtual ~HLinker() {}

		// the top-level HPackage object for the package associated with this linker
		class HPackage* LinkerRoot;

		// the package table of content
		HPackageTOC TOC;

		// name map
		HArray<HName> NameMap;

		// file name
		HString Filename;
	};
}

namespace lsgd { namespace reflect {

	class HReflectionContext;

} }

extern lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HPackageTOC& InTOC);
extern lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HTypeInfo& InTypeInfo);
extern lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HObjectExport& InExport);
extern lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HObjectImport& InImport);
