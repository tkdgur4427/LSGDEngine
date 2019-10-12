#pragma once

#include "HPixWindows.h"

namespace lsgd
{
	// real profiler data
	struct HProfilerData
	{
		enum {
			MAX_NAME_LEN = 64,
			MAX_DESC_LEN = 192,
		};

		HProfilerData(const char* InName);
		~HProfilerData();

		void UpdateName(const char* InName);
		void UpdateElpasedTime(double InElapsedTime);

		char Name[MAX_NAME_LEN];
		char Desc[MAX_DESC_LEN];

		double Min;
		double Max;
		double Average;
		double Total;

		uint32 SampleCounts;
	};

	// just wrapper node to access profiler data
	class HProfilerNode
	{
	public:
		HProfilerNode() {}
		~HProfilerNode() {}

		// preventing reference cycling! (weak_ptr)
		weak_ptr<HProfilerNode> Parent;
		// children
		HArray<shared_ptr<HProfilerNode>> Children;
		// profiler data
		shared_ptr<HProfilerData> Data;
	};

	class HProfilerTree
	{
	public:
		shared_ptr<HProfilerNode> RootNode;
	};

	class HProfiler : public HSingletonTemplate<HProfiler>
	{
	public:
		shared_ptr<HProfilerData> FindOrAddProfilerData(const char* InName);

		HArray<shared_ptr<HProfilerData>> DataPool;
		HArray<shared_ptr<HProfilerNode>> NodePool;

		// profiler node tree
		HProfilerTree ProfilerTree;

		// global access profiler data
		//	 - profiler data name to profiler data mapper
		HHashMap<HString, shared_ptr<HProfilerData>> ProfilerDataTable;
	};

	class HScopedSimpleProfiler
	{
	public:
		HScopedSimpleProfiler(const char* InName);
		~HScopedSimpleProfiler();

		double StartTime;
		shared_ptr<HProfilerData> CachedProfilerData;
	};
}

#define SGD_SCOPED_SIMPLE_PROFILER(Name) \
	HScopedSimpleProfiler ScopedSimpleProfiler##Name(#Name); \
	HScopedPix ScopedPix##Name(#Name);