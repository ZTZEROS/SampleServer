#include "FirstOrderLibrary.h"
#include "CPU_Profiler.h"

CPU_Profiler CPU_PROFILER;

CPU_Profiler::CPU_Profiler()
{
	SYSTEM_INFO systemInformation;

	ProcessHandle = GetCurrentProcess();

	GetSystemInfo(&systemInformation);

	ProcessorCount = systemInformation.dwNumberOfProcessors;

	TotalProcessorUsage = 0;
	UserProcessorUsage = 0;
	KernelProcessorUsage = 0;

	TotalProcessUsage = 0;
	UserProcessUsage = 0;
	KernelProcessUsage = 0;

	PreviousKernelProcessorTime.QuadPart = 0;
	PreviousUserProcessorTime.QuadPart = 0;
	PreviousIdleProcessorTime.QuadPart = 0;

	PreviousKernelProcessTime.QuadPart = 0;
	PreviousUserProcessTime.QuadPart = 0;
	PreviousProcessTime.QuadPart = 0;

	ShowProfiler = 0;
}

CPU_Profiler::~CPU_Profiler() {}

bool CPU_Profiler::Initialize()
{
	SYSTEM_INFO systemInformation;

	ProcessHandle = GetCurrentProcess();

	GetSystemInfo(&systemInformation);

	ProcessorCount = systemInformation.dwNumberOfProcessors;

	TotalProcessorUsage = 0.0f;
	UserProcessorUsage = 0.0f;
	KernelProcessorUsage = 0.0f;

	TotalProcessUsage = 0.0f;
	UserProcessUsage = 0.0f;
	KernelProcessUsage = 0.0f;

	PreviousKernelProcessorTime.QuadPart = 0;
	PreviousUserProcessorTime.QuadPart = 0;
	PreviousIdleProcessorTime.QuadPart = 0;

	PreviousKernelProcessTime.QuadPart = 0;
	PreviousUserProcessTime.QuadPart = 0;
	PreviousProcessTime.QuadPart = 0;

	ShowProfiler = 0;

	return 0;
}

bool CPU_Profiler::Update()
{
	ULARGE_INTEGER currentKernelTime;
	ULARGE_INTEGER currentUserTime;
	ULARGE_INTEGER currentIdleTime;
	ULARGE_INTEGER creationTime;
	ULARGE_INTEGER exitTime;
	ULARGE_INTEGER currentTime;

	ULONGLONG kernelDifference;
	ULONGLONG userDifference;
	ULONGLONG idleDifference;
	ULONGLONG totalDifference;
	ULONGLONG timeDifference;




	if (!GetSystemTimes((PFILETIME)&currentIdleTime, (PFILETIME)&currentKernelTime, (PFILETIME)&currentUserTime)) return 1;

	kernelDifference = currentKernelTime.QuadPart - PreviousKernelProcessorTime.QuadPart;
	userDifference = currentUserTime.QuadPart - PreviousUserProcessorTime.QuadPart;
	idleDifference = currentIdleTime.QuadPart - PreviousIdleProcessorTime.QuadPart;

	totalDifference = kernelDifference + userDifference;

	if (!totalDifference)
	{
		UserProcessorUsage = 0.0f;
		KernelProcessorUsage = 0.0f;
		TotalProcessorUsage = 0.0f;
	}
	else
	{
		KernelProcessorUsage = (float)((double)(kernelDifference - idleDifference) / totalDifference * 100.0f);
		UserProcessorUsage = (float)((double)userDifference / totalDifference * 100.0f);
		TotalProcessorUsage = (float)((double)(totalDifference - idleDifference) / totalDifference * 100.0f);
	}

	PreviousKernelProcessorTime = currentKernelTime;
	PreviousUserProcessorTime = currentUserTime;
	PreviousIdleProcessorTime = currentIdleTime;
	


	GetSystemTimeAsFileTime((LPFILETIME)&currentTime);
	GetProcessTimes(ProcessHandle, (LPFILETIME)&creationTime, (LPFILETIME)&exitTime, (LPFILETIME)&currentKernelTime, (LPFILETIME)&currentUserTime);

	kernelDifference = currentKernelTime.QuadPart - PreviousKernelProcessTime.QuadPart;
	userDifference = currentUserTime.QuadPart - PreviousUserProcessTime.QuadPart;
	timeDifference = currentTime.QuadPart - PreviousProcessTime.QuadPart;

	totalDifference = kernelDifference + userDifference;

	KernelProcessUsage = (float)(kernelDifference / (double)ProcessorCount / (double)timeDifference * 100.0f);
	UserProcessUsage = (float)(userDifference / (double)ProcessorCount / (double)timeDifference * 100.0f);
	TotalProcessUsage = (float)(totalDifference / (double)ProcessorCount / (double)timeDifference * 100.0f);

	PreviousKernelProcessTime = currentKernelTime;
	PreviousUserProcessTime = currentUserTime;
	PreviousProcessTime = currentTime;

	return 0;
}

bool CPU_Profiler::Render()
{
	if (ShowProfiler)
	{
		printf("");
	}

	return 0;
}

bool CPU_Profiler::Terminalize()
{
	return 0;
}