#pragma once

#pragma comment(lib, "DbgHelp.lib")

#include <DbgHelp.h>
#include <signal.h>
#include <crtdbg.h>
#include <Psapi.h>

class MemoryDumper
{
public:
	static long DumpCount;

public:
	MemoryDumper()
	{
		DumpCount = 0;

		_CrtSetReportMode(_CRT_WARN, 0);
		_CrtSetReportMode(_CRT_ASSERT, 0);
		_CrtSetReportMode(_CRT_ERROR, 0);

		_CrtSetReportHook(CatchReport);
		
		//_CRTDBG_MODE_DEBUG; //C_RUNTIME_DEBUG_MODE_DEBUG
		//_CRTDBG_MODE_FILE;
		//_CRTDBG_MODE_WNDW;
		//_CRTDBG_REPORT_MODE;

		_set_invalid_parameter_handler(CatchInvalidParameter); //_invalid_parameter_handler;
		_set_purecall_handler(CatchPurecall); //_purecall_handler;
		_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

		signal(SIGABRT, CatchSignal); //abnormal termination
		signal(SIGINT, CatchSignal); //Ctrl+C signal
		signal(SIGILL, CatchSignal); //illegal instruction
		signal(SIGFPE, CatchSignal); //floating point error
		signal(SIGSEGV, CatchSignal); //illegal storage access
		signal(SIGTERM, CatchSignal); //termination request

		SetHandlerDump();
	}
	virtual ~MemoryDumper() {};

	static void SelfCrash(void) //Suicide(void) //ForcedCrash(void) //Crash(void)
	{
		int* z = nullptr;

		*z = 2705;
	}

	static LONG WINAPI SaveDumpedException(__in PEXCEPTION_POINTERS pExceptionPointers)
	{
		unsigned int workingMemorySize;
		unsigned int dumpCount;
		SYSTEMTIME currentTime;

		HANDLE processHandle;
		PROCESS_MEMORY_COUNTERS processMemoryCounters;
		WCHAR dumpFileName[MAX_PATH];
		HANDLE dumpFileHandle;



		workingMemorySize = 0;
		dumpCount = InterlockedIncrement(&DumpCount);
		GetLocalTime(&currentTime);
		


		processHandle = GetCurrentProcess();
		if (processHandle)
		{
			if (GetProcessMemoryInfo(processHandle, &processMemoryCounters, sizeof(processMemoryCounters)))
			{
				workingMemorySize = (unsigned int)(processMemoryCounters.WorkingSetSize);
			}

			CloseHandle(processHandle);



			wsprintf(dumpFileName, L"Dump_%d%02d%02d_%02d%02d%02d_%u_%u.dmp", currentTime.wYear, currentTime.wMonth, currentTime.wDay,
				currentTime.wHour, currentTime.wMinute, currentTime.wSecond, dumpCount, workingMemorySize);

			wprintf(CRALF CRALF CRALF L"Critical error %d%02d%02d %02d:%02d:%02d" CRALF, currentTime.wYear, currentTime.wMonth, currentTime.wDay,
				currentTime.wHour, currentTime.wMinute, currentTime.wSecond);

			wprintf(L"saving dump file..." CRALF);



			dumpFileHandle = CreateFile(dumpFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			if (dumpFileHandle != INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION minidumpExceptionInformation;

				minidumpExceptionInformation.ThreadId = GetCurrentThreadId();
				minidumpExceptionInformation.ExceptionPointers = pExceptionPointers;
				minidumpExceptionInformation.ClientPointers = 1;

				MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dumpFileHandle, MiniDumpWithFullMemory, &minidumpExceptionInformation, NULL, NULL);

				CloseHandle(dumpFileHandle);

				wprintf(L"Crash dump saved.");
			}

			return EXCEPTION_EXECUTE_HANDLER;
		}
		else return 0;
	}

	static LONG WINAPI RedirectedSetUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionPointers)
	{
		SaveDumpedException(pExceptionPointers);

		return EXCEPTION_EXECUTE_HANDLER;
	}

	static void SetHandlerDump()
	{
		SetUnhandledExceptionFilter(SaveDumpedException);

		//static CAPIHook apiHook("kerner32.dll", "SetUnhandledExceptionFilter", (PROC)RedirectedSetUnhandledExceptionFilter, 1);
	}

	static int CatchReport(int iReportType, char* message, int* returnValue) //HookReport
	{
		SelfCrash();

		return 1;
	}

	static void CatchInvalidParameter(const wchar_t* expression, const wchar_t* function, const wchar_t* filfile, unsigned int line, uintptr_t pReserved)
	{
		SelfCrash();
	}

	static void CatchPurecall(void)
	{
		SelfCrash();
	}

	static void CatchSignal(int Error)
	{
		SelfCrash();
	}
};

extern MemoryDumper MEMORY_DUMPER;