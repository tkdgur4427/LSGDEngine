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

		// main tick
		void Tick();

	protected:
		unique_ptr<class HBaseEngine> EngineInstance;
		unique_ptr<async::HTaskGraphInterface> TaskGraph;
		bool bTerminate;
	};

}