#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "SystemLog.h"

SystemLog::SystemLog()
{
	SystemLogFilePathW.clear();
}

SystemLog::~SystemLog()
{
}

bool SystemLog::WriteSystemLog(WCHAR* type, LOG_LEVEL_INDEX logLevelIndex, WCHAR* systemlogFormat, ...)
{
	WCHAR logText[1024];
	FILE* logFile;
	time_t currentSecond;
	tm currentTime;

	va_list argumentList;



	memset(logText, 0, sizeof(logText));


	va_start(argumentList, systemlogFormat);
	wvsprintf(logText, systemlogFormat, argumentList);
	//StringCchVPrintf(logText, sizeof(logText), systemlogFormat, argumentList);
	va_end(argumentList);



	time(&currentSecond);

	localtime_s(&currentTime, &currentSecond);

	SystemLogFilePathW += L"SystemLog";
	SystemLogFilePathW += to_wstring(currentTime.tm_year + 1900);
	SystemLogFilePathW += to_wstring(currentTime.tm_mon + 1);
	//GameLogFilePathW += to_wstring(currentTime.tm_mday);
	SystemLogFilePathW += L".txt";



	_wfopen_s(&logFile, SystemLogFilePathW.c_str(), L"a, ccs=UNICODE");

	//fprintf(logFile, logFormat);
	fputws(logText, logFile);

	fclose(logFile);

	return 1;
}