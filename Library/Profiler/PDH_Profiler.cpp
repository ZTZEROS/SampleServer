#include "FirstOrderLibrary.h"
#include "PDH_Profiler.h"

PDH_Profiler PDH_PROFILER;

PDH_Profiler::PDH_Profiler()
{
	{
		ZeroMemory(&AvailableByteCounterValue, sizeof(AvailableByteCounterValue));
		ZeroMemory(&PoolNonpagedByteCounterValue, sizeof(PoolNonpagedByteCounterValue));

		ZeroMemory(&CommittedByteCounterValue, sizeof(CommittedByteCounterValue));

		ZeroMemory(&ReceivedByteCounterValue, sizeof(ReceivedByteCounterValue));
		ZeroMemory(&SendedByteCounterValue, sizeof(SendedByteCounterValue));

		//ZeroMemory(&PrivateByteCounterValue, sizeof());



		CurrentAvailableByte = 0;
		CurrentPoolNonpagedByte = 0;

		CurrentCommittedByte = 0;

		CurrentReceivedByte = 0;
		CurrentSendedByte = 0;

		//CurrentPrivateByte = 0;
	}

	{
		PdhOpenQuery(nullptr, NULL, &QueryHandle);

		PdhAddCounter(QueryHandle, L"\\Memory\\Available Bytes", NULL, &AvailableByteCounterHandle);
		PdhAddCounter(QueryHandle, L"\\Memory\\Available MBytes", NULL, &AvailableMByteCounterHandle);
		PdhAddCounter(QueryHandle, L"\\Memory\\Pool Nonpaged Bytes", NULL, &PoolNonpagedByteCounterHandle);

		PdhAddCounter(QueryHandle, L"\\Memory\\Committed Bytes", NULL, &CommittedByteCounterHandle);

		PdhAddCounter(QueryHandle, L"\\Network Interface(*)\\Bytes Received/sec", NULL, &ReceivedByteCounterHandle);
		PdhAddCounter(QueryHandle, L"\\Network Interface(*)\\Bytes Sent/sec", NULL, &SendedByteCounterHandle);

		PdhAddCounter(QueryHandle, L"\\Network Interface(Realtek PCIe GBE Family Controller)\\Bytes Received/sec", NULL, &ReceivedByteCounterHandle1);
		PdhAddCounter(QueryHandle, L"\\Network Interface(Realtek PCIe GBE Family Controller)\\Bytes Sent/sec", NULL, &SendedByteCounterHandle1);

		PdhAddCounter(QueryHandle, L"\\Network Interface(Realtek PCIe GBE Family Controller _2)\\Bytes Received/sec", NULL, &ReceivedByteCounterHandle2);
		PdhAddCounter(QueryHandle, L"\\Network Interface(Realtek PCIe GBE Family Controller _2)\\Bytes Sent/sec", NULL, &SendedByteCounterHandle2);

		//PdhAddCounter(QueryHandle, L"\\Process(/*SERVER_NAME*/)\\Private Bytes", NULL, &PrivateByteCounterHandle);



		PdhCollectQueryData(QueryHandle);
	}
}

PDH_Profiler::~PDH_Profiler()
{
	PdhCloseQuery(QueryHandle);
}

bool PDH_Profiler::Initialize()
{
	{
		ZeroMemory(&AvailableByteCounterValue, sizeof(AvailableByteCounterValue));
		ZeroMemory(&PoolNonpagedByteCounterValue, sizeof(PoolNonpagedByteCounterValue));

		ZeroMemory(&CommittedByteCounterValue, sizeof(CommittedByteCounterValue));

		ZeroMemory(&ReceivedByteCounterValue, sizeof(ReceivedByteCounterValue));
		ZeroMemory(&SendedByteCounterValue, sizeof(SendedByteCounterValue));

		//ZeroMemory(&PrivateByteCounterValue, sizeof());



		CurrentAvailableByte = 0;
		CurrentPoolNonpagedByte = 0;

		CurrentCommittedByte = 0;

		CurrentReceivedByte = 0;
		CurrentSendedByte = 0;

		//CurrentPrivateByte = 0;
	}

	{
		PdhOpenQuery(nullptr, NULL, &QueryHandle);

		PdhAddCounter(QueryHandle, L"\\Memory\\Available Bytes", NULL, &AvailableByteCounterHandle);
		PdhAddCounter(QueryHandle, L"\\Memory\\Available MBytes", NULL, &AvailableMByteCounterHandle);
		PdhAddCounter(QueryHandle, L"\\Memory\\Pool Nonpaged Bytes", NULL, &PoolNonpagedByteCounterHandle);

		PdhAddCounter(QueryHandle, L"\\Memory\\Committed Bytes", NULL, &CommittedByteCounterHandle);

		PdhAddCounter(QueryHandle, L"\\Network Interface(*)\\Bytes Received/sec", NULL, &ReceivedByteCounterHandle);
		PdhAddCounter(QueryHandle, L"\\Network Interface(*)\\Bytes Sent/sec", NULL, &SendedByteCounterHandle);

		PdhAddCounter(QueryHandle, L"\\Network Interface(Realtek PCIe GBE Family Controller)\\Bytes Received/sec", NULL, &ReceivedByteCounterHandle1);
		PdhAddCounter(QueryHandle, L"\\Network Interface(Realtek PCIe GBE Family Controller)\\Bytes Sent/sec", NULL, &SendedByteCounterHandle1);

		PdhAddCounter(QueryHandle, L"\\Network Interface(Realtek PCIe GBE Family Controller _2)\\Bytes Received/sec", NULL, &ReceivedByteCounterHandle2);
		PdhAddCounter(QueryHandle, L"\\Network Interface(Realtek PCIe GBE Family Controller _2)\\Bytes Sent/sec", NULL, &SendedByteCounterHandle2);
		//PdhAddCounter(QueryHandle, L"\\Process(/*SERVER_NAME*/)\\Private Bytes", NULL, &PrivateByteCounterHandle);



		PdhCollectQueryData(QueryHandle);
	}

	return 0;
}

