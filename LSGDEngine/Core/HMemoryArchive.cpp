#include "HCorePCH.h"
#include "HMemoryArchive.h"

using namespace lsgd;
using namespace lsgd::reflect;

HMemoryArchive::HMemoryArchive(HArray<uint8>& InBuffer)
	: Memory(InBuffer)
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
	uint32 CurrSize = Memory.size();
	uint32 StartOffset = CurrSize;

	CurrSize += Length;
	Memory.resize(CurrSize);
	
	uint8* DataOffset = Memory.data();
	HGenericMemory::MemCopy(DataOffset + StartOffset, Value, Length);
}
