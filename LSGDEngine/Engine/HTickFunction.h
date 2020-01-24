#pragma once

namespace lsgd { namespace async {

	class HBaseGraphTask;
	class HGraphEvent;
	
} }

namespace lsgd {

	struct HTickFunctionContext
	{
		HTickFunctionContext()
		{
			Reset();
		}

		void Reset()
		{
			DeltaSeconds = 0.0f;
			ThreadName = "";
			World = nullptr;
		}

		// delta time to tick
		float DeltaSeconds;

		// thread name
		HString ThreadName;

		// world
		class HWorld* World;
	};

	class HTickFunctionPrerequisite
	{
	public:
		shared_ptr<class HTickFunction> PrerequisiteTickFunction;
	};

	// note that HTickFunction is abstract class
	class HTickFunction : public enable_shared_from_this<HTickFunction>
	{
	public:
		HTickFunction();
		virtual ~HTickFunction();

		virtual void Execute() = 0;

		// queue tick function
		void QueueTickFunction(class HTickTaskSequencer& TTS, const HTickFunctionContext& InContext);

		// get completion handler
		shared_ptr<async::HGraphEvent> GetCompletionHandle();

		enum class ETickState : uint8
		{
			Disabled,
			Enabled,
			CoolingDown,
		};

		ETickState TickState;

		shared_ptr<async::HBaseGraphTask> TaskPointer;

		// prerequisites for this tick function
		HArray<HTickFunctionPrerequisite> Prerequisites;

		// the frequency in seconds at which this tick function will be executed
		float TickInterval;

		// owner of tick task level
		class HTickTaskLevel* TickTaskLevel;

		// tick function running asynchronously
		bool bRunOnTaskThreads;
	};

	/*
		note that below class declared in here, but has definitions in separate cpp files
			- e.g. HActorTickFunction in HActor.cpp, HActorComponentTickFunction in HActorTickFunction.cpp
	*/

	class HActorTickFunction : public HTickFunction
	{
	public:
		HActorTickFunction(HObjectHandleWeak<class HActor>& InTarget);
		virtual ~HActorTickFunction();

		virtual void Execute();

		HObjectHandleWeak<class HActor> Target;
	};

	class HActorComponentTickFunction : public HTickFunction
	{
	public:
		HActorComponentTickFunction();
		virtual ~HActorComponentTickFunction();

		virtual void Execute();

		HObjectHandleWeak<class HActorComponent> Target;
	};
}