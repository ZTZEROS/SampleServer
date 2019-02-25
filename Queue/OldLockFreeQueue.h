#pragma once

struct QueueWorkRecord : public CommonWorkRecord
{

};

template<typename DatumType>
struct QueueInstance
{
	QueueInstance<DatumType>* NextAddress; //NextInstanceAddress;
	DatumType Datum;
	QueueInstance<DatumType>* SelfAddress;
};

template<typename DatumType>
struct QueueFront
{
	//QueueInstance<DatumType>* Address;
	unsigned int Index;
	unsigned int Padding;
	unsigned int WorkCount;
	unsigned int ThreadKey;
};

template<typename DatumType>
struct QueueRear
{
	//QueueInstance<DatumType>* Address;
	unsigned int Index;
	unsigned int Padding;
	unsigned int WorkCount;
	unsigned int ThreadKey;
};

template<typename DatumType>
struct QueueNext
{
	//QueueInstance<DatumType>* Address;
	unsigned int Index;
	unsigned int Padding;
	unsigned int WorkCount;
	unsigned int ThreadKey;
};

template<typename DatumType>
class LockFreeQueue //LockFreeLinkedQueue?
{
private:
	volatile unsigned int FrontIndex;
	volatile unsigned int CurrentCount;
	volatile unsigned int NextIndex;
	__declspec(align(16)) QueueFront<DatumType> Front;
	__declspec(align(16)) QueueRear<DatumType> Rear;
	__declspec(align(16)) QueueNext<DatumType> Next;
	unsigned int MaximumCount;


	DatumType* QueueAddress;
	//QueueInstance<DatumType>* QueueAddress;
	//QueueInstance<DatumType>** QueueAddress;

	bool Locked;
	//SRWLOCK EnqueueLock;
	//SRWLOCK DequeueLock;

public:
	LockFreeQueue() : FrontIndex(0), CurrentCount(0), NextIndex(0), MaximumCount(0), QueueAddress(nullptr), Locked(0) {}
	explicit LockFreeQueue(unsigned int maximumCount) : FrontIndex(0), CurrentCount(0), NextIndex(0), MaximumCount(maximumCount), QueueAddress(nullptr), Locked(0)
	{
		QueueAddress = (DatumType*)malloc(sizeof(DatumType) * MaximumCount);
		//QueueAddress = (QueueInstance<DatumType>*)malloc(sizeof(QueueInstance<DatumType>) * MaximumCount);
		//QueueAddress = (QueueInstance<DatumType>**)malloc(sizeof(QueueInstance<DatumType>*) * MaximumCount);
		
		//memset(QueueAddress, 0, sizeof(DatumType) * MaximumCount);
		memset(QueueAddress, 0, sizeof(QueueInstance<DatumType>) * MaximumCount);
		//memset(QueueAddress, 0, sizeof(QueueInstance<DatumType>*));

		FrontIndex = 0;
		CurrentCount = 0;
		NextIndex = 0;

		memset(&Front, 0, sizeof(Front));
		memset(&Rear, 0, sizeof(Rear));
		memset(&Next, 0, sizeof(Next));
		
		//MaximumCount = maximumCount;

		//QueueAddress = nullptr;

		Locked = 0;
	}
	~LockFreeQueue()
	{
		free(QueueAddress);
	}

	bool Initialize()
	{
		FrontIndex = 0;
		CurrentCount = 0;
		NextIndex = 0;

		Front.Address = nullptr;
		Front.Index = 0;
		Front.WorkCount = 0;
		Front.ThreadKey = 0;
		
		Rear.Address = nullptr;
		Rear.Index = 0;
		Rear.WorkCount = 0;
		Rear.ThreadKey = 0;
		
		Next.Address = nullptr;
		Next.Index = 0;
		Next.WorkCount = 0;
		Next.ThreadKey = 0;
		
		//MaximumCount = 0;

		//QueueAddress = nullptr;

		Locked = 0;

		return 0;
	}

	bool Terminalize()
	{
		return 0;
	}

