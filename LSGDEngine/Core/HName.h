#pragma once

namespace lsgd
{
	/*
		To-do lists
			1. need to separate HNameEntry and HNameEntryNumber (for memory efficiency)
			2. need to convert HNameEntry attributes with hash-table! (for performance issue)
	*/

	// indicate real name string information
	class HNameEntry
	{
	public:
		HNameEntry() {}
		HNameEntry(const char* InName, int32 InNumber);

		void Decompose(HString& OutOriginName, int32& OutNumber) const;

		enum 
		{
			MaxNameSize = 128,
			MaxNumberSize = 8, // current max number is 99999999
		};

		union 
		{
			// ansi name
			char NameANSI[MaxNameSize];
			// ... we could add other type of string: e.g. wchar_t
		};

		bool operator==(const HNameEntry& Other);

	protected:
		friend class HNameEntryManager;

		void Init(const char* InName, int32 InNumber);
	};

	class HNameEntryManager
	{
	public:
		static HNameEntryManager& GetSingleton();

		void AddNameEntry(const char* InName, int32 InNumber, int32& OutNameEntryIndex, int32& OutNumber);

		// note that if return value is same as InNumber, it means no collision, otherwise it has collision, so return new number
		int32 FindNameEntry(const char* InName, int32 InNumber);

		const HNameEntry* GetNameEntry(int32 Index);

	protected:
		// prevent instantiation except for GetSingleton()
		HNameEntryManager() {}

		// for now, just fast implementation, I simplify management of name entry
		HArray<HNameEntry> NameEntries;
		HArray<uint32> FreeNameEntries;
		HArray<uint32> UsedNameEntries;

		// sync object
		HCriticalSection SyncObject;
	};

	/*
		in lsgd engine, HName is the key identifier for HCoreObject
			- it can be used as reference object of HCoreObject
	*/

	class HName
	{
	public:
		HName();
		HName(const char* InName);
		HName(const char* InOriginalName, int32 InNumber);

		HString ToString() const;

		int32 NameEntryIndex;
		int32 Number;

		reflect::HReflectionContext& Serialize(reflect::HReflectionContext& InContext);

		friend bool operator==(const HName& Name0, const HName& Name1);

	protected:
		const HNameEntry* GetNameEntry() const;

#if LSGD_DEBUG_BUILD
		const HNameEntry* DebugNameEntry;
#endif
	};
}
