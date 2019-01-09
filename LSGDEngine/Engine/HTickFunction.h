#pragma once

namespace lsgd {

	struct HTickFunctionContext
	{
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
	class HTickFunction
	{
	public:
		HTickFunction();
		virtual ~HTickFunction();

		virtual void ExecuteTick() = 0;

		enum class ETickState : uint8
		{
			Disabled,
			Enabled,
			CoolingDown,
		};

		ETickState TickState;

		// temporary scope of task pointer (whenever it finishes its execution, nullify)
		//	- normally indicates 'HGraphTask<HTickFunctionTask>*'
		void* TaskPointer;

		// prerequisites for this tick function
		HArray<HTickFunctionPrerequisite> Prerequisites;

		// the frequency in seconds at which this tick function will be executed
		float TickInterval;

		// owner of tick task level
		class HTickTaskLevel* TickTaskLevel;
	};

	/*
		note that below class declared in here, but has definitions in separate cpp files
			- e.g. HActorTickFunction in HActor.cpp, HActorComponentTickFunction in HActorTickFunction.cpp
	*/

	class HActorTickFunction : public HTickFunction
	{
	public:
		HActorTickFunction();
		virtual ~HActorTickFunction();

		virtual void ExecuteTick();

		HObjectHandleWeak<class HActor> Target;
	};

	class HActorComponentTickFunction : public HTickFunction
	{
	public:
		HActorComponentTickFunction();
		virtual ~HActorComponentTickFunction();

		virtual void ExecuteTick();

		HObjectHandleWeak<class HActorComponent> Target;
	};
}