#include "FirstOrderLibrary.h"
#include "ByteQueue.h"

ByteQueue::ByteQueue() {}
ByteQueue::~ByteQueue() {}

bool ByteQueue::Initialize()
{
	FrontIndex = 0;
	//CurrentSize = 0;
	NextIndex = 0;
	MaximumSize = sizeof(ByteA);

	memset(ByteA, 0, sizeof(ByteA));

	Locked = 0;

	InitializeSRWLock(&EnqueueLock);
	InitializeSRWLock(&DequeueLock);

	return 0;
}

bool ByteQueue::Terminalize()
{
	FrontIndex = 0;
	//CurrentSize = 0;
	NextIndex = 0;

	return 0;
}

unsigned int ByteQueue::Enqueue(char* datumAddress, unsigned int size)
{
	unsigned int frontIndex;
	unsigned int nextIndex;
	unsigned int maximumSize;
	int rearSpace;
	int copySize;

	frontIndex = FrontIndex;
	maximumSize = MaximumSize;

	if (Locked) AcquireSRWLockExclusive(&EnqueueLock);

	nextIndex = NextIndex;

	if (nextIndex < frontIndex)
	{
		if (size < frontIndex - nextIndex) //currentSpace == maximumSize - currentSize
		{
			memcpy(&ByteA[nextIndex], &datumAddress[0], size);

			if (maximumSize <= nextIndex + size) NextIndex = nextIndex + size - maximumSize;
			else NextIndex = nextIndex + size;

			//CurrentSize += size; //danger in multithread 

			if (Locked) ReleaseSRWLockExclusive(&EnqueueLock);

			return size;
		}
	}
	else
	{
		if (size < maximumSize - nextIndex + frontIndex) //currentSpace == maximumSize - currentSize
		{
			rearSpace = maximumSize - nextIndex;

			if (size < rearSpace) copySize = size;
			else copySize = rearSpace;

			memcpy(&ByteA[nextIndex], &datumAddress[0], copySize);
			memcpy(&ByteA[0], &datumAddress[copySize], size - copySize);

			if (maximumSize <= nextIndex + size) NextIndex = nextIndex + size - maximumSize;
			else NextIndex = nextIndex + size;

			//CurrentSize += size; //danger in multithread

			if (Locked) ReleaseSRWLockExclusive(&EnqueueLock);

			return size;
		}
	}
	//else
	//{
	//	//copy to left space
	//}

	if (Locked) ReleaseSRWLockExclusive(&EnqueueLock);

	return 0;
}

unsigned int ByteQueue::Dequeue(char* datumAddress, unsigned int size)
{
	unsigned int frontIndex;
	unsigned int nextIndex;
	unsigned int maximumSize;
	int frontSize;
	int copySize;

	nextIndex = NextIndex;
	maximumSize = MaximumSize;
	
	//if (Locked) AcquireSRWLockExclusive(&DequeueLock); //not necessary

	frontIndex = FrontIndex;

	if (frontIndex <= nextIndex)
	{
		if (size <= nextIndex - frontIndex) //currentSize
		{
			memcpy(&datumAddress[0], &ByteA[frontIndex], size);

			if (maximumSize <= frontIndex + size) FrontIndex = frontIndex + size - maximumSize;
			else FrontIndex = frontIndex + size;

			//CurrentSize -= size; //danger in multithread

			//if (Locked) AcquireSRWLockExclusive(&DequeueLock); //not necessary

			return size;
		}
	}
	else
	{
		if (size <= maximumSize - frontIndex + nextIndex) //currentSize
		{
			frontSize = maximumSize - frontIndex;

			if (size < frontSize) copySize = size;
			else copySize = frontSize;

			memcpy(&datumAddress[0], &ByteA[frontIndex], copySize);
			memcpy(&datumAddress[copySize], &ByteA[0], size - copySize);

			if (maximumSize <= frontIndex + size) FrontIndex = frontIndex + size - maximumSize;
			else FrontIndex = frontIndex + size;

			//CurrentSize -= size; //danger in multithread

			//if (Locked) AcquireSRWLockExclusive(&DequeueLock); //not necessary

			return size;
		}
	}
	//else
	//{
	//	//copy to left datum
	//}

	//if (Locked) AcquireSRWLockExclusive(&DequeueLock); //not necessary

	return 0;
}

