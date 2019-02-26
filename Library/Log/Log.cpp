#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "Log.h"

Log LOG;

Log::Log()
{
	GameLogFilePath.clear();
	SystemLogFilePath.clear();



	mysql_init(&MySQL);

	//MY_SQL_SERVER.MySQL_Address = mysql_real_connect(&MY_SQL_SERVER.MySQL, mysqlServerIP_Address, mysqlUserName, mysqlPassword, mysqlDatabaseName, MY_SQL_SERVER_PORT, nullptr, NULL))
	//if (!mysql_real_connect(&MySQL, mysqlServerIP_Address, mysqlUserName, mysqlPassword, mysqlDatabaseName, MY_SQL_SERVER_PORT, nullptr, NULL))
	//{
	//	printf("Mysql connection error : %s", mysql_error(&MySQL));
	//
	//	return;
	//}

	//mysql_set_character_set(&MySQL, "utf8");
}

Log::~Log()
{

}

bool Log::WriteGameLog(LPCWCHAR type, LOG_LEVEL_INDEX logLevelIndex, LPCWCHAR systemLogFormat, ...)
{
	wstring gameLogFilePath;
	WCHAR logText[1024];
	FILE* logFile;
	time_t currentSecond;
	tm currentTime;

	va_list argumentList;



	gameLogFilePath = GameLogFilePath;
	memset(logText, 0, sizeof(logText));



	va_start(argumentList, systemLogFormat);
	wvsprintf(logText, systemLogFormat, argumentList);
	//StringCchVPrintf(logText, sizeof(logText), systemLogFormat, argumentList);
	va_end(argumentList);



	_wmkdir(gameLogFilePath.c_str());
	


	time(&currentSecond);

	localtime_s(&currentTime, &currentSecond);

	gameLogFilePath += L"GameLog";
	gameLogFilePath += type;
	gameLogFilePath += to_wstring(currentTime.tm_year + 1900);
	gameLogFilePath += currentTime.tm_mon < 10 ? L"0" + to_wstring(currentTime.tm_mon + 1) : to_wstring(currentTime.tm_mon + 1);
	//gameLogFilePath += to_wstring(currentTime.tm_mday);
	gameLogFilePath += L".txt";



	_wfopen_s(&logFile, gameLogFilePath.c_str(), L"a, ccs=UTF-16LE");

	//fprintf(logFile, logFormat);
	fputws(logText, logFile);

	fclose(logFile);

	return 0;
}

bool Log::WriteGameLog(LPCWCHAR server, LPCWCHAR type, LPCWCHAR code, LPCWSTR accountkey, LPCWSTR parameter1, LPCWSTR parameter2, LPCWSTR parameter3, LPCWSTR parameter4, LPCWSTR parameterText)
{
	wstring temporaryQuery;
	string query;

	temporaryQuery = L"INSERT INTO gameLog VALUES(";
	temporaryQuery += server + (wstring)L", ";
	temporaryQuery += code + (wstring)L", ";
	temporaryQuery += accountkey + (wstring)L", ";
	temporaryQuery += parameter1 + (wstring)L", ";
	temporaryQuery += parameter2 + (wstring)L", ";
	temporaryQuery += parameter3 + (wstring)L", ";
	temporaryQuery += parameter4 + (wstring)L", ";
	temporaryQuery += parameterText + (wstring)L")";

	query.append(temporaryQuery.begin(), temporaryQuery.end());

	switch (mysql_query(&MySQL, query.c_str()))
	{
	case 0:
		{
			//printf(query.c_str());
			//printf(CRALF);
		}
		break;
	case CR_SOCKET_CREATE_ERROR:
	case CR_CONNECTION_ERROR:
	case CR_CONN_HOST_ERROR:
	case CR_SERVER_GONE_ERROR:
	case CR_SERVER_HANDSHAKE_ERR:
	case CR_SERVER_LOST:
	case CR_INVALID_CONN_HANDLE:
		{
			printf("Mysql query error : %s, %d", mysql_error(&MySQL), mysql_errno(&MySQL));
			printf(CRALF);

			mysql_init(&MySQL);

			//MY_SQL_SERVER.MySQL_Address = mysql_real_connect(&MY_SQL_SERVER.MySQL, mysqlServerIP_Address, mysqlUserName, mysqlPassword, mysqlDatabaseName, MY_SQL_SERVER_PORT, nullptr, NULL))
			//if (!mysql_real_connect(&MY_SQL_CLIENT.MySQL, MY_SQL_SERVER.MySQL_ServerIP_Address, MY_SQL_SERVER.MySQL_UserName, MY_SQL_SERVER.MySQL_Password, MY_SQL_SERVER.MySQL_DatabaseName, MY_SQL_SERVER_PORT, nullptr, NULL))
			//{
			//	printf("Mysql connection error : %s", mysql_error(&MY_SQL_CLIENT.MySQL));
			//	printf(CRALF);
			//}

			mysql_set_character_set(&MySQL, "utf8");

			//MY_SQL_CLIENT.CurrentStepIndex = MY_SQL_CLIENT_PHASE_CONNECT;
		}
	default:
		{
			printf("Mysql query error : %s, %d", mysql_error(&MySQL), mysql_errno(&MySQL));
			printf(CRALF);

			printf(query.c_str());
			printf(CRALF);
		}
		break;
	}

	//++MY_SQL_CLIENT.TransactionPerSecond;

	return 0;
}

