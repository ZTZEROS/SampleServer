#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "ThreadLocalDatabaseHandler.h"

ThreadLocalDatabaseHandler::ThreadLocalDatabaseHandler()
{
	TLS_Index = TlsAlloc();

	if (TLS_Index == TLS_OUT_OF_INDEXES) MEMORY_DUMPER.SelfCrash();
}

ThreadLocalDatabaseHandler::~ThreadLocalDatabaseHandler()
{
	TlsFree(TLS_Index);
}

bool ThreadLocalDatabaseHandler::Initialize()
{
	DatabaseHandler* databaseHandlerAddress;

	databaseHandlerAddress = (DatabaseHandler*)TlsGetValue(TLS_Index);

	if (!databaseHandlerAddress)
	{
		databaseHandlerAddress = new DatabaseHandler;

		TlsSetValue(TLS_Index, databaseHandlerAddress);
	}

	databaseHandlerAddress->Initialize();

	return 0;
}

bool ThreadLocalDatabaseHandler::Terminalize()
{
	DatabaseHandler* databaseHandlerAddress;

	databaseHandlerAddress = (DatabaseHandler*)TlsGetValue(TLS_Index);
	
	if (databaseHandlerAddress)
	{
		databaseHandlerAddress->Terminalize();
		
		delete databaseHandlerAddress;
	}

	TlsFree(TLS_Index);

	return 0;
}

bool ThreadLocalDatabaseHandler::Connect()
{
	DatabaseHandler* databaseHandlerAddress;

	databaseHandlerAddress = (DatabaseHandler*)TlsGetValue(TLS_Index);

	if (!databaseHandlerAddress)
	{
		databaseHandlerAddress = new DatabaseHandler;

		TlsSetValue(TLS_Index, databaseHandlerAddress);

		databaseHandlerAddress->Initialize();
	}

	databaseHandlerAddress->Connect();

	return 0;
}

bool ThreadLocalDatabaseHandler::Disconnect()
{
	DatabaseHandler* databaseHandlerAddress;

	databaseHandlerAddress = (DatabaseHandler*)TlsGetValue(TLS_Index);

	if (!databaseHandlerAddress)
	{
		databaseHandlerAddress = new DatabaseHandler;

		TlsSetValue(TLS_Index, databaseHandlerAddress);

		databaseHandlerAddress->Initialize();
	}

	databaseHandlerAddress->Disconnect();

	return 0;
}

bool ThreadLocalDatabaseHandler::Query(WCHAR* queryText)
{
	DatabaseHandler* databaseHandlerAddress;

	databaseHandlerAddress = (DatabaseHandler*)TlsGetValue(TLS_Index);

	if (!databaseHandlerAddress)
	{
		databaseHandlerAddress = new DatabaseHandler;

		TlsSetValue(TLS_Index, databaseHandlerAddress);

		databaseHandlerAddress->Initialize();
	}

	databaseHandlerAddress->Query(queryText);

	return 0;
}

MYSQL_ROW ThreadLocalDatabaseHandler::FetchRow()
{
	DatabaseHandler* databaseHandlerAddress;
	MYSQL_ROW mysqlRow;

	databaseHandlerAddress = (DatabaseHandler*)TlsGetValue(TLS_Index);

	if (!databaseHandlerAddress)
	{
		databaseHandlerAddress = new DatabaseHandler;

		TlsSetValue(TLS_Index, databaseHandlerAddress);

		databaseHandlerAddress->Initialize();
	}

	mysqlRow = databaseHandlerAddress->FetchRow();

	return mysqlRow;
}

void ThreadLocalDatabaseHandler::FreeResult()
{
	DatabaseHandler* databaseHandlerAddress;

	databaseHandlerAddress = (DatabaseHandler*)TlsGetValue(TLS_Index);

	if (!databaseHandlerAddress)
	{
		databaseHandlerAddress = new DatabaseHandler;

		TlsSetValue(TLS_Index, databaseHandlerAddress);

		databaseHandlerAddress->Initialize();
	}

	databaseHandlerAddress->FreeResult();
}