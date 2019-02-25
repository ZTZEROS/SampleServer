#pragma once

template<typename DatumType>
class Queue //CircularQueue
{
private:
	volatile unsigned int FrontIndex;
	//volatile unsigned int CurrentCount; //deprecated
	volatile unsigned int NextIndex; //(FrontIndex + CurrentSize % MaximumSize) == RearIndex + 1
	unsigned int MaximumCount;

	DatumType DatumA[QUEUE_SIZE]; //Queue
	//vector<DatumType> DatumV;

	bool Locked;
	SRWLOCK EnqueueLock;
	SRWLOCK DequeueLock;

public:
	Queue() {}
	~Queue() {}

	bool Initialize()
	{
		FrontIndex = 0;
		//CurrentCount = 0;
		NextIndex = 0;
		MaximumCount = sizeof(DatumA) / sizeof(DatumType);

		memset(DatumA, 0, sizeof(DatumA));

		Locked = 0;

		InitializeSRWLock(&EnqueueLock);
		InitializeSRWLock(&DequeueLock);

		return 0;
	}

	bool Terminalize()
	{
		FrontIndex = 0;
		//CurrentCount = 0;
		NextIndex = 0;

		return 0;
	}

	DatumType Enqueue(DatumType datum)
	{
		unsigned int frontIndex;
		unsigned int oldNextIndex;
		unsigned int newNextIndex;
		unsigned int maximumCount;

		frontIndex = FrontIndex;
		maximumCount = MaximumCount;

		//if (Locked) AcquireSRWLockExclusive(&EnqueueLock);

		oldNextIndex = NextIndex;
		newNextIndex = oldNextIndex;

		//newNextIndex = oldNextIndex + 1;
		//newNextIndex &= maximumCount - 1;
		//newNextIndex = (oldNextIndex + 1) & (maximumCount - 1);
		if (maximumCount <= oldNextIndex + 1) newNextIndex = oldNextIndex + 1 - maximumCount;
		else newNextIndex = oldNextIndex + 1;

		if (newNextIndex != frontIndex)
		{
			DatumA[oldNextIndex] = datum;

			NextIndex = newNextIndex;

			//++CurrentCount;

			//if (Locked) ReleaseSRWLockExclusive(&EnqueueLock);

			return datum;
		}
		else
		{
			DatumType nullDatum;

			memset(&nullDatum, 0, sizeof(nullDatum));

			//if (Locked) ReleaseSRWLockExclusive(&EnqueueLock);

			return nullDatum;
		}
	}

	/*
	unsigned int Enqueue(DatumType* datumAddress, unsigned int size)
	{
		int frontSpace;
		int rearSpace;
		int copySize;

		if (size <= sizeof(DatumType) * (MaximumCount - CurrentCount))
		{
			if ((FrontIndex + CurrentCount) % MaximumCount < FrontIndex)
			{
				memcpy(&DatumA[(FrontIndex + CurrentCount) % MaximumCount], &datumAddress[0], size);
			}
			else
			{
				frontSpace = sizeof(DatumType) * FrontIndex;
				rearSpace = sizeof(DatumType) * (MaximumCount - (FrontIndex + CurrentCount));

				if (size < rearSpace) copySize = size;
				else copySize = rearSpace;

				memcpy(&DatumA[(FrontIndex + CurrentCount) % MaximumCount], &datumAddress[0], copySize);
				memcpy(&DatumA[(FrontIndex + CurrentCount + copySize) % MaximumCount], &datumAddress[copySize], size - copySize);
				//memcpy(&DatumA[0], datumTypeAddress[copySize], size - copySize);
			}

			//slow
			//for (unsigned int i = 0; i < size; ++i)
			//{
			//	//DatumA[(FrontIndex + CurrentCount) % MaximumCount] = datumType[i];
			//	//++CurrentCount;
			//
			//	DatumA[(FrontIndex + CurrentCount + i) % MaximumCount] = datumType[i];
			//}
			
			CurrentCount += size; //danger in multithread 
			
			return size;
		}
		//else
		//{
		//	//copy to left space
		//}
		else return 0;
	}
	*/

	DatumType Dequeue()
	{
		DatumType frontDatum;

		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int maximumCount;

		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		//if (Locked) AcquireSRWLockExclusive(&DequeueLock);

		frontIndex = FrontIndex;

		if (frontIndex != nextIndex)
		{
			frontDatum = DatumA[frontIndex];

			//frontIndex = frontIndex + 1;
			//frontIndex &= maximumCount - 1;
			//frontIndex = frontIndex + 1 & maximumCount - 1;
			if (maximumCount <= frontIndex + 1) FrontIndex = frontIndex + 1 - maximumCount;
			else FrontIndex = frontIndex + 1;

			//--CurrentCount;

			//if (Locked) ReleaseSRWLockExclusive(&DequeueLock);

			return frontDatum;
		}
		else
		{
			DatumType nullDatum;

			memset(&nullDatum, 0, sizeof(nullDatum));

			//if (Locked) ReleaseSRWLockExclusive(&DequeueLock);

			return nullDatum;
		}
	}

