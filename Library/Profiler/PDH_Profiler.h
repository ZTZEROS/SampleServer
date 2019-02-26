#pragma once

class PDH_Profiler
{
private:
	PDH_HQUERY QueryHandle;
	WCHAR CounterPath[PDH_MAX_COUNTER_PATH];

	PDH_HCOUNTER AvailableByteCounterHandle; //limit 4gb
	PDH_HCOUNTER AvailableMByteCounterHandle;
	PDH_HCOUNTER PoolNonpagedByteCounterHandle;
	
	PDH_HCOUNTER CommittedByteCounterHandle;

	PDH_HCOUNTER ReceivedByteCounterHandle;
	PDH_HCOUNTER ReceivedByteCounterHandle1;
	PDH_HCOUNTER ReceivedByteCounterHandle2;
	PDH_HCOUNTER SendedByteCounterHandle;
	PDH_HCOUNTER SendedByteCounterHandle1;
	PDH_HCOUNTER SendedByteCounterHandle2;

	PDH_HCOUNTER PrivateByteCounterHandle;



	PDH_FMT_COUNTERVALUE AvailableByteCounterValue; //limit 4gb
	PDH_FMT_COUNTERVALUE AvailableMByteCounterValue;
	PDH_FMT_COUNTERVALUE PoolNonpagedByteCounterValue;

	PDH_FMT_COUNTERVALUE CommittedByteCounterValue;
	
	PDH_FMT_COUNTERVALUE ReceivedByteCounterValue;
	PDH_FMT_COUNTERVALUE ReceivedByteCounterValue1;
	PDH_FMT_COUNTERVALUE ReceivedByteCounterValue2;
	PDH_FMT_COUNTERVALUE SendedByteCounterValue;
	PDH_FMT_COUNTERVALUE SendedByteCounterValue1;
	PDH_FMT_COUNTERVALUE SendedByteCounterValue2;

	PDH_FMT_COUNTERVALUE PrivateByteCounterValue;



	UINT CurrentAvailableByte; //limit 4gb
	UINT CurrentAvailableMByte;
	UINT CurrentPoolNonpagedByte;

	UINT CurrentCommittedByte;

	UINT CurrentReceivedByte;
	UINT CurrentSendedByte;
	
	UINT CurrentPrivateByte;

public:
	PDH_Profiler();
	virtual ~PDH_Profiler();

	bool Initialize();
	bool Update();
	bool Render();
	bool Terminalize();



	unsigned int GetCurrentAvailableByte() { return CurrentAvailableByte; } //limit 4gb
	unsigned int GetCurrentAvailableMByte() { return CurrentAvailableMByte; }
	unsigned int GetCurrentPoolNonpagedByte() { return CurrentPoolNonpagedByte; }

	unsigned int GetCurrentCommittedByte() { return CurrentCommittedByte; }

	unsigned int GetCurrentReceivedByte() { return CurrentReceivedByte; }
	unsigned int GetCurrentSendedByte() { return CurrentSendedByte; }

	unsigned int GetCurrentPrivateByte() { return CurrentPrivateByte; }
};

extern PDH_Profiler PDH_PROFILER;