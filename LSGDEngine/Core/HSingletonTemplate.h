#pragma once

namespace lsgd
{
	template <typename Type>
	class HSingletonTemplate
	{
	public:
		static Type* GetSingleton()
		{
			static HCriticalSectionCallOnce CallOnceSyncObject;
			static unique_ptr<Type> Instance;
			
			if (Instance == nullptr)
			{
				lsgd::thread::HCallOnceLock Lock(CallOnceSyncObject);
				Instance = make_unique<Type>();
			}

			return Instance.get();
		}		

		virtual ~HSingletonTemplate() {}

	protected:
		HSingletonTemplate() {}		
	};
}