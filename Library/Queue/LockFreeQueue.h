#pragma once

struct QueueWorkHistory : public CommonWorkHistory
{

};

template<typename DatumType>
struct QueueInstance //QueueNode
{
	QueueInstance<DatumType>* NextAddress; //NextInstanceAddress;
	DatumType Datum;
	QueueInstance<DatumType>* SelfAddress; //SelfInstanceAddress;
};

template<typename DatumType>
struct QueueFront
{
	QueueInstance<DatumType>* Address;
	unsigned int WorkCount;
	unsigned int ThreadKey;
};

template<typename DatumType>
struct QueueRear
{
	QueueInstance<DatumType>* Address;
	unsigned int WorkCount;
	unsigned int ThreadKey;
};

template<typename DatumType>
struct QueueNext
{
	QueueInstance<DatumType>* Address;
	unsigned int WorkCount;
	unsigned int ThreadKey;
};

template<typename DatumType>
class LockFreeQueue
{
private:
	LockFreePool<QueueInstance<DatumType>> Pool;

	unsigned int CurrentCount;
	unsigned int MaximumCount;

	__declspec(align(16)) QueueFront<DatumType> Front; //Head
	__declspec(align(16)) QueueRear<DatumType> Rear; //Tail
	__declspec(align(16)) QueueNext<DatumType> Next;

public:
	LockFreeQueue() : CurrentCount(0), MaximumCount(0)
	{
		Front.Address = Pool.Allocate();
		Front.Address->NextAddress = nullptr;
		Front.WorkCount = 0;
		Front.ThreadKey = 0;

		Rear.Address = Front.Address;
		Rear.WorkCount = 0;
		Rear.ThreadKey = 0;
	}
	~LockFreeQueue()
	{
		QueueInstance<DatumType>* currentAddress;

		while (Front.Address) //(Front.Address != Rear.Address)
		{
			currentAddress = Front.Address;

			Front.Address = currentAddress->NextAddress;

			Pool.Deallocate(currentAddress);
		}
	}

	bool Initialize()
	{
		Pool.Initialize();

		CurrentCount = 0;
		MaximumCount = 0;

		Front.Address = Pool.Allocate();
		Front.Address->NextAddress = nullptr;
		Front.WorkCount = 0;
		Front.ThreadKey = 0;

		Rear.Address = Front.Address;
		Rear.WorkCount = 0;
		Rear.ThreadKey = 0;

		return 0;
	}

	bool Terminalize()
	{
		QueueInstance<DatumType>* currentAddress;

		while (Front.Address)
		{
			currentAddress = Front.Address;

			Front.Address = currentAddress->NextAddress;

			Pool.Deallocate(currentAddress);
		}

		Pool.Terminalize();

		return 0;
	}

	DatumType Enqueue(DatumType datum)
	{
		QueueInstance<DatumType>* currentAddress;
		__declspec(align(16)) QueueRear<DatumType> rear; //localTail;
		QueueNext<DatumType> next; //localNext;

		currentAddress = Pool.Allocate();
		currentAddress->NextAddress = nullptr;
		currentAddress->Datum = datum;
		currentAddress->SelfAddress = currentAddress;

		do
		{
			rear.WorkCount = Rear.WorkCount;
			rear.Address = Rear.Address;

			next.WorkCount = rear.WorkCount;
			next.Address = rear.Address;

			++next.WorkCount;
			next.Address = next.Address->NextAddress;

			if (!next.Address)
			{
				if (InterlockedCompareExchangePointer((PVOID*)&(Rear.Address->NextAddress), (PVOID)currentAddress, (PVOID)next.Address) == next.Address)
				{
					InterlockedCompareExchange128((LONG64*)&Rear, (LONG64)next.WorkCount, (LONG64)currentAddress, (LONG64*)&rear);
					
					break;
				}
			}
			else
			{
				InterlockedCompareExchange128((LONG64*)&Rear, (LONG64)next.WorkCount, (LONG64)next.Address, (LONG64*)&rear);
			}
		}
		while (1);

		InterlockedIncrement(&CurrentCount);
		//CurrentCount = Pool.GetCurrentCount;
		MaximumCount = Pool.GetMaximumCount();

		return datum;
	}

	DatumType Dequeue()
	{
		QueueInstance<DatumType>* currentAddress;
		DatumType currentDatum; //frontDatum
		__declspec(align(16)) QueueFront<DatumType> oldFront; //localHead;
		QueueFront<DatumType> newFront;
		__declspec(align(16)) QueueRear<DatumType> rear; //localTail;
		QueueNext<DatumType> next; //localNext;

		currentAddress = nullptr;
		memset(&currentDatum, 0, sizeof(currentDatum)); //currentDatum = NULL;

		if (CurrentCount)
		{
			InterlockedDecrement(&CurrentCount);
			//CurrentCount = Pool.GetCurrentCount();
			MaximumCount = Pool.GetMaximumCount();
		}
		else return currentDatum;
		
		do
		{
			oldFront.WorkCount = Front.WorkCount;
			oldFront.Address = Front.Address;

			newFront.WorkCount = oldFront.WorkCount;
			newFront.Address = oldFront.Address;

			++newFront.WorkCount;
			newFront.Address = newFront.Address->NextAddress;

			

			rear.WorkCount = Rear.WorkCount;
			rear.Address = Rear.Address;
			
			next.WorkCount = rear.WorkCount;
			next.Address = rear.Address;
			
			++next.WorkCount;
			next.Address = next.Address->NextAddress;
			
			if (oldFront.Address == rear.Address)
			{
				if (next.Address)
				{
					if (InterlockedCompareExchange128((LONG64*)&Rear, (LONG64)next.WorkCount, (LONG64)next.Address, (LONG64*)&rear));
					else continue;
				}
				else continue;
				//{
				//	memset(&currentDatum, 0, sizeof(currentDatum));
				//
				//	break;
				//}
			}

			if (newFront.Address)
			{
				//if (oldFront.Address == rear.Address)
				//{
				//	if (next.Address)
				//	{
				//		if (InterlockedCompareExchange128((LONG64*)&Rear, (LONG64)next.WorkCount, (LONG64)next.Address, (LONG64*)&rear));
				//		else continue;
				//	}
				//	else continue;
				//}

				currentDatum = newFront.Address->Datum;

				if (InterlockedCompareExchange128((LONG64*)&Front, (LONG64)newFront.WorkCount, (LONG64)newFront.Address, (LONG64*)&oldFront))
				{
					Pool.Deallocate(oldFront.Address);
					
					break;
				}
			}
		}
		while (1);

		return currentDatum;
	}

	unsigned int GetCurrentCount() { return CurrentCount; }
	unsigned int GetMaximumCount() { return MaximumCount; }

	QueueFront<DatumType> GetQueueFront() { return Front; }
	QueueRear<DatumType> GetQueueRear() { return Rear; }

	QueueInstance<DatumType> GetFrontInstance() { return *(Front.Address); }
	QueueInstance<DatumType> GetRearInstance() { return *(Rear.Address); }
	
	QueueInstance<DatumType>* GetFrontAddress() { return Front.Address; }
	QueueInstance<DatumType>* GetRearAddress() { return Rear.Address; }
	
	DatumType GetFrontDatum() { return Front.Address->Datum; }
	DatumType GetRearDatum() { return Rear.Address->Datum; }
};
