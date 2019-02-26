#pragma once

class DatabaseHandler
{
private:
	MYSQL MySQL;
	//MYSQL* MySQL_Address;
	//MYSQL_RES MySQL_Result;
	MYSQL_RES* MySQL_ResultAddress;
	MYSQL_ROW MySQL_Row;

	volatile BOOL QueryEnqueuing;
	


	wstring MySQL_ServerIP_Address;
	wstring MySQL_UserName;
	wstring MySQL_Password;
	wstring MySQL_DatabaseName;

	UINT MySQL_ServerPort;

	wstring CurrentQueryText;



	UINT LastErrorNumber;
	wstring LastErrorText;



	UINT QueryCount; //TransactionCount
	UINT CurrentQueryCount; //QueryCountPerSecond

	UINT CurrentDisconnectionCount;




	//WCHAR MySQL_ServerIP_Address[16];
	//WCHAR MySQL_UserName[64];
	//WCHAR MySQL_Password[64];
	//WCHAR MySQL_DatabaseName[64];
	//
	//UINT DatabasePort;
	//
	//WCHAR Query[MAXIMUM_QUERY_LENGTH];
	//char UTF8Query[MAXIMUM_QUERY_LENGTH];
	//
	//
	//
	//UINT LastErrorNumber;
	//WCHAR LastErrorText[128];

public:
	DatabaseHandler();
	virtual ~DatabaseHandler();

	bool Initialize();
	//bool Update();
	//bool Render();
	bool Terminalize();



	bool Connect();
	bool Disconnect();

	bool Query(WCHAR* queryText);
	bool SaveQuery(WCHAR* queryText);

	MYSQL_ROW FetchRow();

	void FreeResult();



	MYSQL_ROW GetMySQL_Row() { return MySQL_Row; }
	UINT GetLastErrorNumber() { return LastErrorNumber; }
	WCHAR* GetLastErrorText() { return (WCHAR*)LastErrorText.c_str(); }
};