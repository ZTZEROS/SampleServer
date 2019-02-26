#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "GameLog.h"

GameLog::GameLog()
{
	GameLogFilePathW.clear();
}

GameLog::~GameLog()
{
}

bool GameLog::WriteGameLog(LPCWCHAR type, LOG_LEVEL_INDEX logLevelIndex, LPCWCHAR systemlogFormat, ...)
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

	GameLogFilePathW += L"GameLog";
	GameLogFilePathW += to_wstring(currentTime.tm_year + 1900);
	GameLogFilePathW += to_wstring(currentTime.tm_mon + 1);
	//GameLogFilePathW += to_wstring(currentTime.tm_mday);
	GameLogFilePathW += L".txt";



	_wfopen_s(&logFile, GameLogFilePathW.c_str(), L"a, ccs=UNICODE");

	//fprintf(logFile, logFormat);
	fputws(logText, logFile);

	fclose(logFile);

	return 1;
}