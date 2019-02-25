#include "FirstOrderLibrary.h"
#include "SerialByteQueue.h"

SerialByteQueue::SerialByteQueue() {}
SerialByteQueue::~SerialByteQueue() {}

bool SerialByteQueue::Initialize()
{
	FrontIndex = 0;
	CurrentSize = 0;
	MaximumSize = sizeof(ByteA);

	memset(ByteA, 0, sizeof(ByteA));

	return 0;
}

bool SerialByteQueue::Terminalize()
{
	FrontIndex = 0;
	CurrentSize = 0;

	return 0;
}

template<typename DatumType>
unsigned int SerialByteQueue::Enqueue(DatumType datum)
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize <= MaximumSize - CurrentSize)
	{
		*(DatumType*)&(ByteA[FrontIndex + CurrentSize]) = datum;

		CurrentSize += datumSize;

		return datumSize;
	}
	
	return 0;
}

template<typename DatumType>
unsigned int SerialByteQueue::Dequeue(DatumType& datum)
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize <= CurrentSize)
	{
		datum = *(DatumType*)&(ByteA[FrontIndex]);

		FrontIndex += datumSize;
		CurrentSize -= datumSize;

		return datumSize;
	}
	
	return 0;
}