unsigned int ByteQueue::Peek(char* datumAddress, unsigned int size)
{
	unsigned int frontIndex;
	unsigned int nextIndex;
	unsigned int maximumSize;
	int frontSize;
	int copySize;

	nextIndex = NextIndex;
	maximumSize = MaximumSize;

	//if (Locked) AcquireSRWLockExclusive(&DequeueLock); //not synchronized;

	frontIndex = FrontIndex;

	if (frontIndex <= nextIndex)
	{
		if (size <= nextIndex - frontIndex) //currentSize
		{
			memcpy(&datumAddress[0], &ByteA[frontIndex], size);

			//if (Locked) AcquireSRWLockExclusive(&DequeueLock); //not synchronized;

			return size;
		}
	}
	else
	{
		if (size <= maximumSize - frontIndex + nextIndex) //currentSize
		{
			frontSize = maximumSize - frontIndex;

			if (size < frontSize) copySize = size;
			else copySize = frontSize;

			memcpy(&datumAddress[0], &ByteA[frontIndex], copySize);
			memcpy(&datumAddress[copySize], &ByteA[0], size - copySize);

			//if (Locked) AcquireSRWLockExclusive(&DequeueLock); //not synchronized;

			return size;
		}
	}
	//else
	//{
	//	//peek left datum
	//}

	//if (Locked) AcquireSRWLockExclusive(&DequeueLock); //not synchronized;

	return 0;
}

unsigned int ByteQueue::IncreaseCurrentSize(unsigned int increasingSize) //enqueuedSize
{  
	unsigned int frontIndex;
	unsigned int nextIndex;
	unsigned int currentSize;
	unsigned int maximumSize;

	frontIndex = FrontIndex;
	nextIndex = NextIndex;
	maximumSize = MaximumSize;

	if (frontIndex <= nextIndex) currentSize = nextIndex - frontIndex;
	else currentSize = maximumSize - nextIndex + frontIndex;

	if (increasingSize <= maximumSize - currentSize)
	{
		if (maximumSize <= nextIndex + increasingSize) NextIndex = nextIndex + increasingSize - maximumSize;
		else NextIndex = nextIndex + increasingSize;

		//CurrentSize += enqueuedSize; //danger in multithread

		return increasingSize;
	}
	
	return 0;
}

unsigned int ByteQueue::DecreaseCurrentSize(unsigned int decreasingSize) //dequeuedSize
{
	unsigned int frontIndex;
	unsigned int nextIndex;
	unsigned int currentSize;
	unsigned int maximumSize;

	frontIndex = FrontIndex;
	nextIndex = NextIndex;
	maximumSize = MaximumSize;

	if (frontIndex <= nextIndex) currentSize = nextIndex - frontIndex;
	else currentSize = maximumSize - frontIndex + nextIndex;

	if (decreasingSize <= currentSize)
	{
		if (maximumSize <= frontIndex + decreasingSize) FrontIndex = frontIndex + decreasingSize - maximumSize;
		else FrontIndex = frontIndex + decreasingSize;

		//CurrentSize -= dequeuedSize; //danger in multithread.

		return decreasingSize;
	}

	return 0;
}

int ByteQueue::Receive(int(*recv)(SOCKET, char*, int, int), SOCKET socket) //Enqueue
{
	unsigned int frontIndex;
	unsigned int nextIndex;
	unsigned int maximumSize;
	int receivedSize1;
	int receivedSize2;

	frontIndex = FrontIndex;
	maximumSize = MaximumSize;
	receivedSize1 = 0;
	receivedSize2 = 0;

	if (Locked) AcquireSRWLockExclusive(&EnqueueLock);

	nextIndex = NextIndex;
	
	if (nextIndex < frontIndex)
	{
		receivedSize1 = recv(socket, &ByteA[nextIndex], frontIndex - nextIndex, 0);

		if (receivedSize1 < 1);
		else
		{
			if (maximumSize <= nextIndex + receivedSize1) NextIndex = nextIndex + receivedSize1 - maximumSize;
			else NextIndex = nextIndex + receivedSize1;
			
			//CurrentSize += receivedSize1; //danger?
		}
	}
	else
	{
		receivedSize1 = recv(socket, &ByteA[nextIndex], maximumSize - nextIndex, 0);

		if (receivedSize1 < 1);
		else
		{
			if (maximumSize <= nextIndex + receivedSize1)
			{
				nextIndex = nextIndex + receivedSize1 - maximumSize;
				NextIndex = nextIndex;

				//CurrentSize += receivedSize1; //danger?

				receivedSize2 = recv(socket, &ByteA[nextIndex], frontIndex, 0); //nextIndex == 0

				if (receivedSize2 < 1);
				else
				{
					if (maximumSize <= nextIndex + receivedSize2) NextIndex = nextIndex + receivedSize2 - maximumSize;
					else NextIndex = nextIndex + receivedSize2;

					//CurrentSize += receivedSize2; //danger?
				}
			}
			else
			{
				NextIndex = nextIndex + receivedSize1;

				//CurrentSize += receivedSize1; //danger?
			}
		}
	}

	if (Locked) ReleaseSRWLockExclusive(&EnqueueLock);

	return receivedSize1 + receivedSize2;
}