	/*
	unsigned int Dequeue(DatumType* datumAddress, unsigned int size)
	{
		int frontSize;
		int rearSize;
		int copySize;

		if (size <= sizeof(DatumType) * CurrentCount)
		{
			if (FrontIndex < (FrontIndex + CurrentCount) % MaximumCount)
			{
				memcpy(&datumAddress[0], &DatumA[FrontIndex], size);
			}
			else
			{
				frontSize = sizeof(DatumType) * (MaximumCount - FrontIndex);
				rearSize = sizeof(DatumType) * ((FrontIndex + CurrentCount) - MaximumCount);

				if (size < frontSize) copySize = size;
				else copySize = frontSize;

				memcpy(&datumAddress[0], &DatumA[FrontIndex], copySize);
				memcpy(&datumAddress[copySize], &DatumA[(FrontIndex + copySize) % MaximumCount], size - copySize);
				//memcpy(&destinationAddress[copySize], &DatumA[0], size - copySize);
			}

			//FrontIndex += size;
			//FrontIndex %= MaximumCount;
			FrontIndex = (FrontIndex + size) % MaximumCount; //danger in multithread 
			CurrentCount -= size; //danger in multithread 

			return size;
		}
		//else
		//{
		//	//copy left datum
		//}
		else return 0;
	}
	*/

	DatumType Peek(unsigned int index = 0)
	{
		DatumType currentDatum;

		unsigned int frontIndex;
		unsigned int currentIndex;
		unsigned int currentCount;
		unsigned int nextIndex;
		unsigned int maximumCount;

		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		//if (Locked) AcquireSRWLockExclusive(&DequeueLock);

		frontIndex = FrontIndex;

		if (frontIndex <= nextIndex) currentCount = nextIndex - frontIndex;
		else currentCount = maximumCount - nextIndex + frontIndex;
				
		if (index < currentCount)
		{
			if (maximumCount <= frontIndex + index) currentIndex = frontIndex + index - maximumCount;
			else currentIndex = frontIndex + index;

			if (currentIndex != nextIndex) //tautology case
			{
				currentDatum = DatumA[currentIndex];

				//if (Locked) ReleaseSRWLockExclusive(&DequeueLock);

				return currentDatum;
			}
			//else return nullDatum; //impossible case
		}
		else
		{
			DatumType nullDatum;

			memset(&nullDatum, 0, sizeof(nullDatum));

			//if (Locked) ReleaseSRWLockExclusive(&DequeueLock);

			return nullDatum;
		}
	}

	/*
	unsigned int Peek(DatumType* datumAddress, unsigned int size)
	{
		int frontSize;
		int rearSize;
		int copySize;

		if (size <= sizeof(DatumType) * CurrentCount)
		{
			if (FrontIndex < (FrontIndex + CurrentCount) % MaximumCount)
			{
				memcpy(&datumAddress[0], &DatumA[FrontIndex], size);
			}
			else
			{
				frontSize = sizeof(DatumType) * (MaximumCount - FrontIndex);
				rearSize = sizeof(DatumType) * ((FrontIndex + CurrentCount) - MaximumCount);

				if (size < frontSize) copySize = size;
				else copySize = frontSize;

				memcpy(&datumAddress[0], &DatumA[FrontIndex], copySize);
				memcpy(&datumAddress[copySize], &DatumA[(FrontIndex + copySize) % MaximumCount], size - copySize);
				//memcpy(&destinationAddress[copySize], &DatumA[0], size - copySize);
			}

			return size;
		}
		//else
		//{
		//	//peek left datum
		//}
		else return 0;
	}
	*/

	unsigned int IncreaseCurrentCount(unsigned int increasingCount = 1)
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int currentCount;
		unsigned int maximumCount;

		frontIndex = frontIndex;
		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		if (frontIndex <= nextIndex) currentCount = nextIndex - frontIndex;
		else currentCount = maximumCount - nextIndex + frontIndex;

		if (increasingCount <= maximumCount - currentCount)
		{
			if (maximumCount <= nextIndex + increasingCount) NextIndex = nextIndex + increasingCount - maximumCount;
			else NextIndex = nextIndex + increasingCount;

			//CurrentCount += increasingCount;

			return increasingCount;
		}

