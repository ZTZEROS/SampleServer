#pragma once

struct AllocatedMemory
{
	void* Address; //AllocatedMemoryAddress
	unsigned int Size;
	const char* AllocatingFileName;
	unsigned int AllocatingFileLine;
	AllocatedMemory* NextAddress; //AllocatedMemoryInformationAddress
};

extern AllocatedMemory ALLOCATED_MEMORY;

template <typename DatumType>
DatumType* AllocateMemory(unsigned int count, const char* allocatingFileName, int allocatingFileline)
{
	AllocatedMemory* currentAddress;

	currentAddress = &ALLOCATED_MEMORY;

	while (currentAddress->NextAddress)
	{
		currentAddress = currentAddress->NextAddress;
	}

	currentAddress->NextAddress = (AllocatedMemory*)malloc(sizeof(AllocatedMemory));

	if (currentAddress->NextAddress) currentAddress = currentAddress->NextAddress;
	else return nullptr; //bug point

	currentAddress->Address = (DatumType*)malloc(sizeof(DatumType) * count);

	if (currentAddress->Address);
	else return nullptr; //bug point

	currentAddress->Size = sizeof(DatumType) * count; //currentAddress->Size = sizeof(*(currentAddress->Address));
	currentAddress->AllocatingFileName = allocatingFileName;
	currentAddress->AllocatingFileLine = allocatingFileline;
	currentAddress->NextAddress = nullptr;

	return (DatumType*)currentAddress->Address;
}

extern int DeallocateMemory(void* address, const char* allocatingFileName, int allocatingFileline); //not finished
extern void ShowAllocatedMemory();