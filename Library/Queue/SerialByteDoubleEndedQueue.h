#pragma once

class SerialByteDoubleEndedQueue //[...][Header][C][B][A][...]
{
private:
	unsigned int LeftEndIndex;
	//unsigned int LeftNextIndex;
	unsigned int RightEndIndex; //RearIndex == NextIndex - 1 == FrontIndex + CurrentSize - 1
	//unsigned int RightNextIndex; //FrontIndex + CurrentSize == RearIndex + 1
	unsigned int CurrentSize;
	unsigned int MaximumSize;

	unsigned int ReferenceCount;

	char ByteA[SERIAL_BYTE_DOUBLE_ENDED_QUEUE_SIZE]; //Queue
	//vector<char> ByteV;

public:
	SerialByteDoubleEndedQueue();
	~SerialByteDoubleEndedQueue();

	bool Initialize();
	bool Terminalize();

	//template<typename DatumType>
	//unsigned int Enqueue(DatumType datum); //deprecated

	template<typename DatumType>
	unsigned int LeftEnqueue(DatumType datum);

	template<typename DatumType>
	unsigned int RightEnqueue(DatumType datum);

	//template<typename DatumType>
	//unsigned int Dequeue(DatumType& datum); //deprecated

	template<typename DatumType>
	unsigned int LeftDequeue(DatumType& datum);

	template<typename DatumType>
	unsigned int RightDequeue(DatumType& datum);

	//template<typename DatumType>
	//unsigned int operator<<(DatumType datum); //RightEnqueue //deprecated

	//template<typename DatumType>
	//unsigned int operator>>(DatumType& datum); //RightDequeue //deprecated

	template<typename DatumType>
	friend unsigned int operator>>(DatumType datum, SerialByteDoubleEndedQueue& sbdeq); //LeftEnqueue

	template<typename DatumType>
	friend unsigned int operator<<(SerialByteDoubleEndedQueue& sbdeq, DatumType datum); //RightEnqueue

	template<typename DatumType>
	friend unsigned int operator<<(DatumType& datum, SerialByteDoubleEndedQueue& sbdeq); //LeftDequeue

	template<typename DatumType>
	friend unsigned int operator>>(SerialByteDoubleEndedQueue& sbdeq, DatumType& datum); //RightDequeue

	

	unsigned int IncreaseLeftCurrentSize(unsigned int increasingSize) { LeftEndIndex -= increasingSize; CurrentSize += increasingSize; return CurrentSize; }
	unsigned int IncreaseRightCurrentSize(unsigned int increasingSize) { RightEndIndex += increasingSize; CurrentSize += increasingSize; return CurrentSize; }
	unsigned int IncreaseReferenceCount() { return InterlockedIncrement(&ReferenceCount); }

	unsigned int DecreaseLeftCurrentSize(unsigned int decreasingSize) { CurrentSize -= decreasingSize; LeftEndIndex += decreasingSize; return CurrentSize; }
	unsigned int DecreaseRightCurrentSize(unsigned int decreasingSize) { CurrentSize -= decreasingSize; RightEndIndex -= decreasingSize; return CurrentSize; }
	unsigned int DecreaseReferenceCount() { return InterlockedDecrement(&ReferenceCount); }



	unsigned int GetLeftEndIndex() { return LeftEndIndex; }
	unsigned int GetRightEndIndex() { return RightEndIndex; }
	unsigned int GetCurrentSize() { return CurrentSize; }
	//unsigned int GetLeftNextIndex() { return LeftEndIndex - 1; } //deprecated
	//unsigned int GetRightNextIndex() { return RightEndIndex + 1; } //deprecated
	unsigned int GetMaximumSize() { return MaximumSize; }

	char* GetQueueAddress() { return &ByteA[0]; }
	char* GetLeftEndAddress() { return &ByteA[LeftEndIndex]; }
	char* GetRightEndAddress() { return &ByteA[RightEndIndex]; }
	//char* GetLeftNextAddress() { return &ByteA[LeftEndIndex - 1]; } //deprecated
	//char* GetRightNextAddress() { return &ByteA[RightEndIndex + 1]; } //deprecated
};



template<typename DatumType>
unsigned int operator>>(DatumType datum, SerialByteDoubleEndedQueue& sbdeq) //LeftEnqueue
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize <= sbdeq.LeftEndIndex)
	//if (datumSize <= sbdeq.MaximumSize - sbdeq.CurrentSize)
	{
		*(DatumType*)&(sbdeq.ByteA[sbdeq.LeftEndIndex - datumSize]) = datum;

		sbdeq.LeftEndIndex -= datumSize;
		sbdeq.CurrentSize += datumSize;

		return datumSize;
	}
	
	return 0;
}

template<typename DatumType>
unsigned int operator<<(SerialByteDoubleEndedQueue& sbdeq, DatumType datum) //RightEnqueue
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize < sbdeq.MaximumSize - sbdeq.RightEndIndex)
	//if (datumSize < sbdeq.MaximumSize - sbdeq.CurrentSize)
	{
		*(DatumType*)&(sbdeq.ByteA[sbdeq.RightEndIndex + 1]) = datum;

		sbdeq.RightEndIndex += datumSize;
		sbdeq.CurrentSize += datumSize;

		return datumSize;
	}

	return 0;
}

template<typename DatumType>
unsigned int operator<<(DatumType& datum, SerialByteDoubleEndedQueue& sbdeq) //LeftDequeue
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize <= sbdeq.CurrentSize)
	//if (datumSize <= sbdeq.RightEndIndex - sbdeq.LeftEndIndex + 1)
	{
		datum = *(DatumType*)&(sbdeq.ByteA[sbdeq.LeftEndIndex]);

		sbdeq.CurrentSize -= datumSize;
		sbdeq.LeftEndIndex += datumSize;

		return datumSize;
	}

	return 0;
}

template<typename DatumType>
unsigned int operator>>(SerialByteDoubleEndedQueue& sbdeq, DatumType& datum) //RightDequeue
{
	unsigned int datumSize;

	datumSize = sizeof(datum);

	if (datumSize <= sbdeq.CurrentSize)
	//if (datumSize <= sbdeq.RightEndIndex - sbdeq.LeftEndIndex + 1)
	{
		datum = *(DatumType*)&(sbdeq.ByteA[sbdeq.RightEndIndex - datumSize + 1]);

		sbdeq.CurrentSize -= datumSize;
		sbdeq.RightEndIndex -= datumSize;

		return datumSize;
	}

	return 0;
}
