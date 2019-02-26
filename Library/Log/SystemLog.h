#pragma once
class SystemLog
{
private:
	wstring SystemLogFilePathW;
	string SystemLogFilePathA;
	

public:
	SystemLog();
	virtual ~SystemLog();

	bool WriteSystemLog(WCHAR* type, LOG_LEVEL_INDEX logLevelIndex, WCHAR* systemlogFormat, ...);
	//bool WriteSystemLog(wstring* type, LOG_LEVEL_INDEX logLevelIndex, wstring systemlogFormat);

	void SetSystemLogFilePath(WCHAR* systemLogFilePath) { SystemLogFilePathW = systemLogFilePath; }
};

