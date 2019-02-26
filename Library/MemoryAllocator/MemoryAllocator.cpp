#include "FirstOrderLibrary.h"
#include "MemoryAllocator.h"

AllocatedMemory ALLOCATED_MEMORY;

int DeallocateMemory(void* address, const char* allocatingFileName, int allocatingFileline) //not finished
{
	typeid(address) == typeid(address); //typeof

	AllocatedMemory* previousAddress;
	AllocatedMemory* currentAddress;

	previousAddress = nullptr;
	currentAddress = &ALLOCATED_MEMORY;

	if (address)
	{
		while (currentAddress->NextAddress)
		{
			previousAddress = currentAddress;
			currentAddress = currentAddress->NextAddress;

			if (currentAddress->Address == address)
			{
				free(address);

				previousAddress->NextAddress = currentAddress->NextAddress;

				free(currentAddress);

				return 0;
			}
		}

		return 1;
	}
	else return 2;
}

void ShowAllocatedMemory()
{
	AllocatedMemory* currentAddress;

	currentAddress = &ALLOCATED_MEMORY;

	if (!currentAddress->NextAddress) printf("No allocated Memory." CRLF);

	while (currentAddress->NextAddress)
	{
		currentAddress = currentAddress->NextAddress;

		printf("[%p] : ", currentAddress->Address);

		for (unsigned int i = 0; i < currentAddress->Size; ++i)
		{
			printf("%02x", *((unsigned char*)currentAddress->Address + i));
		}

		printf(CRLF);
		printf("File : %s, Line :%d" CRLF, currentAddress->AllocatingFileName, currentAddress->AllocatingFileLine);
	}

	return;
}

/*
int main(int argumentCount, char* argumentVector[])
{
memset(&ALLOCATED_MEMORY, 0, sizeof(ALLOCATED_MEMORY));

int* sample;

sample = ALLOCATE_MEMORY(int, 10);

sample[0] = 100;
sample[1] = 100;

ShowAllocatedMemory();

DELOCATE_MEMORY(sample);

ShowAllocatedMemory();

getchar();

return 1;
}
*/