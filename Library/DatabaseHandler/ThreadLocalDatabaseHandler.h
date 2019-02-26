#pragma once

class ThreadLocalDatabaseHandler
{
private:
	DWORD TLS_Index; //ThreadLocalStorageIndex

public:
	ThreadLocalDatabaseHandler();
	virtual ~ThreadLocalDatabaseHandler();

	bool Initialize();
	//bool Update();
	//bool Render();
	bool Terminalize();



	bool Connect();
	bool Disconnect();

	bool Query(WCHAR* queryText);

	MYSQL_ROW FetchRow();

	void FreeResult();
};