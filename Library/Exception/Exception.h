#pragma once

class Exception
{
private:
	WCHAR Log[256];
	unsigned int WrittenIndex;

public:
	Exception();
	~Exception();

	template<typename DatumType>
	void WriteLog(DatumType datumType)
	{
		if(WrittenIndex < sizeof(Log))

		typeid(DatumType);

		Log[WrittenIndex];

		WrittenIndex += wcslen(typeid(DatumType).name);
	}
};