#pragma once
class GameLog
{
private:
	wstring GameLogFilePathW;
	string GameLogFilePathA;

public:
	GameLog();
	virtual ~GameLog();

	bool WriteGameLog(LPCWCHAR type, LOG_LEVEL_INDEX logLevelIndex, LPCWCHAR systemlogFormat, ...);
	//bool WriteGameLog(wstring type, LOG_LEVEL_INDEX logLevelIndex, wstring systemlogFormat);

	void SetGameLogFilePath(WCHAR* gameLogFilePath) { GameLogFilePathW = gameLogFilePath; }
};

