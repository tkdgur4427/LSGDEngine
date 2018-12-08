#pragma once

namespace lsgd
{
	template <typename Type>
	class HSingletonTemplate
	{
	public:
		static Type* GetSingleton()
		{
			static Type Instance;
			return &Instance;
		}		

		virtual ~HSingletonTemplate() {}

	protected:
		HSingletonTemplate() {}	
		HSingletonTemplate(const HSingletonTemplate&) = delete;
		HSingletonTemplate& operator=(const HSingletonTemplate&) = delete;
	};
}