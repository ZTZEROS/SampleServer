#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "IOCP_InnerServer.h"

unsigned int IOCP_InnerServer::ReceivePacket(UINT64 sessionKey, SerialPacketQueue<IOCP_PacketHeader>* receivedPacketSPQ_Address)
{
	IOCP_PacketHeader receivedPacketHeader;
	UINT sessionIndex;

	UINT result;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
	//if(1 < InterlockedIncrement(&CSV[sessionIndex].IO_Count))
	{
		//if(CSV[sessionIndex].Connected && CSV[sessionIndex].Key == sessionKey)
		{
			result = CSV[sessionIndex].ReceiveBQ.Peek((char*)&receivedPacketHeader, sizeof(receivedPacketHeader));
			if (sizeof(receivedPacketHeader) == result)
			{
				result = CSV[sessionIndex].ReceiveBQ.Peek(receivedPacketSPQ_Address->GetQueueAddress(), sizeof(receivedPacketHeader) + receivedPacketHeader.BodySize);
				if(sizeof(receivedPacketHeader) + receivedPacketHeader.BodySize == result)
				{
					receivedPacketSPQ_Address->SetHeader(receivedPacketHeader);
					CSV[sessionIndex].ReceiveBQ.DecreaseCurrentSize(sizeof(receivedPacketHeader));

					receivedPacketSPQ_Address->IncreaseCurrentSize(receivedPacketHeader.BodySize);
					CSV[sessionIndex].ReceiveBQ.DecreaseCurrentSize(receivedPacketHeader.BodySize);
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

unsigned int IOCP_InnerServer::SendPacket(UINT64 sessionKey, SerialPacketQueue<IOCP_PacketHeader>* sendingPacketSPQ_Address)
{
	IOCP_PacketHeader sendingPacketHeader;
	UINT sessionIndex;

	UINT result;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
	if (1 < InterlockedIncrement(&CSV[sessionIndex].IO_Count))
	{
		if (CSV[sessionIndex].Connected && CSV[sessionIndex].Key == sessionKey)
		{
			result = sendingPacketSPQ_Address->GetHeaderSize();
			if (sizeof(sendingPacketHeader) == result)
			{
				sendingPacketHeader = sendingPacketSPQ_Address->GetHeader();

				result = sendingPacketSPQ_Address->GetTotalSize();
				if (sizeof(sendingPacketHeader) + sendingPacketHeader.BodySize == result)
				{
					sendingPacketSPQ_Address->IncreaseReferenceCount();

					if (CSV[sessionIndex].SendQ.Enqueue(sendingPacketSPQ_Address) == sendingPacketSPQ_Address);
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
			if (!InterlockedDecrement(&CSV[sessionIndex].IO_Count))
			{
				if (InterlockedExchange(&CSV[sessionIndex].Connected, 0)) // && AWAN_CSV[sessionIndex].Key == sessionKey)
				{
					CSV[sessionIndex].CurrentPhase = IOCP_CLIENT_PHASE_DISCONNECT;

					PostQueuedCompletionStatus(DISCONNECT_CP, NULL, CSV[sessionIndex].Key, &CSV[sessionIndex].OverlappedSend);
				}

				//THIS->PostDisconnect(AWAN_CSV[sessionIndex].Key);
				//THIS->Disconnect(AWAN_CSV[sessionIndex].Key);
			}
		}
	}
	else //result = 0;
	{
		InterlockedDecrement(&CSV[sessionIndex].IO_Count);

		result = 0;
	}

	return result;
}

bool IOCP_InnerServer::Disconnect(UINT64 sessionKey)
{
	UINT sessionIndex;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
	if (1 < InterlockedIncrement(&CSV[sessionIndex].IO_Count))
	{
		if (CSV[sessionIndex].Connected && CSV[sessionIndex].Key == sessionKey)
		{
			shutdown(CSV[sessionIndex].Socket, SD_BOTH);

			PostQueuedCompletionStatus(COMMON_CP, NULL, CSV[sessionIndex].Key, &CSV[sessionIndex].OverlappedSend);
			//CancelIo((HANDLE)CSV[sessionIndex].Socket);
			//CancelIoEx((HANDLE)CSV[sessionIndex].Socket, &CSV[sessionIndex].OverlappedReceive);
		}

		//{
		//	//if(!CSV[sessionIndex].ReceiveCount & !CSV[sessionIndex].SendCount)
		//	if (!InterlockedDecrement(&CSV[sessionIndex].IO_Count))
		//	{
		//		if (InterlockedExchange(&CSV[sessionIndex].Connected, 0) && CSV[sessionIndex].Key == sessionKey)
		//		{
		//			CSV[sessionIndex].CurrentPhase = IOCP_CLIENT_PHASE_DISCONNECT;
		//
		//			PostQueuedCompletionStatus(DISCONNECT_CP, NULL, CSV[sessionIndex].Key, &CSV[sessionIndex].OverlappedSend);
		//		}
		//
		//		//THIS->PostDisconnect(CSV[sessionIndex].Key);
		//		//THIS->Disconnect(CSV[sessionIndex].Key);
		//	}
		//}
	}
	else InterlockedDecrement(&CSV[sessionIndex].IO_Count);

	return 0;
}

unsigned int IOCP_InnerServer::PostConnect(UINT64 sessionKey)
{
	UINT sessionIndex;

	//UINT result;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
	//if (!CSV[sessionIndex].Connected && CSV[sessionIndex].Key == sessionKey)
	{
		CSV[sessionIndex].CurrentPhase = IOCP_CLIENT_PHASE_CONNECT;

		PostQueuedCompletionStatus(CONNECT_CP, NULL, CSV[sessionIndex].Key, &CSV[sessionIndex].OverlappedSend);
	}
	//else result = 0;

	//return result;
	return 0;
}

unsigned int IOCP_InnerServer::PostSend(UINT64 sessionKey)
{
	UINT sessionIndex;

	UINT result;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;

	result = CSV[sessionIndex].SendQ.GetCurrentCount();
	if (result)
	{
		if (InterlockedExchange(&CSV[sessionIndex].Sendable, 0))
		{
			InterlockedIncrement(&CSV[sessionIndex].IO_Count);
			++CSV[sessionIndex].SendCount;

			CSV[sessionIndex].CurrentPhase = IOCP_CLIENT_PHASE_SEND;

			PostQueuedCompletionStatus(COMMON_CP, NULL, CSV[sessionIndex].Key, &CSV[sessionIndex].OverlappedSend);
		}
		else result = 0;
	}
	else result = 0;

	return result;
}

unsigned int IOCP_InnerServer::PostDisconnect(UINT64 sessionKey)
{
	UINT sessionIndex;

	UINT result;

	sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
	if (InterlockedExchange(&CSV[sessionIndex].Connected, 0) && CSV[sessionIndex].Key == sessionKey)
	{
		CSV[sessionIndex].CurrentPhase = IOCP_CLIENT_PHASE_DISCONNECT;

		PostQueuedCompletionStatus(DISCONNECT_CP, NULL, CSV[sessionIndex].Key, &CSV[sessionIndex].OverlappedSend);
	}
	else result = 0;

	return result;
}

bool IOCP_InnerServer::Start(LPCWSTR ipAddress, UINT port, UINT workThreadCount, BOOL nagleOff, UINT maximumClientCount)
{
	IP_Address = ipAddress;
	Port = port;
	MaximumClientCount = maximumClientCount;
	WorkThreadCount = workThreadCount;

	Initialize();

	PostConnect(0xffffffffffffffff + CSV.size() / 2);

	return 0;
}

void IOCP_InnerServer::Stop()
{
	Terminalize();
}
