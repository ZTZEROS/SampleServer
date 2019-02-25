#pragma once

struct SingleByteQueue //change to SerialByteQueue in the future
{
	unsigned int FrontIndex;
	unsigned int CurrentSize;
	unsigned int MaximumSize;
	char* QueueAddress;
};

class DoubleByteQueue
{
private:
	SingleByteQueue Queue[2];
	unsigned int CurrentIndex;

	//unsigned int FrontIndex[2];
	//unsigned int CurrentSize[2];
	//unsigned int MaximumSize[2];
	//char* QueueAddress[2];

public:
	DoubleByteQueue();
	~DoubleByteQueue();

	void Initialize(unsigned int maximumSize);

	unsigned int Enqueue(char* datumAddress, unsigned int size);
	unsigned int Dequeue(char* datumAddress, unsigned int size);

	unsigned int Peek(char* datumAddress, unsigned int size);

	unsigned int Erase(unsigned int size);

	unsigned int Resize(unsigned int maximumSize);

	unsigned int Clear();

	void Terminalize();

	unsigned int GetCurrentSize() { return Queue[CurrentIndex].CurrentSize; }
	unsigned int GetLeftSize() { return Queue[CurrentIndex].MaximumSize - Queue[CurrentIndex].CurrentSize; }
	unsigned int GetMaximumSize() { return Queue[CurrentIndex].MaximumSize; }
	unsigned int GetFrontIndex() { return Queue[CurrentIndex].FrontIndex; }

	char* GetFrontAddress() { return &(Queue[CurrentIndex].QueueAddress[Queue[CurrentIndex].FrontIndex]); }
	char* GetRearAddress() { return &(Queue[CurrentIndex].QueueAddress[(Queue[CurrentIndex].FrontIndex + Queue[CurrentIndex].CurrentSize - 1) % Queue[CurrentIndex].MaximumSize]); }
	char* GetQueueAddress() { return Queue[CurrentIndex].QueueAddress; }
};

