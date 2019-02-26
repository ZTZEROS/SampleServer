#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "DatabaseHandler.h"

DatabaseHandler::DatabaseHandler()
{
	mysql_init(&MySQL);
	//MySQL_Address = nullptr;
	//mysql_free_result(MySQL_ResultAddress);
	MySQL_ResultAddress = nullptr;

	QueryEnqueuing = 0;



	MySQL_ServerIP_Address.clear();
	MySQL_UserName.clear();
	MySQL_Password.clear();
	MySQL_DatabaseName.clear();

	MySQL_ServerPort = 0;

	CurrentQueryText.clear();



	LastErrorNumber = 0;
	LastErrorText.clear();



	QueryCount = 0;
	CurrentQueryCount = 0;
}

DatabaseHandler::~DatabaseHandler()
{
	mysql_close(&MySQL);
	//MySQL_Address = nullptr;
	//mysql_free_result(MySQL_ResultAddress);
	MySQL_ResultAddress = nullptr;

	QueryEnqueuing = 0;



	MySQL_ServerIP_Address.clear();
	MySQL_UserName.clear();
	MySQL_Password.clear();
	MySQL_DatabaseName.clear();

	MySQL_ServerPort = 0;

	CurrentQueryText.clear();



	LastErrorNumber = 0;
	LastErrorText.clear();



	QueryCount = 0;
	CurrentQueryCount = 0;

	CurrentDisconnectionCount = 0;
}

bool DatabaseHandler::Initialize()
{
	mysql_init(&MySQL);
	//MySQL_Address = nullptr;
	mysql_free_result(MySQL_ResultAddress);
	MySQL_ResultAddress = nullptr;

	QueryEnqueuing = 0;

	//{
	//	FILE* jsonFile;
	//	Document jsonDocument;
	//	CHAR* jsonBuffer;
	//
	//	_wfopen_s(&jsonFile, L"DatabaseHandler.ini", L"r, ccs=UTF8");
	//	jsonBuffer = (CHAR*)malloc(_filelength(_fileno(jsonFile)));
	//	fread_s(jsonBuffer, sizeof(*jsonBuffer), sizeof(*jsonBuffer), 1, jsonFile);
	//	fclose(jsonFile);
	//
	//	jsonDocument.Parse(jsonBuffer);
	//
	//	MySQL_ServerIP_Address = jsonDocument["MySQL_ServerIP_Address"].GetString();
	//	MySQL_UserName = jsonDocument["MySQL_UserName"].GetString();
	//	MySQL_Password = jsonDocument["MySQL_Password"].GetString();
	//	MySQL_DatabaseName = jsonDocument["MySQL_DatabaseName"].GetString();
	//
	//	MySQL_ServerPort = jsonDocument["MySQL_ServerPort"].GetUint();
	//
	//	free(jsonBuffer);
	//}

	{
		FILE* jsonFile;
		GenericDocument<UTF16LE<>> jsonDocument;
		WCHAR* jsonBuffer;
		UINT jsonFileSize;

		_wfopen_s(&jsonFile, L"DatabaseHandler.ini", L"r, ccs=UTF-16LE");

		jsonFileSize = _filelength(_fileno(jsonFile));

		jsonBuffer = (WCHAR*)malloc(jsonFileSize);
		ZeroMemory(jsonBuffer, jsonFileSize);
		fread_s(jsonBuffer, jsonFileSize, jsonFileSize, 1, jsonFile);
		fclose(jsonFile);

		jsonDocument.Parse(jsonBuffer);

		{
			MySQL_ServerIP_Address = jsonDocument[L"MySQL_ServerIP_Address"].GetString();
			MySQL_UserName = jsonDocument[L"MySQL_UserName"].GetString();
			MySQL_Password = jsonDocument[L"MySQL_Password"].GetString();
			MySQL_DatabaseName = jsonDocument[L"MySQL_DatabaseName"].GetString();

			MySQL_ServerPort = jsonDocument[L"MySQL_ServerPort"].GetUint();
		}

		free(jsonBuffer);
	}

	CurrentQueryText.clear();



	LastErrorNumber = 0;
	LastErrorText.clear();



	QueryCount = 0;
	CurrentQueryCount = 0;

	CurrentDisconnectionCount = 0;

	return 0;
}

bool DatabaseHandler::Terminalize()
{
	mysql_close(&MySQL);
	//MySQL_Address = nullptr;
	mysql_free_result(MySQL_ResultAddress);
	MySQL_ResultAddress = nullptr;

	QueryEnqueuing = 0;



	MySQL_ServerIP_Address.clear();
	MySQL_UserName.clear();
	MySQL_Password.clear();
	MySQL_DatabaseName.clear();

	MySQL_ServerPort = 0;

	CurrentQueryText.clear();



	LastErrorNumber = 0;
	LastErrorText.clear();



	QueryCount = 0;
	CurrentQueryCount = 0;

	CurrentDisconnectionCount = 0;

	return 0;
}

