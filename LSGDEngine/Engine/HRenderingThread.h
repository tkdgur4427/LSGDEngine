#pragma once

// async
#include "..\Core\HTaskGraphInterface.h"
#include "..\Core\HTaskThreadBase.h"

extern void InitializeRenderingThread();
extern void DestroyRenderingThread();

namespace lsgd
{
	class HRenderCommand
	{
	public:
	};
}

#define TASK_FUNCTION(Code) \
	void Execute() \
	{ \
		Code; \
	}

#define TASKNAME_FUNCTION(TypeName) \
	HString GetTaskName() \
	{ \
		return #TypeName; \
	}

template <typename LambdaType>
class TEnqueueUniqueRenderCommandType : public lsgd::HRenderCommand
{
public:
	TEnqueueUniqueRenderCommandType(LambdaType&& InLambda)
		: Lambda(InLambda)
	{}

	void Execute()
	{
		Lambda();
	}

private:
	LambdaType Lambda;
};


template <typename LambdaType>
void EnqueueUniqueRenderCommand(LambdaType&& InLambda)
{
	using EURCType = TEnqueueUniqueRenderCommandType<LambdaType>;
	HArray<shared_ptr<lsgd::async::HGraphEvent>> Tasks;
	lsgd::async::HGraphTask<EURCType>::CreateTask(Tasks, true, "RenderingThread").ConstructAndDispatchWhenReady(forward<LambdaType>(InLambda));
}

#define ENQUEUE_UNIQUE_RENDER_COMMAND(TypeName, Code) \
	class EURC_##TypeName : public lsgd::HRenderCommand \
	{ \
	public:	\
		TASKNAME_FUNCTION(TypeName) \
		TASK_FUNCTION(Code) \
	}; \
	{ \
		/*log the command @todo*/ \
		HArray<shared_ptr<lsgd::async::HGraphEvent>> Tasks; \
		lsgd::async::HGraphTask<EURC_##TypeName>::CreateTask(Tasks, true, "RenderingThread").ConstructAndDispatchWhenReady(); \
	}

#define ENQUEUE_UNIQUE_RENDER_COMMAND_ONE_PARAMETER(TypeName, ParamType1, ParamName1, ValueName1, Code) \
	class EURC_##TypeName : public lsgd::HRenderCommand \
	{ \
	public:	\
		EURC_##TypeName(ParamType1 _ParamName1) \
			: ParamName1(_ParamName1) \
		{} \
		TASKNAME_FUNCTION(TypeName) \
		TASK_FUNCTION(Code) \
		ParamType1 ParamName1; \
	}; \
	{ \
		/*log the command @todo*/ \
		HArray<shared_ptr<lsgd::async::HGraphEvent>> Tasks; \
		lsgd::async::HGraphTask<EURC_##TypeName>::CreateTask(Tasks, true, "RenderingThread").ConstructAndDispatchWhenReady(ValueName1); \
	}