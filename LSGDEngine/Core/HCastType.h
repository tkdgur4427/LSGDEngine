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
		check(HTypeDatabaseUtils::ExistClass<From>());
		check(HTypeDatabaseUtils::ExistClass<To>());

		return HCastImpl::DoCast<From, To>::DoCast(Src);
	}
} }