bool PDH_Profiler::Update()
{
	PdhCollectQueryData(QueryHandle);

	//{
	//	PdhGetFormattedCounterValue(AvailableByteCounterHandle, PDH_FMT_LONG, NULL, &AvailableByteCounterValue);
	//
	//	PdhGetFormattedCounterValue(PoolNonpagedByteCounterHandle, PDH_FMT_LONG, NULL, &PoolNonpagedByteCounterValue);
	//
	//	PdhGetFormattedCounterValue(CommittedByteCounterHandle, PDH_FMT_LONG, NULL, &CommittedByteCounterValue);
	//
	//	PdhGetFormattedCounterValue(ReceivedByteCounterHandle, PDH_FMT_LONG, NULL, &ReceivedByteCounterValue);
	//	PdhGetFormattedCounterValue(SendedByteCounterHandle, PDH_FMT_LONG, NULL, &SendedByteCounterValue);
	//
	//	//PdhGetFormattedCounterValue(PrivateByteCounterHandle, PDH_FMT_LONG, NULL, &PrivateByteCounterValue);
	//}

	{
		PDH_FMT_COUNTERVALUE counterValue;

		PdhGetFormattedCounterValue(AvailableByteCounterHandle, PDH_FMT_LONG, NULL, &counterValue);
		CurrentAvailableByte = counterValue.longValue;

		PdhGetFormattedCounterValue(AvailableMByteCounterHandle, PDH_FMT_LONG, NULL, &counterValue);
		CurrentAvailableMByte = counterValue.longValue;

		PdhGetFormattedCounterValue(PoolNonpagedByteCounterHandle, PDH_FMT_LONG, NULL, &counterValue);
		CurrentPoolNonpagedByte = counterValue.longValue;



		PdhGetFormattedCounterValue(CommittedByteCounterHandle, PDH_FMT_LONG, NULL, &counterValue);
		CurrentCommittedByte = counterValue.longValue;



		//PdhGetFormattedCounterValue(ReceivedByteCounterHandle, PDH_FMT_LONG, NULL, &counterValue);
		//CurrentReceivedByte = counterValue.longValue;

		PdhGetFormattedCounterValue(ReceivedByteCounterHandle1, PDH_FMT_LONG, NULL, &counterValue);
		CurrentReceivedByte = counterValue.longValue;

		PdhGetFormattedCounterValue(ReceivedByteCounterHandle2, PDH_FMT_LONG, NULL, &counterValue);
		CurrentReceivedByte += counterValue.longValue;

		//PdhGetFormattedCounterValue(SendedByteCounterHandle, PDH_FMT_LONG, NULL, &counterValue);
		//CurrentSendedByte = counterValue.longValue;

		PdhGetFormattedCounterValue(SendedByteCounterHandle1, PDH_FMT_LONG, NULL, &counterValue);
		CurrentSendedByte = counterValue.longValue;

		PdhGetFormattedCounterValue(SendedByteCounterHandle2, PDH_FMT_LONG, NULL, &counterValue);
		CurrentSendedByte += counterValue.longValue;

		//PdhGetFormattedCounterValue(PrivateByteCounterHandle, PDH_FMT_LONG, NULL, &counterValue);
		//CurrentPrivateByte = counterValue.longValue;
	}

	return 0;
}

bool PDH_Profiler::Render()
{
	//{
	//	printf(
	//		"AvailableByteCounterValue : %l" CRALF
	//		"PoolNonpagedByteCounterValue : %l" CRALF
	//
	//		"CommittedByteCounterValue : %l" CRALF
	//
	//		"ReceivedByteCounterValue : %l" CRALF
	//		"SendedByteCounterValue : %l" CRALF
	//
	//		"PrivateByteCounterValue : %l" CRALF
	//		,
	//		AvailableByteCounterValue.longValue,
	//		PoolNonpagedByteCounterValue.longValue,
	//		
	//		CommittedByteCounterValue.longValue,
	//		
	//		ReceivedByteCounterValue.longValue,
	//		SendedByteCounterValue.longValue,
	//
	//		PrivateByteCounterValue.longValue
	//		);
	//}

	{
		printf(
			"CurrentAvailableByte : %u" CRALF
			"CurrentPoolNonpagedByte : %u" CRALF
			CRALF
			"CurrentCommittedByte : %u" CRALF
			CRALF
			"CurrentReceivedByte : %u" CRALF
			"CurrentSendedByte : %u" CRALF
			CRALF
			"CurrentPrivateByte : %u" CRALF
			,
			CurrentAvailableByte,
			CurrentPoolNonpagedByte,

			CurrentCommittedByte,

			CurrentReceivedByte,
			CurrentSendedByte,

			CurrentPrivateByte
			);
	}

	return 0;
}

bool PDH_Profiler::Terminalize()
{
	PdhCloseQuery(QueryHandle);

	return 0;
}