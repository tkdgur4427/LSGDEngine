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

void HNameEntry::Init(const char* InName, int32 InNumber)
{
	check((InNumber >> MaxNumberSize) == 0);

	char PostfixNumber[MaxNumberSize + 1] = { '_', };
	HCString::Itoa(InNumber, PostfixNumber + 1);

	int32 NumberLen = HCString::Strlen(PostfixNumber);
	int32 NameLen = HCString::Strlen(InName);
	check(NumberLen + NameLen < MaxNameSize);

	HCString::Strcpy(NameANSI, InName);
	HCString::Strcpy(NameANSI + NameLen, PostfixNumber);
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

HName::HName()
	: NameEntryIndex(-1)
	, Number(0)
{

}

HName::HName(const char* InName)
{
	HNameEntryManager::GetSingleton().AddNameEntry(InName, 0, NameEntryIndex, Number);
}

reflect::HReflectionContext& operator<<(reflect::HReflectionContext& InContext, HName& Value)
{
	if (InContext.IsSaving())
	{
		
	}
	else
	{

	}

	return InContext;
}
