#include "HCorePCH.h"
#include "HMemoryArchive.h"

using namespace lsgd;
using namespace lsgd::reflect;

HMemoryArchive::HMemoryArchive(HArray<uint8>& InBuffer)
	: Memory(InBuffer)
	, Offset(0)
{

}

HMemoryArchive::~HMemoryArchive()
{

}

HReflectionContext& HMemoryArchive::operator<<(class HName& Value)
{
	check(false); // not implemented;
	return *this;
}

HReflectionContext& HMemoryArchive::operator<<(class HObject*& Value)
{
	check(false); // not implemented;
	return *this;
}

void HMemoryArchive::Serialize(void* Value, int64 Length)
{
	if (bIsSaving)
	{
		uint32 CurrSize = Memory.size();
		uint32 StartOffset = CurrSize;

		CurrSize += Length;
		Memory.resize(CurrSize);

		uint8* DataOffset = Memory.data();
		HGenericMemory::MemCopy(DataOffset + StartOffset, Value, Length);
	}
	else
	{
		uint8* StartAddress = Memory.data() + Offset;
		HGenericMemory::MemCopy(Value, StartAddress, Length);
		
		// update the offset
		Offset += Length;
	}
}
