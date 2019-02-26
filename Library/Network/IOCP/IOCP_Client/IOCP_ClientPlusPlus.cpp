#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "IOCP_Client.h"

unsigned int IOCP_Client::ReceivePacket(UINT64 sessionKey, SerialPacketQueue<IOCP_PacketHeader>* receivedPacketSPQ_Address)
{
	IOCP_PacketHeader receivedPacketHeader;
	UINT sessionIndex;

	UINT result;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
	//if(1 < InterlockedIncrement(&CS.IO_Count))
	{
		//if(CS.Connected && CS.Key == sessionKey)
		{
			result = CS.ReceiveBQ.Peek((char*)&receivedPacketHeader, sizeof(receivedPacketHeader));
			if (sizeof(receivedPacketHeader) == result)
			{
				result = CS.ReceiveBQ.Peek(receivedPacketSPQ_Address->GetQueueAddress(), sizeof(receivedPacketHeader) + receivedPacketHeader.BodySize);
				if (sizeof(receivedPacketHeader) + receivedPacketHeader.BodySize == result)
				{
					receivedPacketSPQ_Address->SetHeader(receivedPacketHeader);
					CS.ReceiveBQ.DecreaseCurrentSize(sizeof(receivedPacketHeader));

					receivedPacketSPQ_Address->IncreaseCurrentSize(receivedPacketHeader.BodySize);
					CS.ReceiveBQ.DecreaseCurrentSize(receivedPacketHeader.BodySize);
				}
				else result = 0;
			}
			else result = 0;
		}
		//else result = 0;
	}
	//else result = 0;

	//InterlockedDecrement(&CSV[sessionIndex].IO_Count);

	return result;
}

unsigned int IOCP_Client::SendPacket(UINT64 sessionKey, SerialPacketQueue<IOCP_PacketHeader>* sendingPacketSPQ_Address)
{
	IOCP_PacketHeader sendingPacketHeader;
	UINT sessionIndex;

	UINT result;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
	if (1 < InterlockedIncrement(&CS.IO_Count))
	{
		if (CS.Connected && CS.Key == sessionKey)
		{
			result = sendingPacketSPQ_Address->GetHeaderSize();
			if (sizeof(sendingPacketHeader) == result)
			{
				sendingPacketHeader = sendingPacketSPQ_Address->GetHeader();

				result = sendingPacketSPQ_Address->GetTotalSize();
				if (sizeof(sendingPacketHeader) + sendingPacketHeader.BodySize == result)
				{
					sendingPacketSPQ_Address->IncreaseReferenceCount();

					if (CS.SendQ.Enqueue(sendingPacketSPQ_Address) == sendingPacketSPQ_Address);
					else
					{
						sendingPacketSPQ_Address->DecreaseReferenceCount();

						result = 0;
					}
				}
				else result = 0;
			}
			else result = 0;
		}
		else result = 0;

		{
			//if(!AWAN_CSV[sessionIndex].ReceiveCount && !AWAN_CSV[sessionIndex].SendCount)
			if (!InterlockedDecrement(&CS.IO_Count))
			{
				if (InterlockedExchange(&CS.Connected, 0)) // && AWAN_CSV[sessionIndex].Key == sessionKey)
				{
					CS.CurrentPhase = IOCP_CLIENT_PHASE_DISCONNECT;

					PostQueuedCompletionStatus(DISCONNECT_CP, NULL, CS.Key, &CS.OverlappedSend);
				}

				//THIS->PostDisconnect(AWAN_CSV[sessionIndex].Key);
				//THIS->Disconnect(AWAN_CSV[sessionIndex].Key);
			}
		}
	}
	else //result = 0;
	{
		InterlockedDecrement(&CS.IO_Count);

		result = 0;
	}

	return result;
}

bool IOCP_Client::Disconnect(UINT64 sessionKey)
{
	UINT sessionIndex;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
	if (1 < InterlockedIncrement(&CS.IO_Count))
	{
		if (CS.Connected && CS.Key == sessionKey)
		{
			shutdown(CS.Socket, SD_BOTH);

			PostQueuedCompletionStatus(COMMON_CP, NULL, CS.Key, &CS.OverlappedSend);
			//CancelIo((HANDLE)CS.Socket);
			//CancelIoEx((HANDLE)CS.Socket, &CS.OverlappedReceive);
		}

		//{
		//	//if(!CSV[sessionIndex].ReceiveCount & !CSV[sessionIndex].SendCount)
		//	if (!InterlockedDecrement(&CS.IO_Count))
		//	{
		//		if (InterlockedExchange(&CS.Connected, 0) && CS.Key == sessionKey)
		//		{
		//			CS.CurrentPhase = IOCP_CLIENT_PHASE_DISCONNECT;
		//
		//			PostQueuedCompletionStatus(DISCONNECT_CP, NULL, CS.Key, &CS.OverlappedSend);
		//		}
		//
		//		//THIS->PostDisconnect(CSV[sessionIndex].Key);
		//		//THIS->Disconnect(CSV[sessionIndex].Key);
		//	}
		//}
	}
	else InterlockedDecrement(&CS.IO_Count);

	return 0;
}

unsigned int IOCP_Client::PostSend(UINT64 sessionKey)
{
	UINT sessionIndex;

	UINT result;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;

	result = CS.SendQ.GetCurrentCount();
	if (result)
	{
		if (InterlockedExchange(&CS.Sendable, 0))
		{
			InterlockedIncrement(&CS.IO_Count);
			++CS.SendCount;

			CS.CurrentPhase = IOCP_CLIENT_PHASE_SEND;

			PostQueuedCompletionStatus(COMMON_CP, NULL, CS.Key, &CS.OverlappedSend);
		}
		else result = 0;
	}
	else result = 0;

	return result;
}

unsigned int IOCP_Client::PostDisconnect(UINT64 sessionKey)
{
	UINT sessionIndex;

	UINT result;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
	if (InterlockedExchange(&CS.Connected, 0) && CS.Key == sessionKey)
	{
		CS.CurrentPhase = IOCP_CLIENT_PHASE_DISCONNECT;

		PostQueuedCompletionStatus(DISCONNECT_CP, NULL, CS.Key, &CS.OverlappedSend);
	}
	else result = 0;

	return result;
}

bool IOCP_Client::Start(LPCWSTR ipAddress, UINT port, UINT workThreadCount, BOOL nagleOff, UINT maximumClientCount)
{
	IP_Address = ipAddress;
	Port = port;
	//MaximumClientCount = maximumClientCount;
	WorkThreadCount = workThreadCount;

	Initialize();

	return 0;
}

void IOCP_Client::Stop()
{
	Terminalize();
}