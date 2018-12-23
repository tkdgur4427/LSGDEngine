#include "HCorePCH.h"
#include "HName.h"
#include "HReflectionContext.h"

using namespace lsgd;

HNameEntry::HNameEntry(const char* InName, int32 InNumber)
{
	Init(InName, InNumber);
}

bool HNameEntry::operator==(const HNameEntry& Other)
{
	if (HCString::Strcmp(NameANSI, Other.NameANSI) == 0)
	{
		return true;
	}
	return false;
}

void HNameEntry::Decompose(HString& OutString, int32& OutNumber) const
{
	// find '_' position
	int32 NameLen = HCString::Strlen(NameANSI);
	int32 NumberStartLoc = -1;

	for (int32 Index = NameLen - 1; Index >= 0; --Index)
	{
		if (NameANSI[Index] == '_')
		{
			// set the number start location
			NumberStartLoc = Index + 1;
			break;
		}
	}
	check(NumberStartLoc != -1);

	// process number
	HCString::Atoi((char*)NameANSI + NumberStartLoc, OutNumber);

	// process original string
	int32 OriginalNameLen = NumberStartLoc;
	OutString.resize(OriginalNameLen);
	
	// copy substring with MemCopy
	HGenericMemory::MemCopy((void*)OutString.data(), NameANSI, OriginalNameLen - 1);
}

void HNameEntry::Init(const char* InName, int32 InNumber)
{
	check((InNumber >> MaxNumberSize) == 0);

	char PostfixNumber[MaxNumberSize + 1] = { '_', };
	HCString::Itoa(InNumber, PostfixNumber + 1, MaxNumberSize);

	int32 NumberLen = HCString::Strlen(PostfixNumber);
	int32 NameLen = HCString::Strlen(InName);
	check(NumberLen + NameLen < MaxNameSize);

	HCString::Strcpy(NameANSI, MaxNameSize, InName);
	HCString::Strcpy(NameANSI + NameLen, MaxNameSize, PostfixNumber);
}

HNameEntryManager& HNameEntryManager::GetSingleton()
{
	static HNameEntryManager Instance;
	return Instance;
}

void HNameEntryManager::AddNameEntry(const char* InName, int32 InNumber, int32& OutNameEntryIndex, int32& OutNumber)
{
	OutNumber = FindNameEntry(InName, InNumber);
	
	HNameEntry* NameEntry = nullptr;

	// if there is available name entry, get it
	if (FreeNameEntries.size() > 0)
	{
		OutNameEntryIndex = FreeNameEntries.back();
		FreeNameEntries.pop_back();		
	}
	else
	{
		OutNameEntryIndex = NameEntries.size();
		NameEntries.push_back(HNameEntry());		
	}

	NameEntries[OutNameEntryIndex].Init(InName, InNumber);
	UsedNameEntries.push_back(OutNameEntryIndex);
}

int32 HNameEntryManager::FindNameEntry(const char* InName, int32 InNumber)
{
	int32 OutNumber = InNumber;
	
	// note that this algorithm is very very inefficient, but for fast implementation, I choose this way
	//	- NEED TO REFACTOR with HashTable later
	while (true)
	{
		int32 StartNumber = OutNumber;
		for (int32 EntryIndex : UsedNameEntries)
		{
			if (NameEntries[EntryIndex] == HNameEntry(InName, OutNumber))
			{
				OutNumber++;
				break;
			}
		}

		if (StartNumber == OutNumber)
		{
			// successfully find the unique number
			break;
		}
	}
	
	return OutNumber;
}

const HNameEntry* HNameEntryManager::GetNameEntry(int32 Index) const
{
	return &NameEntries[Index];
}

HName::HName()
	: NameEntryIndex(-1)
	, Number(0)
{

}

HName::HName(const char* InName)
{
	HNameEntryManager::GetSingleton().AddNameEntry(InName, 0, NameEntryIndex, Number);
}

HName::HName(const char* InOriginalName, int32 InNumber)
{
	// check if the matched name exists
	int32 FoundNumber = HNameEntryManager::GetSingleton().FindNameEntry(InOriginalName, InNumber);
	check(FoundNumber != InNumber);

	HNameEntryManager::GetSingleton().AddNameEntry(InOriginalName, InNumber, NameEntryIndex, Number);
}

const HNameEntry* HName::GetNameEntry() const
{
	return HNameEntryManager::GetSingleton().GetNameEntry(NameEntryIndex);
}

HString HName::ToString() const
{
	const HNameEntry* NameEntry = GetNameEntry();
	return HString(NameEntry->NameANSI);
}

reflect::HReflectionContext& HName::Serialize(reflect::HReflectionContext& InContext)
{
	HString OriginalName;

	if (InContext.IsSaving())
	{
		const HNameEntry* NameEntry = GetNameEntry();
		// @todo.. um need to think about more optimized way
		NameEntry->Decompose(OriginalName, Number);

		InContext << Number;
		InContext << OriginalName;
	}
	else
	{
		InContext << Number;
		InContext << OriginalName;

		// add new name
		(*this) = HName(OriginalName.c_str(), Number);
	}

	return InContext;
}
