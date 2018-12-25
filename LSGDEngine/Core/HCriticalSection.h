#pragma once

namespace lsgd
{
	class HPlatformCriticalSection;

	class HCriticalSection
	{
	public:
		HCriticalSection();
		~HCriticalSection();

		void Lock();
		void Unlock();

		unique_ptr<HPlatformCriticalSection> CS;
	};
}