#pragma once

#include "HTypeDatabaseUtils.h"

namespace lsgd {

	template <class From, class To>
	struct HCastImpl
	{
		static To* DoCast(HObject* InObject)
		{
			return InObject->IsA<To>() ? (To*)InObject : nullptr;
		}
	};

	template <class From, class To>
	To* HCast(From* Src)
	{
		check(reflect::HTypeDatabaseUtils::template ExistClass<From>());
		check(reflect::HTypeDatabaseUtils::template ExistClass<To>());

		return HCastImpl<From, To>::DoCast(Src);
	}
}