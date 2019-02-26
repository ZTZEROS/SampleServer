#pragma once

class Parser
{
public:
	Parser();
	~Parser();

private:
	const wchar_t* Delimiter; //seperator
	FILE* FileAddress;
	wchar_t* LoadedDatum;
	wchar_t ReadDatum[256];
	
	unsigned int Depth;

public:
	bool Initialize();
	bool Terminalize();

	bool LoadFile();
	bool ReadFile();
	bool AssignValue();
	bool UnloadFile();
};

extern Parser PARSER;