bool Log::WriteHexadecimalLog(LPCWCHAR type, LOG_LEVEL_INDEX logLevelIndex, LPCWCHAR log, BYTE* byte, int length)
{
	wstring hexadecimalLogFilePath;
	WCHAR logText[1024];
	FILE* logFile;
	time_t currentSecond;
	tm currentTime;

	hexadecimalLogFilePath = SystemLogFilePath;
	memset(logText, 0, sizeof(logText));


	
	wsprintf(logText, CRALF L"%s", log);

	//for (unsigned int i = 0; i < length * sizeof(WCHAR); ++++i)
	//{
	//	wsprintf(logText, L"%s%02x%02x", logText, byte[i + 1], byte[i]);
	//}

	//for (unsigned int i = 0; i < length; ++++i)
	//{
	//	wsprintf(logText, L"%s%02x", logText, ((WCHAR*)byte)[i]);
	//}
	
	for (size_t i = wcslen(logText), j = 0; i < sizeof(logText) && j < length * sizeof(WCHAR); i += 5, j += 2)
	{
		logText[i] = L" "[0];
		logText[i + 1] = L"0123456789abcdef"[byte[j+1] >> 4];
		logText[i + 2] = L"0123456789abcdef"[byte[j+1] & 0x0f];
		logText[i + 3] = L"0123456789abcdef"[byte[j] >> 4];
		logText[i + 4] = L"0123456789abcdef"[byte[j] & 0x0f];
	}



	_wmkdir(hexadecimalLogFilePath.c_str()); //add CRALF to logText



	time(&currentSecond);

	localtime_s(&currentTime, &currentSecond);

	hexadecimalLogFilePath += L"HexadecimalLog";
	hexadecimalLogFilePath += type;
	hexadecimalLogFilePath += to_wstring(currentTime.tm_year + 1900);
	hexadecimalLogFilePath += currentTime.tm_mon < 10 ? L"0" + to_wstring(currentTime.tm_mon + 1) : to_wstring(currentTime.tm_mon + 1);
	//hexadecimalLogFilePath += to_wstring(currentTime.tm_mday);
	hexadecimalLogFilePath += L".txt";


	_wfopen_s(&logFile, hexadecimalLogFilePath.c_str(), L"a, ccs=UTF-16LE");

	//fprintf(logFile, logFormat);
	fputws(logText, logFile);

	fclose(logFile);

	return 0;
}

bool Log::WriteSystemLog(LPCWCHAR type, LOG_LEVEL_INDEX logLevelIndex, LPCWCHAR systemLogFormat, ...)
{
	wstring systemLogFilePath;
	WCHAR logText[1024];
	FILE* logFile;
	time_t currentSecond;
	tm currentTime;

	va_list argumentList;



	systemLogFilePath = SystemLogFilePath;
	memset(logText, 0, sizeof(logText));



	va_start(argumentList, systemLogFormat);
	wvsprintf(logText, systemLogFormat, argumentList);
	//StringCchVPrintf(logText, sizeof(logText), systemLogFormat, argumentList);
	va_end(argumentList);



	_wmkdir(systemLogFilePath.c_str());



	time(&currentSecond);

	localtime_s(&currentTime, &currentSecond);

	systemLogFilePath += L"SystemLog";
	systemLogFilePath += type;
	systemLogFilePath += to_wstring(currentTime.tm_year + 1900);
	systemLogFilePath += currentTime.tm_mon < 10 ? L"0" + to_wstring(currentTime.tm_mon + 1) : to_wstring(currentTime.tm_mon + 1);
	//systemLogFilePath += to_wstring(currentTime.tm_mday);
	systemLogFilePath += L".txt";



	_wfopen_s(&logFile, systemLogFilePath.c_str(), L"a, ccs=UTF-16LE");

	//fprintf(logFile, logFormat);
	fputws(logText, logFile);

	fclose(logFile);

	return 0;
}