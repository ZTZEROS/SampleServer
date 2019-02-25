#pragma once

template<typename DatumType>
class Stack
{
private:
	unsigned int TopIndex;
	unsigned int CurrentCount;
	unsigned int MaximumCount;

	DatumType DatumA[STACK_SIZE]; //Stack

public:
	Stack() {}
	~Stack() {}

	bool Initialize()
	{
		TopIndex = 0xffffffff;
		CurrentCount = 0;
		MaximumCount = sizeof(DatumA) / sizeof(DatumType);
		
		memset(DatumA, 0, sizeof(DatumA));

		return 0;
	}

	bool Terminalize()
	{
		TopIndex = 0xffffffff;
		CurrentCount = 0;

		return 0;
	}

	DatumType Push(DatumType datum)
	{
		unsigned int currentIndex;

		currentIndex = ++TopIndex;

		if (currentIndex < MaximumCount)
		{
			DatumA[currentIndex] = datum;

			++CurrentCount;

			return datum;
		}
		else
		{
			DatumType nullDatum;

			memset(&nullDatum, 0, sizeof(nullDatum));

			return nullDatum;
		}
	}

	DatumType Pop()
	{
		DatumType topDatum;
		unsigned int currentIndex;

		currentIndex = --TopIndex;

		if (currentIndex < MaximumCount)
		{
			topDatum = DatumA[currentIndex];

			--CurrentCount;

			return topDatum;
		}
		else
		{
			DatumType nullDatum;

			memset(&nullDatum, 0, sizeof(nullDatum));

			return nullDatum;
		}
	}

	unsigned int GetTopIndex() { return TopIndex; }
	unsigned int GetCurrentCount() { return CurrentCount; }
	unsigned int GetCurrentSize() { return CurrentCount * sizeof(DatumType); }
	unsigned int GetMaximumCount() { return MaximumCount; }
	unsigned int GetMaximumSize() { return MaximumCount * sizeof(DatumType); }

	DatumType* GetTopAddress() { return &DatumA[TopIndex]; }
	DatumType* GetBottomAddress() { return &DatumA[0]; }
	DatumType* GetStackAddress() { return &DatumA[0]; }

	DatumType& TopDatum() { return DatumA[TopIndex]; }
	DatumType& BottomDatum() { return DatumA[0]; }
	DatumType& Datum(unsigned int index) { return DatumA[index]; }

	DatumType GetTopDatum() { return DatumA[TopIndex]; }
	DatumType GetBottomDatum() { return DatumA[0]; }
	DatumType GetDatum(unsigned int index) { return DatumA[index]; }

	DatumType& operator[](unsigned int index)
	{
		return DatumA[index];
	}
};
