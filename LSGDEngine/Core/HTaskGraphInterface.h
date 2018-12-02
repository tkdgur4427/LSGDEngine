#pragma once

namespace lsgd { namespace async {

	class HTaskGraphInterface
	{
	public:
		virtual void Initialize() = 0;
		virtual void Destroy() = 0;
	};

	/*
		version 1 for implemented task graph in lsgd
	*/
	class HTaskGraph1 : public HTaskGraphInterface
	{
	public:
		void Initialize();
		void Destroy();

	protected:

	};
} }