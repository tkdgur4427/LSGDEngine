#include "HCorePCH.h"
#include "HGCSweepExecutor.h"
#include "HObjectArray.h"

using namespace lsgd;
using namespace lsgd::gc;

void HGCSweepExecutor::Sweep()
{
	for (auto& Object : HObjectArray::GetSingleton()->Objects)
	{
		if (!(Object->IsEmpty()))
		{
			if (!(Object->HasFlags(EObjectItemFlags::MarkGC)))
			{
				// release the object
				Object->Unbind();
			}			
			else
			{
				// UnMarkGC
				Object->UnsetFlag(EObjectItemFlags::MarkGC);
			}
		}
	}
}