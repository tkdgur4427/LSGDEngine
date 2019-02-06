#include "HEnginePCH.h"
#include "HShaderCompilingManager.h"

// material common
#include "HMaterialCommon.h"

// shader compiler utils
#include "HShaderCompilerUtil.h"

using namespace lsgd;

HShaderCompilingManager* lsgd::GShaderCompilerManager = nullptr;
uint32 lsgd::GlobalShaderMapId = 0;

HShaderCompileJob* HShaderCommonCompileJob::GetSingleShaderJob() const
{
	if (DynamicCastFlag == HShaderCommonCompileJob::ShaderCompileJob)
	{
		return (HShaderCompileJob*)this;
	}
	return nullptr;
}

HShaderCompilingManager::HShaderCompilingManager()
	: bAllowAsynchronousShaderCompiles(false) // currently, set the default value as false
{

}

HShaderCompilingManager::~HShaderCompilingManager()
{

}

void HShaderCompilingManager::Initialize()
{

}

void HShaderCompilingManager::Destroy()
{

}

void HShaderCompilingManager::AddJobs(HArray<HShaderCommonCompileJob*> NewJobs)
{
	HScopedLock Lock(CompileQueueSyncObject);

	// add the jobs
	for (auto& Job : NewJobs)
	{
		CompileQueue.push_back(Job);
	}

	// @todo processing pipeline job...
}

void HShaderCompilingManager::FinishCompilation(const HString& MaterialName, const HArray<uint32>& ShaderMapIdsToFinishCompiling)
{
	HHashMap<int32, HShaderMapFinalizeResults> CompiledShaderMaps;
	//CompiledShaderMaps.Append(PendingFinalizeShaderMaps);
	BlockOnShaderCompletion(ShaderMapIdsToFinishCompiling, CompiledShaderMaps);

	//...

	ProcessCompiledShaderMaps(CompiledShaderMaps, FLT_MAX);
}

void HShaderCompilingManager::ProcessCompiledShaderMaps(HHashMap<int32, HShaderMapFinalizeResults>& CompiledShaderMaps, float TimeBudget)
{
	// keeps shader maps alive as they are passed from the shader compiler and applied to the owning HMaterial
	//HArray<HRefCountPtr<HMaterialShaderMap>> LocalMaterialMapReferences;
	//HHashMap<HMaterial*, HMaterialShaderMap*> MaterialToUpdate;
	//HHashMap<HMaterial*, HMaterialShaderMap*> MaterialToApplyToScene;

	// process compiled shader maps in FIFO order, in case a shader map has been enqueued multiple times
	// which can happen if a material is edited while a background compile is going on
	for (auto& ProcessIt : CompiledShaderMaps)
	{
		//HRefCountPtr<HMaterialShaderMap> ShaderMap;
		//HArray<HMaterial*>* Materials = NULL;

		if (false/*ShaderMap && Materials*/)
		{
			//...
		}

		// @todo - 
		else if (ProcessIt.first == GlobalShaderMapId)
		{
			auto GlobalShaderResultIter = CompiledShaderMaps.find(GlobalShaderMapId);
			if (GlobalShaderResultIter != CompiledShaderMaps.end())
			{
				HShaderMapFinalizeResults* GlobalShaderResult = (HShaderMapFinalizeResults*)&GlobalShaderResultIter->second;
				const HArray<HShaderCommonCompileJob*>& CompilationResults = GlobalShaderResult->FinishedJobs;

				HShaderCompilerUtil::ProcessCompiledGlobalShaders(CompilationResults);

				// release compile jobs
				for (int32 ResultIndex = 0; ResultIndex < CompilationResults.size(); ++ResultIndex)
				{
					delete CompilationResults[ResultIndex];
				}

				CompiledShaderMaps.erase(GlobalShaderMapId);
			}
		}
	}

	//...
}

void HShaderCompilingManager::BlockOnShaderCompletion(const HArray<uint32>& ShaderIdsToFinishCompiling, HHashMap<int32, HShaderMapFinalizeResults>& CompiledShaderMaps)
{
	//if (bAllowAsynchronousShaderCompiles)
	{
		int32 NumPendingJobs = 0;
		do
		{
			// lock compile queue section so we can access the input and output queues
			HScopedLock Lock(CompileQueueSyncObject);

			for (auto& ShaderMapId : ShaderIdsToFinishCompiling)
			{
				auto ResultIter = ShaderMapJobs.find((int32)ShaderMapId);
				if (ResultIter != ShaderMapJobs.end())
				{
					auto& Result = ResultIter->second;
					int32 FinishedJobs = Result.FinishedJobs.size();

					if (FinishedJobs == Result.NumJobsQueued)
					{
						CompiledShaderMaps.insert({ShaderIdsToFinishCompiling[ShaderMapId], HShaderMapFinalizeResults()});
						ShaderMapJobs.erase(ShaderIdsToFinishCompiling[ShaderMapId]);
					}
					else
					{
						NumPendingJobs += Result.NumJobsQueued;
					}
				} 
			}

			if (NumPendingJobs > 0)
			{
				HGenericPlatformMisc::Sleep(0.01f);
			}
		} while (NumPendingJobs > 0);

	}
	//else
	{
		//...
	}
}