		return 0;
	}

	unsigned int DecreaseCurrentCount(unsigned int decreasingCount = 1)
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int currentCount;
		unsigned int maximumCount;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		if (frontIndex <= nextIndex) currentCount = nextIndex - frontIndex;
		else currentCount = maximumCount - frontIndex + nextIndex;

		if (decreasingCount <= currentCount)
		{
			if (maximumCount <= frontIndex + decreasingCount) FrontIndex = frontIndex + decreasingCount - maximumCount;
			else FrontIndex = frontIndex + decreasingCount;

			//CurrentCount -= decreasingCount;

			return decreasingCount;
		}

		return 0;
	}



	unsigned int GetFrontIndex() { return FrontIndex; }
	unsigned int GetCurrentCount()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int maximumCount;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		//return CurrentCount;
		return frontIndex <= nextIndex ? nextIndex - frontIndex : maximumCount - frontIndex + nextIndex;
	}
	unsigned int GetCurrentSize()
	{
		unsigned int frontIndex;
		unsigned int currentCount;
		unsigned int nextIndex;
		unsigned int maximumCount;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		currentCount = frontIndex <= nextIndex ? nextIndex - frontIndex : maximumCount - frontIndex + nextIndex;

		//return CurrentCount * sizeof(DatumType);
		return currentCount * sizeof(DatumType);
	}
	unsigned int GetLeftCount()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int maximumCount;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		//return MaximumCount - CurrentCount;
		return nextIndex < frontIndex ? frontIndex - nextIndex : maximumCount - nextIndex + frontIndex;
	}
	unsigned int GetLeftSize()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int leftCount;
		unsigned int maximumCount;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		leftCount = nextIndex < frontIndex ? frontIndex - nextIndex : maximumCount - nextIndex + frontIndex;

		//return (MaximumCount - CurrentCount) * sizeof(DatumType);
		return leftCount * sizeof(DatumType);
	}
	unsigned int GetNextIndex() { return NextIndex; }
	unsigned int GetMaximumCount() { return MaximumCount; }
	unsigned int GetMaximumSize() { return MaximumCount * sizeof(DatumType); }

	unsigned int GetFrontSpaceCount()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;

		return frontIndex < nextIndex ? frontIndex : frontIndex - nextIndex;
	}
	//unsigned int GetFrontSpaceSize() { return 0; }
	unsigned int GetRearSpaceCount()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int maximumCount;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		return frontIndex < nextIndex ? maximumCount - nextIndex : frontIndex - nextIndex;
	}
	//unsigned int GetRearSpaceSize() { return 0; }

	unsigned int GetFrontDatumCount()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int maximumCount;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		return frontIndex <= nextIndex ? nextIndex - frontIndex : maximumCount - frontIndex;
	}
	//unsigned int GetFrontDatumSize() { return 0; }
	unsigned int GetRearDatumCount()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;

		return frontIndex <= nextIndex ? nextIndex - frontIndex : nextIndex;
	}
	//unsigned int GetRearDatumSize() { return 0; }

	DatumType* GetFrontAddress() { return &DatumA[FrontIndex]; }
	DatumType* GetRearAddress()
	{
		unsigned int nextIndex;
		unsigned int maximumCount;

		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		return &DatumA[1 < nextIndex ? nextIndex - 1 : maximumCount - 1];
	}
	DatumType* GetNextAddress() { return &DatumA[NextIndex]; }
	DatumType* GetQueueAddress() { return &DatumA[0]; }

	DatumType& FrontDatum() { return DatumA[FrontIndex]; }
	DatumType& RearDatum()
	{
		unsigned int nextIndex;
		unsigned int maximumCount;

		nextIndex = NextIndex;
		maximumCount = MaximumCount;

		return DatumA[1 < nextIndex ? nextIndex - 1 : maximumCount - 1];
	}
	DatumType& Datum(unsigned int index) { return DatumA[index]; } //danger

	DatumType GetFrontDatum() { return DatumA[FrontIndex]; }
	DatumType GetRearDatum()
	{
		unsigned int nextIndex;
		unsigned int maximumCount;

		nextIndex = nextIndex;
		maximumCount = MaximumCount;

		return DatumA[1 < nextIndex ? nextIndex - 1 : maximumCount - 1];
	}
	DatumType GetDatum(unsigned int index) { return DatumA[index]; } //danger
	
	bool Lock() { Locked = 1; return Locked; }
	bool Unlock() { Locked = 0; return Locked; }

	void AquireEnqueueLock() { AcquireSRWLockExclusive(&EnqueueLock); } //{ if (!Locked) AcquireSRWLockExclusive(&EnqueueLock); }
	void ReleaseEnqueueLock() { ReleaseSRWLockExclusive(&EnqueueLock); } //{ if (!Locked) ReleaseSRWLockExclusive(&EnqueueLock); }

	void AquireDequeueLock() { AcquireSRWLockExclusive(&DequeueLock); } //{ if (!Locked) AcquireSRWLockExclusive(&DequeueLock); }
	void ReleaseDequeueLock() { ReleaseSRWLockExclusive(&DequeueLock); } //{ if (!Locked) ReleaseSRWLockExclusive(&DequeueLock); }

	//DatumType& operator[](unsigned int index)
	//{
	//	return DatumA[index]; //return *(DatumA + index);
	//}

	DatumType& operator[](unsigned int index)
	{
		return DatumA[(FrontIndex + index) % MaximumCount];
	}
};
