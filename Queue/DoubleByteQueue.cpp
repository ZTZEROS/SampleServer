#include "FirstOrderLibrary.h"
#include "DoubleByteQueue.h"

DoubleByteQueue::DoubleByteQueue() {}
DoubleByteQueue::~DoubleByteQueue() {}

void DoubleByteQueue::Initialize(unsigned int maximumSize)
{
	for (CurrentIndex = 0; CurrentIndex < 2; ++CurrentIndex)
	{
		Queue[CurrentIndex].FrontIndex = 0;
		Queue[CurrentIndex].CurrentSize = 0;
		Queue[CurrentIndex].MaximumSize = maximumSize;
		Queue[CurrentIndex].QueueAddress = (char*)malloc(Queue[CurrentIndex].MaximumSize);
	}
}

unsigned int DoubleByteQueue::Enqueue(char* datumAddress, unsigned int size)
{
	int frontSpace;
	int rearSpace;
	int copySize;

	return 0;
}

unsigned int DoubleByteQueue::Dequeue(char* datumAddress, unsigned int size)
{
	int frontSize;
	int rearSize;
	int copySize;

	return 0;
}

unsigned int DoubleByteQueue::Peek(char* datumAddress, unsigned int size)
{


	return 0;
}

unsigned int DoubleByteQueue::Erase(unsigned int size)
{
	unsigned int erasingSize;

	//erasingSize = size < CurrentSize ? size : 0;
	//CurrentSize -= erasingSize;

	return erasingSize;
}


unsigned int DoubleByteQueue::Resize(unsigned int maximumSize)
{
	//free(QueueAddress);
	//
	//FrontIndex = 0;
	//CurrentSize = 0;
	//MaximumSize = maximumSize;

	//QueueAddress = (char*)malloc(MaximumSize);
}

unsigned int DoubleByteQueue::Clear()
{
	unsigned int currentSize;

	//currentSize = CurrentSize;
	//
	//FrontIndex = 0;
	//CurrentSize = 0;

	return currentSize;
}

void DoubleByteQueue::Terminalize()
{

}