	/*
	DatumType Enqueue(DatumType datum)
	{
		__declspec(align(16)) QueueFront<DatumType> front;
		QueueNext<DatumType> oldNext;
		QueueNext<DatumType> newNext;
		unsigned int nextIndex;
		unsigned int maximumCount;

		do
		{
			front = Front;
			oldNext = Next;
			maximumCount = MaximumCount;

			if (oldNext.Index + 1 != front.Index)
			{
				if (maximumCount <= oldNext.Index + 1) newNext.Index = oldNext.Index + 1 - maximumCount;
				else newNext.Index = oldNext.Index + 1;

				newNext.WorkCount = oldNext.WorkCount;
				++newNext.WorkCount;
			}
			else break;
		}
		while (!InterlockedCompareExchange128((LONG64*)&Next, (LONG64)newNext.WorkCount, (LONG64)newNext.Index, (LONG64*)&oldNext));
		
		if (oldNext.Index + 1 != front.Index)
		{
			QueueAddress[oldNext.Index].Instance = datum; //lazy enqueue problem
			QueueAddress[oldNext.Index].Dequeueable = 1;

			return QueueAddress[oldNext.Index].Instance;
		}
		else
		{
			DatumType nullDatum;

			memset(&nullDatum, 0, sizeof(nullDatum));

			return nullDatum;
		}
	}

	DatumType Dequeue()
	{
		__declspec(align(16)) QueueFront<DatumType> oldFront;
		QueueFront<DatumType> newFront;
		QueueNext<DatumType> next;
		unsigned int maximumCount;

		do
		{
			oldFront = Front;
			next = Next;
			maximumCount = MaximumCount;

			if (QueueAddress[oldFront.Index].Dequeueable) QueueAddress[oldFront.Index].Dequeueable = 0;
			else continue;

			if (oldFront.Index != next.Index)
			{
				if (maximumCount <= oldFront.Index + 1) newFront.Index = oldFront.Index + 1 - maximumCount;
				else newFront.Index = oldFront.Index + 1;

				newFront.WorkCount = oldFront.WorkCount;
				++newFront.WorkCount;
			}
			else break;
		}
		while (!InterlockedCompareExchange128((LONG64*)&Front, (LONG64)newFront.WorkCount, (LONG64)newFront.Index, (LONG64*)&oldFront));

		if (oldFront.Index != next.Index)
		{
			return QueueAddress[oldFront.Index].Instance;
		}
		else
		{
			DatumType nullDatum;

			memset(&nullDatum, 0, sizeof(nullDatum));

			return nullDatum;
		}
	}
	*/

	DatumType Enqueue(DatumType datum)
	{
		//QueueInstance<DatumType> rearInstance;
		DatumType rearDatum;
		QueueFront<DatumType> front;
		__declspec(align(16)) QueueNext<DatumType> oldNext;
		QueueNext<DatumType> newNext;
		unsigned int oldNextIndex;
		unsigned int currentCount;
		unsigned int maximumCount;

		//memset(&rearInstance, 0, sizeof(rearInstance));
		memset(&rearDatum, 0, sizeof(rearDatum));

		maximumCount = MaximumCount;
		//newNext.ThreadKey = GetCurrentThreadId();

		do
		{
			front = Front;
			oldNext = Next;

			if (maximumCount <= oldNext.Index + 1) newNext.Index = oldNext.Index + 1 - maximumCount;
			else newNext.Index = oldNext.Index + 1;

			newNext.WorkCount = oldNext.WorkCount;
			++newNext.WorkCount;

			if (newNext.Index != front.Index) oldNextIndex = oldNext.Index;
			else
			{
				currentCount = CurrentCount;

				if (currentCount < maximumCount)
				{
					oldNext.Index = 0xffffffff;
					oldNext.WorkCount = 0xffffffff;

					continue; //how about using goto?
				}
				else return rearDatum;
				//else return rearInstance.Datum;
			}
		}
		while (!InterlockedCompareExchange128((LONG64*)&Next, (LONG64)newNext.WorkCount, (LONG64)newNext.Index, (LONG64*)&oldNext));

		InterlockedIncrement(&CurrentCount);

		//while (1)
		//{
		//	if (QueueAddress[oldNextIndex].Datum == NULL && QueueAddress[oldNextIndex].SelfAddress ==nullptr)
		//	{
		//		QueueAddress[oldNextIndex].Datum = datum;
		//
		//		QueueAddress[oldNextIndex].SelfAddress = &QueueAddress[oldNextIndex];
		//		
		//		break;
		//	}
		//}

		while (InterlockedCompareExchange64((LONG64*)&QueueAddress[oldNextIndex], (LONG64)datum, (LONG64)nullptr));

		return datum;
	}

