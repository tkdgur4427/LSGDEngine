#include "HCorePCH.h"
#include "HProfiler.h"

using namespace lsgd;

HProfilerData::HProfilerData(const char* InName)
	: Min(FLT_MAX)
	, Max(0.0)
	, Average(0.0)
	, Total(0.0)
	, SampleCounts(0)
{
	UpdateName(InName);
}

HProfilerData::~HProfilerData()
{

}

void HProfilerData::UpdateName(const char* InName)
{
	HCString::Strcpy(Name, MAX_NAME_LEN, InName);
}

void HProfilerData::UpdateElpasedTime(double InElapsedTime)
{
	Min = (Min > InElapsedTime) ? InElapsedTime : Min;
	Max = (Max < InElapsedTime) ? InElapsedTime : Max;
	Total += InElapsedTime;
	SampleCounts += 1;

	Average = Total / SampleCounts;
}

shared_ptr<HProfilerData> HProfiler::FindOrAddProfilerData(const char* InName)
{
	HString NameInHString(InName);

	auto ProfilerData = ProfilerDataTable.find(NameInHString);
	if (ProfilerData != ProfilerDataTable.end())
	{
		return ProfilerData->second;
	}

	// add new profile data
	// firstly, add the profiler data to the pool
	shared_ptr<HProfilerData> NewData = HMakeShared<HProfilerData>(InName);
	DataPool.push_back(NewData);

	// next, add the data to table
	ProfilerDataTable.insert({ NameInHString, NewData });

	return NewData;
}

HScopedSimpleProfiler::HScopedSimpleProfiler(const char* InName)
	: StartTime(0.0)
{
	// do not include search time elapsed
	CachedProfilerData = HProfiler::GetSingleton()->FindOrAddProfilerData(InName);

	// record the start time
	StartTime = HGenericPlatformMisc::GetSeconds();
}

HScopedSimpleProfiler::~HScopedSimpleProfiler()
{
	double EndTime = HGenericPlatformMisc::GetSeconds();
	double ElapsedTime = EndTime - StartTime;

	// update elpased time
	CachedProfilerData->UpdateElpasedTime(ElapsedTime);
}