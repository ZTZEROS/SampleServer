#pragma once

#ifdef USE_PROFILER
	#define RESET_PROFILER PROFILER.ResetProfiler()
	#define BEGIN_PROFILING(blockName) { PROFILER.BeginProfiling(blockName); }
	#define END_PROFILING(blockName) { PROFILER.EndProfiling(blockName); }
	#define STOP_PROFILER PROFILER.StopProfiler()
#else
	#define RESET_PROFILER
	#define BEGIN_PROFILING(blockName)
	#define END_PROFILING(blockName)
	#define STOP_PROFILER
#endif

struct Profile
{
	const char* BlockName;
	LARGE_INTEGER CallCount;
	LARGE_INTEGER PreviousPerformanceCount;
	LARGE_INTEGER AccumulatedPerformanceCount;
	LARGE_INTEGER AveragePerformanceCount; //not required
	//LARGE_INTEGER CurrentPerformanceDeviation; //not required
	//LARGE_INTEGER AccumulatedPerformanceSquareDeviation;
	//LARGE_INTEGER PerformanceVariance; //not required
	//LARGE_INTEGER PerformanceStandardDeviation; //not required

	Profile* NextProfileAddress;
};

class Profiler
{
private:
	LARGE_INTEGER Frequency; //PerformanceFrequency;
	
	Profile Head;
	//Profile* CurrentProfileAddress;

	FILE* ProfilingResultAddress;

	bool IsProfiling;

public:
	Profiler();
	~Profiler();

	bool Initialize();
	bool Terminalize();

	bool ResetProfiler();
	bool BeginProfiling(const char* blockName);
	bool EndProfiling(const char* blockName);
	bool StopProfiler();

private:
	Profile* FindProfile(const char* blockName);
};

extern Profiler PROFILER;