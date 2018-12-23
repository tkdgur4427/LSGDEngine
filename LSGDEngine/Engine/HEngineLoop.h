#pragma once

namespace lsgd { namespace async {

	class HTaskGraphInterface;

} }

namespace lsgd {

	class HEngineLoop
	{
	public:
		HEngineLoop();
		~HEngineLoop();

		void Init();
		void Loop();
		void Destroy();

	protected:
		unique_ptr<async::HTaskGraphInterface> TaskGraph;
		bool bTerminate;
	};

}