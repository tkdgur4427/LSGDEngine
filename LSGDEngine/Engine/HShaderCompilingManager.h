#pragma once

#include "HShaderCompilerEnvironment.h"
#include "../Core/HThreadRunnable.h"

using namespace lsgd::thread;

namespace lsgd
{
	// stores all of the common information used to compile a shader or pipeline
	class HShaderCommonCompileJob : public HRefCountedObject
	{
	public:
		HShaderCommonCompileJob() {}
		virtual ~HShaderCommonCompileJob() {}

		// id of the shader map this shader belongs to
		uint32 Id;
		// true if the results of the shader compile have been processed
		bool bFinalized;
		// output of the shader compile
		bool bSuccessed;
	};

	// struct that gathers all readonly inputs needed for the compilation of a single header
	struct HShaderCompilerInput
	{
		HShaderTarget Target;
		HString ShaderFormat;
		HString SourceFilePrefix;
		// e.g. BasePassPixelShader, ReflectionEnvironmentShaders, SlateElementPixelShader, ...
		HString SourceFilename;
		HString EntryPointName;

		// skip the preprocessor and instead loads the usf file directly
		bool bSkipPreprocessorCache;
		bool bGenerateDirectCompileFile;

		// shader pipeline information
		bool bCompilingForShaderPipeline;
		bool bIncludeUsedOutputs;
		HArray<HString> UsedOutputs;

		// dump debug path (up to platform) e.g. "D:/MMittring-Z3941-A/UE4-Orion/OrionGame/Saved/ShaderDebugInfo/PCD3D_SM5"
		HString DumpDebugInfoRootPath;
		HString DumpDebugInfoPath;

		// material name or 'global' for debugging and better error messages
		HString DebugGroupName;

		// compilation environment
		HShaderCompilerEnvironment Environment;
		HRefCountPtr<HShaderCompilerEnvironment> SharedEnvironment;
	};

	// a shader compiler error or warning
	struct HShaderCompilerError
	{
		HString ErrorFile;
		HString ErrorLineString;
		HString StrippedErrorMessage;
	};

	class HShaderCode
	{
		// -1 if ShaderData was finalized
		mutable int32 OptionalDataSize;
		// access through class methods
		mutable HArray<uint8> ShaderCodeWithOptionalData;
	};

	// the output of the shader compiler
	struct HShaderCompilerOutput
	{
		HShaderParameterMap ParameterMap;
		HArray<HShaderCompilerError> Errors;
		HShaderTarget Target;
		HShaderCode ShaderCode;
		// FSHAHash OutputHash
		uint32 NumInstructions;
		uint32 NumTextureSamplers;
		bool bSuccessed;
		bool bFailedRemovingUnused;
		bool bSupportsQueryingUsedAttributes;
		HArray<HString> UsedAttributes;
	};

	// stores all of the input and output information used to compile a single shader
	class HShaderCompileJob : public HShaderCommonCompileJob
	{
	public:
		HShaderCompileJob(class HShaderType* InShaderType)
			: ShaderType(InShaderType)
		{}

		virtual ~HShaderCompileJob() {}

		class HVertexFactoryType* VFType;	// VF type that this shader belongs to, may be NULL
		class HShaderType* ShaderType;		// shader type that this shader belongs to, must be VALID

		// input & output for the shader compile
		HShaderCompilerInput Input;
		HShaderCompilerOutput Output;

		// list of pipelines that are sharing the job
		HHashMap<const class HVertexFactoryType*, HArray<const class HShaderPipeline*>> SharingPipelines;
	};

	class HShaderPipelineCompileJob : public HShaderCommonCompileJob
	{
	public:
		HArray<HShaderCommonCompileJob*> StageJobs;
		bool bFailedRemovingUnused;

		// shader pipeline that this shader belongs to, may (currently) be NULL
		const class HShaderPipelineType* ShaderPipeline;
	};

	// results for a single compiled shader map
	struct HShaderMapCompileResults
	{
		int32 NumJobsQueued;
		bool bAllJobsSucceeded;
		bool bApplyCompletedShaderMapForRendering;
		bool bRecreateComponentRenderStateOnCompletion;
		HArray<HShaderCommonCompileJob*> FinishedJobs;
	};

