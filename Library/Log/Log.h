#pragma once

class Log
{
private:
	MYSQL MySQL;

	wstring GameLogFilePath;
	//string GameLogFilePath;
	wstring SystemLogFilePath;
	//string SystemLogFilePath;

public:
	Log();
	virtual ~Log();

	bool Initialize();
	bool Terminalize();

	bool WriteGameLog(LPCWCHAR type, LOG_LEVEL_INDEX logLevelIndex, LPCWCHAR gameLogFormat, ...);
	bool WriteGameLog(LPCWCHAR server, LPCWCHAR type, LPCWCHAR code, LPCWSTR accountkey, LPCWSTR parameter1, LPCWSTR parameter2, LPCWSTR parameter3, LPCWSTR parameter4, LPCWSTR parameterText);
	//bool WriteGameLog(wstring type, LOG_LEVEL_INDEX logLevelIndex, wstring systemLogFormat);

	bool WriteSystemLog(LPCWCHAR type, LOG_LEVEL_INDEX logLevelIndex, LPCWCHAR systemLogFormat, ...);
	//bool WriteSystemLog(wstring* type, LOG_LEVEL_INDEX logLevelIndex, wstring systemLogFormat);

	bool WriteHexadecimalLog(LPCWCHAR type, LOG_LEVEL_INDEX logLevelIndex, LPCWCHAR log, BYTE* byte, int length);



	void SetGameLogFilePath(LPCWCHAR gameLogFilePath) { GameLogFilePath = gameLogFilePath; }
	void SetSystemLogFilePath(LPCWCHAR systemLogFilePath) { SystemLogFilePath = systemLogFilePath; }
};

extern Log LOG;