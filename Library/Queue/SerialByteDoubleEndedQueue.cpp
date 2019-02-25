#include "FirstOrderLibrary.h"
#include "SerialByteDoubleEndedQueue.h"

SerialByteDoubleEndedQueue::SerialByteDoubleEndedQueue() {}
SerialByteDoubleEndedQueue::~SerialByteDoubleEndedQueue() {}

bool SerialByteDoubleEndedQueue::Initialize()
{
	//LeftEndIndex = 0;
	//RightEndIndex = 0;

	//LeftEndIndex = SERIAL_BYTE_DOUBLE_ENDED_QUEUE_SIZE - 1;
	//RightEndIndex = SERIAL_BYTE_DOUBLE_ENDED_QUEUE_SIZE - 1;

	LeftEndIndex = SERIAL_BYTE_DOUBLE_ENDED_QUEUE_SIZE / 2;
	RightEndIndex = SERIAL_BYTE_DOUBLE_ENDED_QUEUE_SIZE / 2 - 1;

	CurrentSize = 0;
	MaximumSize = sizeof(ByteA);

	ReferenceCount = 0;

	memset(ByteA, 0, sizeof(ByteA));

	return 0;
}

bool SerialByteDoubleEndedQueue::Terminalize()
{
	//LeftEndIndex = 0;
	//RightEndIndex = 0;
	
	//LeftEndIndex = MaximumSize - 1;
	//RightEndIndex = MaximumSize - 1;

	LeftEndIndex = MaximumSize / 2;
	RightEndIndex = MaximumSize / 2 - 1;

	CurrentSize = 0;

	return 0;
}

template<typename DatumType>
unsigned int SerialByteDoubleEndedQueue::LeftEnqueue(DatumType datum)
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize <= LeftEndIndex)
	//if (datumSize <= MaximumSize - CurrentSize)
	{
		*(DatumType*)&(ByteA[LeftEndIndex - datumSize]) = datum;

		LeftEndIndex -= datumSize;
		CurrentSize += datumSize;

		return datumSize;
	}

	return 0;
}

template<typename DatumType>
unsigned int SerialByteDoubleEndedQueue::RightEnqueue(DatumType datum)
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize < MaximumSize - RightEndIndex)
	//if (datumSize < MaximumSize - CurrentSize)
	{
		*(DatumType*)&(ByteA[RightEndIndex + 1]) = datum;

		RightEndIndex += datumSize;
		CurrentSize += datumSize;

		return datumSize;
	}

	return 0;
}

template<typename DatumType>
unsigned int SerialByteDoubleEndedQueue::LeftDequeue(DatumType& datum)
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize <= CurrentSize)
	//if (datumSize <= RightEndIndex - LeftEndIndex + 1)
	{
		datum = *(DatumType*)&(ByteA[LeftEndIndex]);

		CurrentSize -= datumSize;
		LeftEndIndex += datumSize;

		return datumSize;
	}

	return 0;
}

template<typename DatumType>
unsigned int SerialByteDoubleEndedQueue::RightDequeue(DatumType& datum)
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize <= CurrentSize)
	//if (datumSize <= RightEndIndex - LeftEndIndex + 1)
	{
		datum = *(DatumType*)&(ByteA[RightEndIndex - datumSize + 1]);

		CurrentSize -= datumSize;
		RightEndIndex -= datumSize;

		return datumSize;
	}

	return 0;
}