int ByteQueue::Send(int(*send)(SOCKET, const char*, int, int), SOCKET socket) //Dequeue
{
	unsigned int frontIndex;
	unsigned int nextIndex;
	unsigned int maximumSize;
	int sendedSize1; //shame
	int sendedSize2; //shame
	
	frontIndex = FrontIndex;
	nextIndex = NextIndex;
	maximumSize = MaximumSize;
	sendedSize1 = 0;
	sendedSize2 = 0;

	if (frontIndex <= nextIndex)
	{
		sendedSize1 = send(socket, &ByteA[frontIndex], nextIndex - frontIndex, 0);

		if (sendedSize1 < 0);
		else
		{
			if (maximumSize <= frontIndex + sendedSize1) FrontIndex = frontIndex + sendedSize1 - maximumSize;
			else FrontIndex = frontIndex + sendedSize1;

			//CurrentSize -= sendedSize1; //danger?
		}
	}
	else
	{
		sendedSize1 = send(socket, &ByteA[frontIndex], maximumSize - frontIndex, 0);

		if (sendedSize1 < 0);
		else
		{
			if (maximumSize <= frontIndex + sendedSize1)
			{
				frontIndex = frontIndex + sendedSize1 - maximumSize;
				FrontIndex = frontIndex;

				//CurrentSize -= sendedSize1; //danger?

				sendedSize2 = send(socket, &ByteA[frontIndex], nextIndex, 0);

				if (sendedSize2 < 0);
				else
				{
					if (maximumSize <= frontIndex + sendedSize2) FrontIndex = frontIndex + sendedSize2 - maximumSize;
					else FrontIndex = frontIndex + sendedSize2;

					//CurrentSize -= sendedSize2; //danger?
				}
			}
			else
			{
				FrontIndex = frontIndex + sendedSize1;

				//CurrentSize -= sendedSize1; //danger?
			}
		}
	}

	return sendedSize1 + sendedSize2;
}

int ByteQueue::WSAReceive(int(*WSARecv)(SOCKET, LPWSABUF, DWORD, LPDWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE), SOCKET socket, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped) //Enqueue
{
	unsigned int frontIndex;
	unsigned int nextIndex;
	unsigned int maximumSize;
	WSABUF wsaBufferA[2];
	DWORD receivedSize;
	int returnValue;

	frontIndex = FrontIndex;
	maximumSize = MaximumSize;

	if (Locked) AcquireSRWLockExclusive(&EnqueueLock);

	nextIndex = NextIndex;

	if (nextIndex < frontIndex)
	{
		wsaBufferA[0].buf = &ByteA[nextIndex];
		wsaBufferA[0].len = frontIndex - nextIndex;

		returnValue = WSARecv(socket, wsaBufferA, 1, &receivedSize, lpFlags, lpOverlapped, nullptr);
	}
	else
	{
		wsaBufferA[0].buf = &ByteA[nextIndex];
		wsaBufferA[0].len = maximumSize - nextIndex;
		wsaBufferA[1].buf = &ByteA[0];
		wsaBufferA[1].len = frontIndex;

		returnValue = WSARecv(socket, wsaBufferA, 2, &receivedSize, lpFlags, lpOverlapped, nullptr);
	}

	if (returnValue < 1);
	else
	{
		if (maximumSize <= nextIndex + receivedSize) NextIndex = nextIndex + receivedSize - maximumSize;
		else NextIndex = nextIndex + receivedSize;

		//CurrentSize += receivedSize; //danger?
	}

	if (Locked) ReleaseSRWLockExclusive(&EnqueueLock);

	return returnValue;
}

int ByteQueue::WSASend(int(*WSASend)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE), SOCKET socket, DWORD flags, LPWSAOVERLAPPED lpOverlapped) //Send
{
	unsigned int frontIndex;
	unsigned int nextIndex;
	unsigned int maximumSize;
	WSABUF wsaBufferA[2];
	DWORD sendedSize; //shame
	int returnValue;

	frontIndex = FrontIndex;
	nextIndex = NextIndex;
	maximumSize = MaximumSize;

	if (frontIndex <= nextIndex)
	{
		wsaBufferA[0].buf = &ByteA[frontIndex];
		wsaBufferA[0].len = nextIndex - frontIndex;

		returnValue = WSASend(socket, wsaBufferA, 1, &sendedSize, flags, lpOverlapped, nullptr);
	}
	else
	{
		wsaBufferA[0].buf = &ByteA[frontIndex];
		wsaBufferA[0].len = maximumSize - frontIndex;
		wsaBufferA[1].buf = &ByteA[0];
		wsaBufferA[1].len = nextIndex;

		returnValue = WSASend(socket, wsaBufferA, 2, &sendedSize, flags, lpOverlapped, nullptr);
	}

	if (returnValue < 1);
	else
	{
		if (maximumSize <= frontIndex + sendedSize) FrontIndex = frontIndex + sendedSize - maximumSize;
		else FrontIndex = frontIndex + sendedSize;

		//CurrentSize -= sendedSize; //danger?
	}

	return returnValue;
}