	DatumType Dequeue()
	{
		//QueueInstance<DatumType> frontInstance;
		DatumType frontDatum;
		__declspec(align(16)) QueueFront<DatumType> oldFront;
		QueueFront<DatumType> newFront;
		QueueNext<DatumType> next;
		unsigned int oldFrontIndex;
		unsigned int currentCount;
		unsigned int maximumCount;

		//memset(&frontInstance, 0, sizeof(frontInstance));
		memset(&frontDatum, 0, sizeof(frontDatum));

		maximumCount = MaximumCount;
		//newFront.ThreadKey = GetCurrentThreadId();

		do
		{
			oldFront = Front;
			next = Next;

			if (maximumCount <= oldFront.Index + 1) newFront.Index = oldFront.Index + 1 - maximumCount;
			else newFront.Index = oldFront.Index + 1;

			newFront.WorkCount = oldFront.WorkCount;
			++newFront.WorkCount;

			if (oldFront.Index != next.Index) //다른 스레드의 작업으로 Front와 Next가 만나 버리면 이 조건으로는 불충분, while조건을 체크하면 곤란
			{
				oldFrontIndex = oldFront.Index;

				frontDatum = QueueAddress[oldFrontIndex];

				if (!frontDatum)
				{
					oldFront.Index = 0xffffffff; //while 조건 체크가 성공하는 사태를 막기 위한, 있을 수 없는 oldFront값 세팅
					oldFront.WorkCount = 0xffffffff;

					continue; //how about using goto?
				}

				//do
				//{
				//	frontDatum = QueueAddress[oldFront.Index];
				//}
				//while (!frontDatum);
				//
				//QueueAddress[oldFront.Index] = nullptr;
			}
			else
			{
				currentCount = CurrentCount;

				if (0 < currentCount) //if(QueueAddress[newFront.Index].SelfAddress != nullptr)
				{
					oldFront.Index = 0xffffffff;
					oldFront.WorkCount = 0xffffffff;

					continue; //how about using goto?
				}
				else return frontDatum;
			}
		}
		while (!InterlockedCompareExchange128((LONG64*)&Front, (LONG64)newFront.WorkCount, (LONG64)newFront.Index, (LONG64*)&oldFront));
		
		InterlockedDecrement(&CurrentCount);
		
		//do
		//{
		//	frontDatum = QueueAddress[oldFrontIndex];
		//
		//	if (!frontDatum)
		//	{
		//		int a;
		//
		//		a = 1;
		//	}
		//
		//}
		//while (!frontDatum);
		
		if (!frontDatum)
		{
			int a;

			a = 1;

			//작업중 컨텍스트 스위치
			//다른 스레드의 작업으로 front와 next가 만난 상황,
			//continue를 타고 while 조건 체크, 실패
			//루틴 반복, front와 next가 만난 채로 while 조건 체크, 성공
			//frontDatum이 nullptr인 채로 여기에 도달
			
			//enqueue에서의 실패가 선행되었을 가능성이 있음
		}

		//frontDatum = QueueAddress[oldFrontIndex];

		if (*(frontDatum) != GetCurrentThreadId())
		{
			int c;

			c = 3;
		}
		
		//QueueAddress[oldFrontIndex].Datum = NULL;

		QueueAddress[oldFrontIndex] = nullptr;

		//return frontInstance.Datum;
		return frontDatum;
	}

	unsigned int GetCurrentCount() { return CurrentCount; }
	unsigned int GetMaximumCount() { return MaximumCount; }
	DatumType* GetFront() { return &QueueAddress[Front.Index]; }
	//DatumType* GetRear() { return &QueueAddress[Rear.Index]; }
	DatumType* GetNext() { return &QueueAddress[Next.Index]; }
};

template<typename T>
class QueueT
{
private:
	long _size;

	struct Node
	{
		T data;
		Node* next;
	};

	Node* _head;
	Node* _tail;

public:
	QueueT()
	{
		_size = 0;
		_head = new Node;
		_head->next = nullptr;
		_tail = _head;
	}

	void Enqueue(T t)
	{
		Node* node = new Node;
		node->data = t;
		node->next = nullptr;

		do
		{
			Node* tail = _tail;
			Node* next = tail->next;

			if (next == nullptr)
			{
				if (InterlockedCompareExchangePointer((PVOID*)&tail->next, node, next) == next)
				{
					InterlockedCompareExchangePointer((PVOID*)&_tail, node, tail);
					break;
				}
			}
		}
		while (1);

		InterlockedExchangeAdd(&_size, 1);
	}


	int Dequeue(T& t)
	{
		if (_size == 0) return -1;

		while (1)
		{
			Node* head = _head;
			Node* next = head->next;

			if (next == nullptr) return -1;
			else
			{
				if (InterlockedCompareExchangePointer((PVOID*)&_head, next, head) == head)
				{
					t = next->data;
					delete head;
					break;
				}
			}
		}

		InterlockedExchangeAdd(&_size, -1);

		return 0;
	}
};

/*
if (InterlockedCompareExchange64((LONG64*)&QueueAddress[oldNext.Index], (LONG64)datum, (LONG64)nullptr) == (LONG64)nullptr)
{
//Front와 Next가 옮겨지고 값도 널인 경우 버그 발생, 둘 다 변한 경우를 체크할 필요가 있다

//Next를 다른 곳에서 갱신해줄 필요?
if (!InterlockedCompareExchange128((LONG64*)&Next, (LONG64)newNext.WorkCount, (LONG64)newNext.Index, (LONG64*)&oldNext))
{
if (oldNext.Index != newNext.Index)
{
int e;

e = 50;
}
}

break;
}
else
{
int c;

c = 30;

continue;

//같은 datum을 동시에 두 스레드에서 넣고자 하면 한쪽 스레드가 실패 후 다시 같은 datum을 넣고자 시도하게 될 것
//같은 datum을 두번 넣고 나면 넣어야 할 다른 datum을 못 넣게 되어 아래 else 루틴을 실행하게 되어야 한다
//그렇다면 같은 datum을 빼게 될 것
//같은 datum을 동시에 두 스레드에서 넣는 경우는 발생하지 않는 테스트 환경이므로 지금 문제는 이 문제가 아님
//위 문제는 현재 테스트 환경에서는 dequeue문제가 생긴 다음에야 생김
}
*/

__m128 //SIMD single instruction multiple data