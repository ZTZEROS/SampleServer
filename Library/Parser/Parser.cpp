#include "FirstOrderLibrary.h"
#include "Parser.h"

Parser PARSER;

Parser::Parser() {}
Parser::~Parser() {}

bool Parser::Initialize()
{
	Delimiter = nullptr;
	FileAddress = nullptr;
	LoadedDatum = nullptr;
	memset(ReadDatum, 0, sizeof(ReadDatum));
	
	Depth = 0;

	Delimiter = L" ,.:;=\t\r\n";
	//Delimiter = " ,.:;=\"/\t\r\n";
	
	return 0;
}

bool Parser::Terminalize()
{
	Delimiter = nullptr;
	FileAddress = nullptr;
	LoadedDatum = nullptr;
	memset(ReadDatum, 0, sizeof(ReadDatum));

	Depth = 0;

	return 0;
}

bool Parser::LoadFile()
{
	unsigned int fileSize;
	/*
	//FileAddress = fopen("../GlobalHeader/GlobalConstant.txt", "rb");
	//FileAddress = fopen("System/GlobalHeader/GlobalConstant.txt", "rb, ccs=UNICODE");
	FileAddress = _wfopen(L"System/GlobalHeader/GlobalConstant.txt", L"rb, ccs=UNICODE");
	*/
	fseek(FileAddress, 0, SEEK_END);
	fileSize = ftell(FileAddress);

	LoadedDatum = (wchar_t*)malloc(fileSize);

	fseek(FileAddress, 0, SEEK_SET);
	fread(LoadedDatum, fileSize, 1, FileAddress);
	
	//fclose(FileAddress);

	return 0;
}

bool Parser::ReadFile()
{
	wchar_t* currentDatum;

	currentDatum = nullptr;
	fseek(FileAddress, 0, SEEK_SET);

	while (!feof(FileAddress))
	{
		/*
		//currentDatum = fgets(ReadDatum, sizeof(ReadDatum), FileAddress);
		currentDatum = fgetws(ReadDatum, sizeof(ReadDatum), FileAddress);

		//currentDatum = strtok(ReadDatum, Delimiter);
		currentDatum = wcstok(ReadDatum, Delimiter);
		*/
		while (currentDatum)
		{
			//if (!strcmp("//", currentDatum)) break;
			if (!wcscmp(L"//", currentDatum)) break;
			
			//if (!strcmp("/*", currentDatum))
			//{
			//	while (strcmp("*/", currentDatum))
			//	{
			//		currentDatum = strtok(nullptr, Delimiter);
			//	}
			//}

			/*
			if (!wcscmp(L"@", currentDatum));
			
			if (!wcscmp(L"{", currentDatum)) ++Depth;
			else if (!wcscmp(L"}", currentDatum)) --Depth;

			//if (!strcmp("\"", &currentDatum[0]) && !strcmp("\"", &currentDatum[strlen(currentDatum) - 1]))
			//{
			//	char textBuffer[256];
			//	
			//	memcpy(textBuffer, &currentDatum[1], strlen(currentDatum) - 2);
			//}

			if (!wcscmp(L"PRIMARY_VIDEO_BUFFER", currentDatum)) int positiveInteger = _wtoi(wcstok(nullptr, Delimiter));
			else if (!wcscmp(L"BACK_VIDEO_BUFFER", currentDatum)) int positiveInteger = _wtoi(wcstok(nullptr, Delimiter));
			else if (!wcscmp(L"VIDEO_BUFFER_COUNT", currentDatum)) int positiveInteger = _wtoi(wcstok(nullptr, Delimiter));
			
			//currentDatum = strtok(nullptr, Delimiter);
			currentDatum = wcstok(nullptr, Delimiter);
			*/
		}
	}

	//fclose(FileAddress);

	return 0;
}

bool Parser::AssignValue()
{
	wchar_t* currentDatum;

	if (!wcscmp(WIDE_STRINGIZING(PRIMARY_VIDEO_BUFFER), currentDatum))
	{
		/*
		//currentDatum = strtok(nullptr, Delimiter);
		currentDatum = wcstok(nullptr, Delimiter);

		//PositiveInteger[PRIMARY_VIDEO_BUFFER] = currentDatum;
		*/
	}

	return 0;
}

bool Parser::UnloadFile()
{
	free(LoadedDatum);
	free(ReadDatum);

	fclose(FileAddress);
	
	return 0;
}
