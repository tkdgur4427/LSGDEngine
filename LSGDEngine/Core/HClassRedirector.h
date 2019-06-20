#pragma once

namespace lsgd
{
	class HClassRedirector : public HSingletonTemplate<HClassRedirector>
	{
	public:
		HClassRedirector() {}
		virtual ~HClassRedirector() {}

		void SetClassRedirectMap(const HHashMap<HString, HString>& InClassRedirectMap)
		{
			ClassRedirectMap = InClassRedirectMap;
		}

		const HString& GetRedirectedClassName(const HString& InClassName)
		{
			auto FoundIter = ClassRedirectMap.find(InClassName);
			if (FoundIter != ClassRedirectMap.end())
			{
				return FoundIter->second;
			}
			return InClassName;
		}

		HHashMap<HString, HString> ClassRedirectMap;
	};
}