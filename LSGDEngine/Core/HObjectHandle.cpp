#include "HCorePCH.h"
#include "HObjectHandle.h"

#include "HObject.h"
#include "HObjectArray.h"

using namespace lsgd;

HObject* HObjectHelper::GetObject(uint32 Index, uint32 SerialNumber)
{
	return HObjectArray::GetSingleton()->GetObject(Index, SerialNumber);
}

void HObjectHelper::SetAsDestroyed(const HObjectArrayData& InData)
{
	HObjectArray::GetSingleton()->SetAsDestroyed(InData.Index, InData.SerialNumber);
}

void HObjectHelper::SetAsRootSet(const HObjectArrayData& InData)
{
	HObjectArray::GetSingleton()->SetAsRootSet(InData.Index, InData.SerialNumber);
}

void HObjectHelper::UnsetAsRootSet(const HObjectArrayData& InData)
{
	HObjectArray::GetSingleton()->UnsetAsRootSet(InData.Index, InData.SerialNumber);
}

void HObjectHelper::MarkGC(const HObjectArrayData& InData)
{
	HObjectArray::GetSingleton()->MarkGC(InData.Index, InData.SerialNumber);
}

void HObjectHelper::UnMarkGC(const HObjectArrayData& InData)
{
	HObjectArray::GetSingleton()->UnMarkGC(InData.Index, InData.SerialNumber);
}