int32 HShaderCompileThreadRunnable::PullTasksFromQueue()
{
	int32 NumActiveThreads = 0;
	{
		// enter the CS so we can access the input and output queues
		HScopedLock Lock(Manager->CompileQueueSyncObject);

		const int32 NumWorkersToFeed = Manager->NumShaderCompilingThreads;

		for (int32 WorkerIndex = 0; WorkerIndex < WorkerInfos.size(); ++WorkerIndex)
		{
			HShaderCompileWorkerInfo& CurrentWorkerInfo = *WorkerInfos[WorkerIndex];

			// if this worker doesn't have any queued jobs, look for more in the input queue
			if (CurrentWorkerInfo.QueuedJobs.size() == 0 && WorkerIndex < NumWorkersToFeed)
			{
				check(!CurrentWorkerInfo.bComplete);

				if (Manager->CompileQueue.size() > 0)
				{
					int32 JobIndex = 0;

					// try grab up to MaxShaderJobBatchSize jobs
					for (; JobIndex < Manager->MaxShaderJobBatchSize && JobIndex < Manager->CompileQueue.size(); ++JobIndex)
					{
						CurrentWorkerInfo.QueuedJobs.push_back(Manager->CompileQueue[JobIndex]);
					}

					// update the worker state as having new tasks that need to be issued
					// don't reset worker app id, because the shadercompileworkers dont shutdown immeidately after finishing a single job queue
					CurrentWorkerInfo.bIssuedTasksToWorker = false;
					CurrentWorkerInfo.bLaunchWorker = false;
					NumActiveThreads++;

					auto StartIter = Manager->CompileQueue.begin();
					Manager->CompileQueue.erase(StartIter, StartIter + JobIndex);
				}

				else // when compilequeue is empty
				{
					if (CurrentWorkerInfo.QueuedJobs.size() > 0)
					{
						NumActiveThreads++;
					}

					// add completed jobs to the output queue, which is ShaderMapJobs
					if (CurrentWorkerInfo.bComplete)
					{
						for (int32 JobIndex = 0; JobIndex < CurrentWorkerInfo.QueuedJobs.size(); ++JobIndex)
						{
							auto ResultIter = Manager->ShaderMapJobs.find(CurrentWorkerInfo.QueuedJobs[JobIndex]->Id);
							check(ResultIter != Manager->ShaderMapJobs.end());

							HShaderMapCompileResults& ShaderMapResults = ResultIter->second;
							ShaderMapResults.FinishedJobs.push_back(CurrentWorkerInfo.QueuedJobs[JobIndex]);
							ShaderMapResults.bAllJobsSucceeded = ShaderMapResults.bAllJobsSucceeded && CurrentWorkerInfo.QueuedJobs[JobIndex]->bSuccessed;
						}

						HGenericPlatformAtomics::HInterlockedAdd(&Manager->NumOutstandingJobs, -(int32)(CurrentWorkerInfo.QueuedJobs.size()));
						CurrentWorkerInfo.bComplete = false;
						CurrentWorkerInfo.QueuedJobs.clear();
					}
				}
			}
		}
	}

	return NumActiveThreads;
}

void HShaderCompileThreadRunnable::CompileDirectlyThroughDll()
{
	// enter the CS so we can access the input and output queues
	HScopedLock Lock(Manager->CompileQueueSyncObject);

	const int32 NumWorkersToFeed = Manager->NumShaderCompilingThreads;

	for (int32 WorkerIndex = 0; WorkerIndex < WorkerInfos.size(); ++WorkerIndex)
	{
		HShaderCompileWorkerInfo& CurrentWorkerInfo = *WorkerInfos[WorkerIndex];

		if (CurrentWorkerInfo.bLaunchWorker == false)
		{
			CurrentWorkerInfo.bLaunchWorker = true;

			for (auto& Job : CurrentWorkerInfo.QueuedJobs)
			{
				HShaderCompileJob* CompileJob = Job->GetSingleShaderJob();
				check(CompileJob);

				HString WorkingDirectory("..//Shaders//");
				HShaderCompilerUtil::ProcessCompilationJob(CompileJob->Input, CompileJob->Output, WorkingDirectory);
			}

			CurrentWorkerInfo.bComplete = true;
			CurrentWorkerInfo.bLaunchWorker = false;
		}
	}
}

int32 HShaderCompileThreadRunnable::CompilingLoop()
{
	// grab more shader compile jobs from the input queue, and move completed jobs to Manager->ShaderMapJobs
	const int32 NumActiveThreads = PullTasksFromQueue();

	if (NumActiveThreads == 0 && Manager->bAllowAsynchronousShaderCompiles)
	{
		HGenericPlatformMisc::Sleep(0.01f);
	}

	if (Manager->bAllowAsynchronousShaderCompiles)
	{
		// write out the files which are input to the shader compile workers
		// WriteNewTasks();

		// launch shader compile workers if they are not already running workers can time out when idle so they may be need to relaunched
		/*
			bool bAbandonWorkers = LaunchWorkersIfNeeded();
			if (bAbandonWorkers)
			{
				// error happens
			}
			else
			{
				// read files which are outputs from the shader compile worker
				ReadAvailableResults();
			}
		*/
	}
	else
	{
		// compile directly through dll
		CompileDirectlyThroughDll();
	}

	return NumActiveThreads;
}