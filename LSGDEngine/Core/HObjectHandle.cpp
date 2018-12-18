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