#pragma once

struct StackWorkHistory : public CommonWorkHistory
{

};

template<typename DatumType>
struct StackInstance //StackNode
{
	StackInstance<DatumType>* NextAddress; //NextInstanceAddress;
	DatumType Datum;
	StackInstance<DatumType>* SelfAddress; //SelfInstanceAddress;
};

template<typename DatumType>
struct StackTop
{
	StackInstance<DatumType>* Address;
	unsigned int WorkCount;
	unsigned int ThreadKey; //ThreadId = GetCurrentThreadId();
};

template<typename DatumType>
class LockFreeStack
{
private:
	LockFreePool<StackInstance<DatumType>> Pool;

	unsigned int CurrentCount;
	unsigned int MaximumCount;
	
	__declspec(align(16)) StackTop<DatumType> Top;

public:
	LockFreeStack() : CurrentCount(0), MaximumCount(0)
	{
		Top.Address = nullptr;
		Top.WorkCount = 0;
		Top.ThreadKey = 0;
	}
	~LockFreeStack()
	{
		StackInstance<DatumType>* currentAddress;

		while (Top.Address)
		{
			currentAddress = Top.Address;

			Top.Address = currentAddress->NextAddress;

			Pool.Deallocate(currentAddress);
		}
	}

	bool Initialize()
	{
		Pool.Initialize();

		CurrentCount = 0;
		MaximumCount = 0;

		Top.Address = nullptr;
		Top.WorkCount = 0;
		Top.ThreadKey = 0;

		return 0;
	}

	bool Terminalize()
	{
		StackInstance<DatumType>* currentAddress;

		while (Top.Address)
		{
			currentAddress = Top.Address;

			Top.Address = currentAddress->NextAddress;

			Pool.Deallocate(currentAddress);
		}

		Pool.Terminalize();

		return 0;
	}

	DatumType Push(DatumType datum)
	{
		StackInstance<DatumType>* currentAddress;
		__declspec(align(16)) StackTop<DatumType> oldTop;
		StackTop<DatumType> newTop;

		currentAddress = Pool.Allocate();
		currentAddress->Datum = datum;
		currentAddress->SelfAddress = currentAddress;

		InterlockedIncrement(&CurrentCount);
		//CurrentCount = Pool.GetCurrentCount();
		MaximumCount = Pool.GetMaximumCount();

		do
		{
			oldTop.WorkCount = Top.WorkCount;
			oldTop.Address = Top.Address;

			currentAddress->NextAddress = oldTop.Address;
			
			newTop.WorkCount = oldTop.WorkCount;
			
			++newTop.WorkCount;
			newTop.Address = currentAddress;
		}
		while (InterlockedCompareExchange64((LONG64*)&Top.Address, (LONG64)newTop.Address, (LONG64)oldTop.Address) != (LONG64)oldTop.Address);
		//while (!InterlockedCompareExchange128((LONG64*)&Top, (LONG64)newTop.WorkCount, (LONG64)newTop.Address, (LONG64*)&oldTop));

		return datum;
	}

	DatumType Pop()
	{
		StackInstance<DatumType>* currentAddress;
		DatumType currentDatum;
		__declspec(align(16)) StackTop<DatumType> oldTop; //localTop;
		StackTop<DatumType> newTop;

		currentAddress = nullptr;
		currentDatum = NULL;

		do
		{
			oldTop.WorkCount = Top.WorkCount;
			oldTop.Address = Top.Address;

			if (oldTop.Address)
			{
				currentAddress = oldTop.Address;

				newTop.WorkCount = oldTop.WorkCount;
				newTop.Address = oldTop.Address;

				++newTop.WorkCount;
				newTop.Address = oldTop.Address->NextAddress;
			}
			else break;
		}
		while (!InterlockedCompareExchange128((LONG64*)&Top, (LONG64)newTop.WorkCount, (LONG64)newTop.Address, (LONG64*)&oldTop));

		if (oldTop.Address) //if(currentAddress)
		{
			currentDatum = currentAddress->Datum;
			
			currentAddress->NextAddress = nullptr;
			currentAddress->SelfAddress = nullptr;

			InterlockedDecrement(&CurrentCount);
			//CurrentCount = Pool.GetCurrentCount();
			MaximumCount = Pool.GetMaximumCount();

			Pool.Deallocate(currentAddress);
		}
		else
		{
			//currentAddress = Pool.Allocate();
			//currentAddress->NextAddress = nullptr;
			//currentAddress->SelfAddress = currentAddress;
			//
			//currentAddress->Datum = NULL;
			//
			//InterlockedIncrement(&MaximumCount);
			//MaximumCount = Pool.GetMaximumCount();
		}

		return currentDatum;
	}

	unsigned int GetCurrentCount() { return CurrentCount; }
	unsigned int GetMaximumCount() { return MaximumCount; }

	StackTop<DatumType> GetStackTop() { return Top; }
	
	StackInstance<DatumType> GetTopInstance() { return *(Top.Address); }
	
	StackInstance<DatumType>* GetTopAddress() { return Top.Address; }
	
	DatumType GetTopDatum() { return Top.Address->Datum; }
};