bool DatabaseHandler::Connect()
{
	string mysqlServerIP_Address;
	string mysqlUserName;
	string mysqlPassword;
	string mysqlDatabaseName;
	unsigned int mysqlServerPort;

	string lastErrorText;

	mysqlServerIP_Address.assign(MySQL_ServerIP_Address.begin(), MySQL_ServerIP_Address.end());
	mysqlUserName.assign(MySQL_UserName.begin(), MySQL_UserName.end());
	mysqlPassword.assign(MySQL_Password.begin(), MySQL_Password.end());
	mysqlDatabaseName.assign(MySQL_DatabaseName.begin(), MySQL_DatabaseName.end());

	//MySQL_ServerIP_Address.assign(mysqlServerIP_Address.begin(), mysqlServerIP_Address.end());
	//MySQL_UserName.assign(mysqlUserName.begin(), mysqlUserName.end());
	//MySQL_Password.assign(mysqlPassword.begin(), mysqlPassword.end());
	//MySQL_DatabaseName.assign(mysqlDatabaseName.begin(), mysqlDatabaseName.end());

	mysqlServerPort = MySQL_ServerPort;
	
	if (!mysql_real_connect(&MySQL, mysqlServerIP_Address.c_str(), mysqlUserName.c_str(), mysqlPassword.c_str(), mysqlDatabaseName.c_str(), mysqlServerPort, nullptr, NULL))
	{
		LastErrorNumber = mysql_errno(&MySQL);
		lastErrorText = mysql_error(&MySQL);
		lastErrorText.assign(LastErrorText.begin(), LastErrorText.end());
	
		printf("Mysql connection error : %s, %u" CRALF, LastErrorText.c_str(), LastErrorNumber);
	
		return 1;
	}
	else printf("Mysql connection succeeded." CRALF);

	mysql_set_character_set(&MySQL, "utf8");

	return 0;
}

bool DatabaseHandler::Disconnect()
{
	mysql_close(&MySQL);

	return 0;
}

bool DatabaseHandler::Query(WCHAR* queryText)
{
	string currentQueryText;
	string lastErrorText;

	CurrentQueryText = queryText;
	currentQueryText.assign(CurrentQueryText.begin(), CurrentQueryText.end());
	
	switch (mysql_query(&MySQL, currentQueryText.c_str()))
	{
	case 0:
		{
			//LOG.WriteGameLog(L"MySQL", LOG_LEVEL_DEBUG, CurrentQueryText.c_str());
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
			LastErrorNumber = mysql_errno(&MySQL);
			lastErrorText = mysql_error(&MySQL);
			LastErrorText.assign(lastErrorText.begin(), lastErrorText.end());

			LOG.WriteSystemLog(L"MySQL", LOG_LEVEL_ERROR, LastErrorText.c_str());
	
			printf(
				"MySQL query error : %s, %d" CRALF
				"%s" CRALF
				CRALF
				,
				LastErrorText,
				LastErrorNumber,
				currentQueryText.c_str()
				);
	
			FOR(i, 5)
			{
				if(!Connect()) break;
			}
		}
		break;
	default :
		{
			LastErrorNumber = mysql_errno(&MySQL);
			lastErrorText = mysql_error(&MySQL);
			LastErrorText.assign(lastErrorText.begin(), lastErrorText.end());

			LOG.WriteSystemLog(L"MySQL", LOG_LEVEL_ERROR, LastErrorText.c_str());
	
			printf(
				"MySQL query error : %s, %d" CRALF
				"%s" CRALF
				CRALF
				,
				LastErrorText,
				LastErrorNumber,
				currentQueryText.c_str()
				);
		}
		break;
	}

	if (mysql_query(&MySQL, "START TRANSACTION") && mysql_query(&MySQL, currentQueryText.c_str()) && mysql_query(&MySQL, "COMMIT"))
	{
		mysql_query(&MySQL, "ROLLBACK");
	}

	return 0;
}

bool DatabaseHandler::SaveQuery(WCHAR* queryText)
{
	CurrentQueryText = queryText;

	return false;
}

MYSQL_ROW DatabaseHandler::FetchRow()
{
	MySQL_ResultAddress = mysql_store_result(&MySQL);
	if (MySQL_ResultAddress)
	{
		MySQL_Row = mysql_fetch_row(MySQL_ResultAddress);
	}

	return MySQL_Row;
}

void DatabaseHandler::FreeResult()
{
	mysql_free_result(MySQL_ResultAddress);
}
