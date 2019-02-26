#include "FirstOrderLibrary.h"
#include "Profiler.h"

Profiler PROFILER;

Profiler::Profiler() {}
Profiler::~Profiler() {}

bool Profiler::Initialize()
{
	QueryPerformanceFrequency(&Frequency);

	memset(&Head, 0, sizeof(Head));
	
	//ProfilingResultAddress = fopen("ProfilingResult.txt", "wb");
	fopen_s(&ProfilingResultAddress, "ProfilingResult.txt", "wb");

	fprintf(ProfilingResultAddress, "[Block name] [Accumulated clock count] [Call count] [Average clock count] [Average elapsed time(ms)]" CRLF);

	IsProfiling = 1;

	return 1;
}

bool Profiler::Terminalize()
{
	Profile* previousProfileAddress;
	Profile* currentProfileAddress;

	previousProfileAddress = nullptr;
	currentProfileAddress = &Head;

	while (currentProfileAddress->NextProfileAddress)
	{
		previousProfileAddress = currentProfileAddress;
		currentProfileAddress = currentProfileAddress->NextProfileAddress;

		fprintf(ProfilingResultAddress, "[%s] [%lld] [%lld] [%lld] [%lld]" CRLF,
			currentProfileAddress->BlockName, currentProfileAddress->AccumulatedPerformanceCount.QuadPart,
			currentProfileAddress->CallCount.QuadPart, currentProfileAddress->AveragePerformanceCount.QuadPart,
			1000 * currentProfileAddress->AveragePerformanceCount.QuadPart / Frequency.QuadPart);

		free(previousProfileAddress);
	}

	fclose(ProfilingResultAddress);

	memset(&Frequency, 0, sizeof(Frequency));
	memset(&Head, 0, sizeof(Head));
	ProfilingResultAddress = nullptr;
	IsProfiling = 0;

	return 1;
}

bool Profiler::BeginProfiling(const char* blockName)
{
	if (IsProfiling)
	{
		Profile* currentProfileAddress;

		currentProfileAddress = FindProfile(blockName);

		++(currentProfileAddress->CallCount.QuadPart);

		QueryPerformanceCounter(&(currentProfileAddress->PreviousPerformanceCount));
	}

	return 1;
}

bool Profiler::EndProfiling(const char* blockName)
{
	if (IsProfiling)
	{
		Profile* currentProfileAddress;
		LARGE_INTEGER currentPerformanceCount;

		QueryPerformanceCounter(&currentPerformanceCount);

		currentProfileAddress = FindProfile(blockName);

		if (currentProfileAddress->CallCount.QuadPart < 1) return 1;

		currentProfileAddress->AccumulatedPerformanceCount.QuadPart += (currentPerformanceCount.QuadPart - currentProfileAddress->PreviousPerformanceCount.QuadPart);
		currentProfileAddress->AveragePerformanceCount.QuadPart = currentProfileAddress->AccumulatedPerformanceCount.QuadPart / currentProfileAddress->CallCount.QuadPart; //not required
		//currentProfileAddress->CurrentPerformanceDeviation.QuadPart = currentProfileAddress->AveragePerformanceCount.QuadPart - (currentPerformanceCount.QuadPart - currentProfileAddress->PreviousPerformanceCount.QuadPart); //not required
		//currentProfileAddress->AccumulatedPerformanceSquareDeviation.QuadPart += currentProfileAddress->CurrentPerformanceDeviation.QuadPart * currentProfileAddress->CurrentPerformanceDeviation.QuadPart;
		//currentProfileAddress->PerformanceVariance.QuadPart = currentProfileAddress->AccumulatedPerformanceSquareDeviation.QuadPart / currentProfileAddress->AveragePerformanceCount.QuadPart; //not required
		//currentProfileAddress->PerformanceStandardDeviation.QuadPart = sqrt(currentProfileAddress->PerformanceVariance.QuadPart); //not required

		//FILE* profilingResultAddress;
		//char profilingBuffer;
		//
		//profilingResultAddress = fopen("ProfilingResult.txt", "ab");
	}

	return 1;
}

bool Profiler::ResetProfiler()
{
	Profile* currentProfileAddress;

	currentProfileAddress = &Head;

	while (currentProfileAddress->NextProfileAddress)
	{
		currentProfileAddress = currentProfileAddress->NextProfileAddress;

		currentProfileAddress->CallCount.QuadPart = 0;
		currentProfileAddress->PreviousPerformanceCount.QuadPart = 0;
		currentProfileAddress->AccumulatedPerformanceCount.QuadPart = 0;
		currentProfileAddress->AveragePerformanceCount.QuadPart = 0;
		//currentProfileAddress->CurrentPerformanceDeviation.QuadPart = 0;
		//currentProfileAddress->AccumulatedPerformanceSquareDeviation.QuadPart = 0;
		//currentProfileAddress->PerformanceVariance.QuadPart = 0;
		//currentProfileAddress->PerformanceStandardDeviation.QuadPart = 0;
	}

	IsProfiling = 1;

	return 1;
}

Profile* Profiler::FindProfile(const char* blockName)
{
	Profile* currentProfileAddress;

	currentProfileAddress = &Head;

	while (currentProfileAddress->NextProfileAddress)
	{
		currentProfileAddress = currentProfileAddress->NextProfileAddress;

		if (!strcmp(blockName, currentProfileAddress->BlockName)) return currentProfileAddress;
	}

	//if (!currentProfileAddress->NextProfileAddress)
	{
		currentProfileAddress->NextProfileAddress = (Profile*)malloc(sizeof(Profile));
		currentProfileAddress = currentProfileAddress->NextProfileAddress;

		memset(currentProfileAddress, 0, sizeof(*currentProfileAddress));

		currentProfileAddress->BlockName = blockName;

		return currentProfileAddress;
	}

	return nullptr;
}

bool Profiler::StopProfiler()
{
	IsProfiling = 0;

	return 1;
}