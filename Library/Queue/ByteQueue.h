#pragma once

class ByteQueue //CircularByteQueue
{
private:
	volatile unsigned int FrontIndex;
	//volatile unsigned int CurrentSize; //deprecated
	volatile unsigned int NextIndex; //(FrontIndex + CurrentSize % MaximumSize) == RearIndex + 1
	//volatile unsigned int LeftSize;
	//volatile unsigned int EnqueuedSize;
	//voaltile unsigned int DequeuedSize;
	unsigned int MaximumSize;

	char ByteA[BYTE_QUEUE_SIZE]; //Queue
	//vector<char> ByteV;

	bool Locked;
	SRWLOCK EnqueueLock;
	SRWLOCK DequeueLock;

	//bool Enqueuing;
	//bool Dequeuing;

public:
	ByteQueue();
	~ByteQueue();

	bool Initialize();
	bool Terminalize();

	unsigned int Enqueue(char* datumAddress, unsigned int size);
	unsigned int Dequeue(char* datumAddress, unsigned int size);

	unsigned int Peek(char* datumAddress, unsigned int size);

	unsigned int IncreaseCurrentSize(unsigned int increasingSize);
	unsigned int DecreaseCurrentSize(unsigned int decreasingSize);

	int Receive(int(*recv)(SOCKET, char*, int, int), SOCKET socket);
	int Send(int(*send)(SOCKET, const char*, int, int), SOCKET socket);

	int WSAReceive(int(*WSARecv)(SOCKET, LPWSABUF, DWORD, LPDWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE), SOCKET socket, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped);
	int WSASend(int(*WSASend)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE), SOCKET socket, DWORD flags, LPWSAOVERLAPPED lpOverlapped);



	unsigned int GetFrontIndex() { return FrontIndex; }
	unsigned int GetCurrentSize()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int maximumSize;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumSize = MaximumSize;

		//return CurrentSize;
		return frontIndex <= nextIndex ? nextIndex - frontIndex : maximumSize - frontIndex + nextIndex;
	}
	unsigned int GetLeftSize()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int maximumSize;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumSize = MaximumSize;

		//return MaximuSize - CurrentSize;
		return nextIndex < frontIndex ? frontIndex - nextIndex : maximumSize - nextIndex + frontIndex;
	}

	unsigned int GetNextIndex() { return NextIndex; }
	unsigned int GetMaximumSize() { return MaximumSize; }

	unsigned int GetFrontSpaceSize()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;

		return frontIndex < nextIndex ? frontIndex : frontIndex - nextIndex;
	}
	unsigned int GetRearSpaceSize()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int maximumSize;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumSize = MaximumSize;

		return frontIndex < nextIndex ? maximumSize - nextIndex : frontIndex - nextIndex;
	}

	unsigned int GetFrontDatumSize()
	{
		unsigned int frontIndex;
		unsigned int nextIndex;
		unsigned int maximumSize;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;
		maximumSize = MaximumSize;

		return frontIndex <= nextIndex ? nextIndex - frontIndex : maximumSize - frontIndex;
	}
	unsigned int GetRearDatumSize()
	{ 
		unsigned int frontIndex;
		unsigned int nextIndex;

		frontIndex = FrontIndex;
		nextIndex = NextIndex;

		return frontIndex <= nextIndex ? nextIndex - frontIndex : nextIndex;
	}

	char* GetFrontAddress() { return &ByteA[FrontIndex]; }
	char* GetRearAddress()
	{
		unsigned int nextIndex;
		unsigned int maximumSize;

		nextIndex = NextIndex;
		maximumSize = MaximumSize;

		return &ByteA[1 < nextIndex ? nextIndex - 1 : maximumSize - 1];
	}
	char* GetNextAddress() { return &ByteA[NextIndex]; } //{ return &ByteA[(FrontIndex + CurrentSize) % MaximumSize]; }
	char* GetQueueAddress() { return &ByteA[0]; }
	
	char& Front() { return ByteA[FrontIndex]; }
	char& Rear()
	{ 
		unsigned int nextIndex;
		unsigned int maximumSize;

		nextIndex = NextIndex;
		maximumSize = MaximumSize;

		return ByteA[1 < nextIndex ? nextIndex - 1 : maximumSize - 1];
	}
	char& Datum(unsigned int index) { return ByteA[index]; } //danger

	bool Lock() { Locked = 1; return Locked; }
	bool Unlock() { Locked = 0; return Locked; }

	void AquireEnqueueLock() { AcquireSRWLockExclusive(&EnqueueLock); } //{ if (!Locked) AcquireSRWLockExclusive(&EnqueueLock); }
	void AquireDequeueLock() { AcquireSRWLockExclusive(&DequeueLock); } //{ if (!Locked) AcquireSRWLockExclusive(&DequeueLock); }

	void ReleaseEnqueueLock() { ReleaseSRWLockExclusive(&EnqueueLock); } //{ if (!Locked) ReleaseSRWLockExclusive(&EnqueueLock); }
	void ReleaseDequeueLock() { ReleaseSRWLockExclusive(&DequeueLock); } //{ if (!Locked) ReleaseSRWLockExclusive(&DequeueLock); }
};