	// results for a single compiled and finalized shader map
	struct HShaderMapFinalizeResults : public HShaderMapCompileResults
	{
		// tracks finalization progress on this shader map
		int32 FinalizeJobIndex;

		// list of pipelines with shared shaders; nullptr for non-mesh pipeline
		HHashMap<const class HVertexFactoryType*, HArray<const class HShaderPipelineType*>> SharedPipelines;
	};

	class HShaderCompileThreadRunnableBase : public HThreadRunnable
	{
	public:
		HShaderCompileThreadRunnableBase()
			: HThreadRunnable("ShaderCompileThreadRunnable")
			, bForceFinish(false)
		{}
		virtual ~HShaderCompileThreadRunnableBase() {}

		virtual void Run() override {}
		virtual void Terminate() override {}

		// the manager for this thread
		class HShaderCompilingManager* Manager;
		// if the thread has been terminated by an unhandled exception, this contains the error message
		HString ErrorMessage;
		bool bTerminateByError;

		volatile bool bForceFinish;
	};

	/*
		manager of asynchronous and parallel shader compilation
		this class contains an interface to enqueue and retrieve asynchronous shader jobs, and manages a HShaderCompileRunnable
	*/
	class HShaderCompilingManager
	{
	public:
		HShaderCompilingManager();
		~HShaderCompilingManager();

		void Initialize();
		void Destroy();

		// *** thread shared properties; these variables can only be read from or written to when a lock on CompileQueueSection is obtained
		// tracks whether we are compiling while the game is running; if true, we need to throttle down shader compiling CPU usage to avoid starving the runtime threads
		bool bCompilingDuringGame;
		// queue of tasks that haven't been assigned to a worker yet
		HArray<HShaderCommonCompileJob*> CompileQueue;
		// map from shader map Id to the compile results for that map, used to gather compiled results
		HHashMap<int32, HShaderMapCompileResults> ShaderMapJobs;
		// number of jobs currently being compiled; this includes CompileQueue and any jobs that have been assigned to workers but aren't complete yet
		int32 NumOutstandingJobs;
		// CS used to gain access to the variables above
		HCriticalSection CompileQueueSyncObject;
		// *** 

		// *** main thread state; these are only accessed on the main thread and used to track progress
		// map from shader map id to results being finalized; used to track shader finalization over multiple frames
		HHashMap<int32, HShaderMapFinalizeResults> PendingFinalizeShaderMaps;

		// the threads spawned for shader compiling
		shared_ptr<HShaderCompileThreadRunnableBase> Thread;
		// ***

		// *** configuration properties; these are set only on initialization and can be read from either thread
		// number of busy threads to use for shader compiling while loading
		uint32 NumShaderCompilingThreads;
		// number of busy threads to use for shader compiling while in-game
		uint32 NumShaderCompilingThreadDuringGame;
		// largest number of jobs that can be put in the same batch
		uint32 MaxShaderJobBatchSize;
		// process id of UE4
		uint32 ProcessId;
		// whether to allow compiling shaders through the worker application, which allows multiple cores to be used
		bool bAllowCompilingThroughWorkers;
		// whether to allow shaders to compile in the background or to block after each material
		bool bAllowAsynchronousShaderCompiles;
		// whether to log out shader job compilation times on the worker thread; useful for tracking down which global shader is taking a long time
		bool bLogJobCompletionTimes;
		// target execution time for ProcessAsyncResult; larger values speed up async shader map processing but cause more hitchiness while async compiling is happening
		bool ProcessGameThreadTargetTime;

		// base directory where temporary files are wirtten out during multi-core shader compiling
		HString ShaderBaseWorkingDirectory;
		// absolute version of ShaderBaseWorkingDirectory
		HString AbsoluteShaderBaseWorkingDirectory;
		// absolute path to the directory to dump shader debug info to
		HString AbsoluteShaderDebugInfoDirectory;
		// name of the shader worker application
		HString ShaderCompileWorkerName;
		// ***

		double WorkersBusyTime; // tracks the total time that shader workers have been busy since startup useful for profiling the shader compile worker thread time
	};
}