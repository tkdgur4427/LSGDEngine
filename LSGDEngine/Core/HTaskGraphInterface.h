#pragma once

namespace lsgd { namespace async {

	class HGraphEvent;
	class HBaseGraphTask;

	class HTaskGraphInterface
	{
	public:
		virtual void Initialize() = 0;
		virtual void Destroy() = 0;
		virtual void DispatchAndWaitUntilTaskComplete(shared_ptr<HBaseGraphTask>& Task, bool InbNamedThread = false, const HString& NamedThreadName = "") = 0;
	};

	/*
		version 1 for implemented task graph in lsgd
	*/
	class HTaskGraph1 : public HTaskGraphInterface
	{
	public:
		void Initialize();
		void Destroy();

		void DispatchAndWaitUntilTaskComplete(shared_ptr<HBaseGraphTask>& Task, bool InbNamedThread = false, const HString& NamedThreadName = "");

	protected:

	};
} }

extern lsgd::async::HTaskGraphInterface* GTaskGraphInterface;