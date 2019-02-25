#pragma once

class SerialByteQueue
{
private:
	unsigned int FrontIndex;
	unsigned int CurrentSize;
	unsigned int MaximumSize;

	unsigned int ReferenceCount;

	char ByteA[SERIAL_BYTE_QUEUE_SIZE]; //Queue
	//vector<char> ByteV;

public:
	SerialByteQueue();
	~SerialByteQueue();

	bool Initialize();
	bool Terminalize();

	template<typename DatumType>
	unsigned int Enqueue(DatumType datum);

	template<typename DatumType>
	unsigned int Dequeue(DatumType& datum);

	template<typename DatumType>
	unsigned int operator<<(DatumType datum) //Enqueue
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
	unsigned int operator>>(DatumType& datum) //Dequeue
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



	unsigned int IncreaseCurrentSize(unsigned int increasingSize) { CurrentSize += increasingSize; return CurrentSize; }
	unsigned int IncreaseReferenceCount() { return InterlockedIncrement(&ReferenceCount); }
	
	unsigned int DecreaseCurrentSize(unsigned int decreasingSize) { CurrentSize -= decreasingSize; return CurrentSize; }
	unsigned int DecreaseReferenceCount() { return InterlockedDecrement(&ReferenceCount); }



	unsigned int GetFrontIndex() { return FrontIndex; }
	unsigned int GetCurrentSize() { return CurrentSize; }
	//unsigned int GetNextIndex() { return FrontIndex + CurrentSize; } //RearIndex + 1 //deprecated
	unsigned int GetMaximumSize() { return MaximumSize; }

	char* GetQueueAddress() { return &ByteA[0]; }
	char* GetFrontAddress() { return &ByteA[FrontIndex]; }
	//char* GetNextAddress() { return &ByteA[FrontIndex + CurrentSize]; } //deprecated
};
