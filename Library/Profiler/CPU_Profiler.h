#pragma once

class CPU_Profiler
{
private:
	HANDLE ProcessHandle;
	int ProcessorCount;

	float KernelProcessorUsage;
	float UserProcessorUsage;
	float TotalProcessorUsage;

	float KernelProcessUsage;
	float UserProcessUsage;
	float TotalProcessUsage;

	ULARGE_INTEGER PreviousKernelProcessorTime;
	ULARGE_INTEGER PreviousUserProcessorTime;
	ULARGE_INTEGER PreviousIdleProcessorTime;

	ULARGE_INTEGER PreviousKernelProcessTime;
	ULARGE_INTEGER PreviousUserProcessTime;
	ULARGE_INTEGER PreviousProcessTime;

	bool ShowProfiler;

public:
	CPU_Profiler();
	virtual ~CPU_Profiler();

	bool Initialize();
	bool Update();
	bool Render();
	bool Terminalize();

	float GetKernelProcessorUsage() { return KernelProcessorUsage; }
	float GetUserProcessorUsage() {	return UserProcessorUsage; }
	float GetTotalProcessorUsage() { return TotalProcessorUsage; }

	float GetKernelProcessUsage() { return KernelProcessUsage; }
	float GetUserProcessUsage() { return UserProcessUsage; }
	float GetTotalProcessUsage() { return TotalProcessUsage; }
};

extern CPU_Profiler CPU_PROFILER;