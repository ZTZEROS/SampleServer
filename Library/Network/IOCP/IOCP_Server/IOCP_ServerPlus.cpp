#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "IOCP_Server.h"

unsigned int WINAPI IOCP_Server::AcceptClient(LPVOID selfInstanceAddress)
{
	IOCP_Server* thisInstanceAddress;

	thisInstanceAddress = (IOCP_Server*)selfInstanceAddress;

	thisInstanceAddress->AcceptClient();

	return 0;
}

unsigned int WINAPI IOCP_Server::DisconnectClient(LPVOID selfInstanceAddress)
{
	IOCP_Server* thisInstanceAddress;

	thisInstanceAddress = (IOCP_Server*)selfInstanceAddress;

	thisInstanceAddress->DisconnectClient();

	return 0;
}

unsigned int WINAPI IOCP_Server::WorkServer(LPVOID selfInstanceAddress)
{
	IOCP_Server* thisInstanceAddress;

	thisInstanceAddress = (IOCP_Server*)selfInstanceAddress;

	thisInstanceAddress->OnWorkerThreadBegin();
	thisInstanceAddress->WorkServer();
	thisInstanceAddress->OnWorkerThreadEnd();

	return 0;
}

unsigned int IOCP_Server::AcceptClient()
{
	IOCP_ClientSession clientSession;
	INT addressLength;
	UINT64 sessionKey;
	UINT sessionIndex;

	addressLength = sizeof(clientSession.Address);
	sessionKey = 0xffffffffffffffff;
	sessionIndex = 0xffffffff;

	while (SUB_THREAD_ON)
	{
		clientSession.Socket = WSAAccept(SS.Socket, (SOCKADDR*)&clientSession.Address, &addressLength, nullptr, NULL);
		CHECK_INVALID_SOCKET(clientSession.Socket);
		//IF_INVALID_SOCKET(acceptedclientSession.Socket);
		//ELSE
		{
			if (OnConnectionRequest(clientSession.Address, clientSession.Address.sin_port));

			while (1)
			{
				++sessionIndex;

				if (sessionIndex < CSV.size());
				else sessionIndex = 0;

				if (CSV[sessionIndex].CurrentPhase == IOCP_CLIENT_PHASE_ACCEPT)
				{
					++CURRENT_ACCEPTION_COUNT;
					sessionKey = sessionIndex;
					sessionKey = sessionKey << IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
					sessionKey += CURRENT_ACCEPTION_COUNT;

					IF_NULL(CreateIoCompletionPort((HANDLE)clientSession.Socket, COMMON_CP, sessionKey, NULL))
					ELSE
					{
						CSV[sessionIndex].Socket = clientSession.Socket;
						CSV[sessionIndex].Address = clientSession.Address;
						CSV[sessionIndex].Key = sessionKey;
						CSV[sessionIndex].Index = sessionIndex;
						CSV[sessionIndex].Connected = 1;
						CSV[sessionIndex].Sendable = 1;
						CSV[sessionIndex].ReceiveCount = 1;
						CSV[sessionIndex].SendCount = 0;

						ZeroMemory(&CSV[sessionIndex].OverlappedReceive, sizeof(CSV[sessionIndex].OverlappedReceive));
						ZeroMemory(&CSV[sessionIndex].OverlappedSend, sizeof(CSV[sessionIndex].OverlappedSend));

						//CSV[sessionIndex].IO_Count = 1;
						InterlockedIncrement(&CSV[sessionIndex].IO_Count);

						InitializeSRWLock(&CSV[sessionIndex].Lock);

						OnClientJoin(CSV[sessionIndex].Address, sessionKey);

						PostQueuedCompletionStatus(COMMON_CP, NULL, sessionKey, &CSV[sessionIndex].OverlappedReceive);
						//PostQueuedCompletionStatus(RECEIVE_CP, NULL, sessionKey, &CSV[sessionIndex].OverlappedReceive);

						CSV[sessionIndex].CurrentPhase = IOCP_CLIENT_PHASE_UPDATE;

						break;
					}
				}
			}
		}
	}

	return 0;
}

unsigned int IOCP_Server::DisconnectClient()
{
	DWORD transferredSize;
	ULONG_PTR completionKey;
	LPOVERLAPPED overlapped;
	UINT64 sessionKey;
	UINT sessionIndex;

	while (SUB_THREAD_ON)
	{
		transferredSize = 0;
		completionKey = 0;
		overlapped = nullptr;
		sessionKey = 0xffffffffffffffff;
		sessionIndex = 0xffffffff;

		GetQueuedCompletionStatus(DISCONNECT_CP, &transferredSize, &completionKey, &overlapped, INFINITE);

		sessionKey = completionKey;
		sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;

		if (overlapped)
		{
			//if (InterlockedExchange(&CSV[sessionIndex].Connected, 0) && CSV[sessionIndex].Key == sessionKey)
			{
				if (CSV[sessionIndex].CurrentPhase == IOCP_CLIENT_PHASE_DISCONNECT)
				{
					//ProcessRemainedDatum
					{

					}

					OnClientLeave(sessionKey);

					closesocket(CSV[sessionIndex].Socket);

					CSV[sessionIndex].Socket = INVALID_SOCKET;
					CSV[sessionIndex].Address;

					CSV[sessionIndex].Key = 0xffffffffffffffff;
					CSV[sessionIndex].Index = 0xffffffff;

					CSV[sessionIndex].Connected = 0;
					CSV[sessionIndex].Sendable = 0;
					CSV[sessionIndex].ReceiveCount = 0;
					CSV[sessionIndex].SendCount = 0;

					ZeroMemory(&CSV[sessionIndex].OverlappedReceive, sizeof(CSV[sessionIndex].OverlappedReceive));
					ZeroMemory(&CSV[sessionIndex].OverlappedSend, sizeof(CSV[sessionIndex].OverlappedSend));

					while (1) //CSV[sessionIndex].SendQ.GetCurrentCount();
					{
						SerialPacketQueue<IOCP_PacketHeader>* sendedPacketSPQ_Address;

						sendedPacketSPQ_Address = CSV[sessionIndex].SendQ.Dequeue();

						if (!sendedPacketSPQ_Address) break;

						if (!sendedPacketSPQ_Address->DecreaseReferenceCount())
						{
							InterlockedDecrement(&CURRENT_SPQ_COUNT);
							InterlockedIncrement(&DEALLOCATED_SPQ_COUNT);
							//delete sendedPacketSPQ_Address;
							PACKET_SPQ_TLP.Deallocate(sendedPacketSPQ_Address);
						}
					}

					CSV[sessionIndex].ReceiveQ.Terminalize();
					CSV[sessionIndex].SendQ.Terminalize();
					CSV[sessionIndex].ReceiveBQ.Terminalize();
					CSV[sessionIndex].SendBQ.Terminalize();

					//CSV[sessionIndex].IO_Count = 0;

					CSV[sessionIndex].CurrentPhase = IOCP_CLIENT_PHASE_ACCEPT;
				}
			}
		}
		else
		{
			if (GetLastError() == ERROR_ABANDONED_WAIT_0) break;

			if (completionKey || transferredSize) continue;
			else break;
		}
	}

	return 0;
}

unsigned int IOCP_Server::WorkServer()
{
	DWORD transferredSize;
	ULONG_PTR completionKey;
	LPOVERLAPPED overlapped;
	UINT64 sessionKey;
	UINT sessionIndex;

	DWORD flag;
	INT result;

	while (SUB_THREAD_ON)
	{
		transferredSize = 0;
		completionKey = 0;
		overlapped = nullptr;
		sessionKey = 0xffffffffffffffff;
		sessionIndex = 0xffffffff;

		flag = 0;

		GetQueuedCompletionStatus(COMMON_CP, &transferredSize, &completionKey, &overlapped, INFINITE);

		sessionKey = completionKey;
		sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;

		if (overlapped)
		{
			//if(AWAN_CSV[sessionIndex].CurrentPhase == ADVANCED_WAN_CLIENT_PHASE_UPDATE)
			{
				if (&CSV[sessionIndex].OverlappedReceive == overlapped)
				{
					ZeroMemory(&CSV[sessionIndex].OverlappedReceive, sizeof(CSV[sessionIndex].OverlappedReceive));

					if (transferredSize)
					{
						UINT receivedSize;

						receivedSize = transferredSize;
						CSV[sessionIndex].ReceiveBQ.IncreaseCurrentSize(receivedSize);

						OnRecv(CSV[sessionIndex].Key, receivedSize);

						//AWAN_CSV[sessionIndex].CurrentPhase = ADVANCED_WAN_CLIENT_PHASE_UPDATE;
					}

					InterlockedIncrement(&CSV[sessionIndex].IO_Count);
					++CSV[sessionIndex].ReceiveCount;

					result = CSV[sessionIndex].ReceiveBQ.WSAReceive(WSARecv, CSV[sessionIndex].Socket, &flag, &CSV[sessionIndex].OverlappedReceive);
					if (result == SOCKET_ERROR)
					{
						if (WSAGetLastError() == WSA_IO_PENDING);
						else
						{
							InterlockedDecrement(&CSV[sessionIndex].IO_Count);
							--CSV[sessionIndex].ReceiveCount;

							//if (INDICATOR_ON) printf("[WSARecv] sessionIndex : %u, sessionKey : %llu, IO_Count : %u, WSAGetLastError() : %d" CRALF, sessionIndex, CSV[sessionIndex].Key, CSV[sessionIndex].IO_Count, WSAGetLastError());
						}
					}

					//InterlockedDecrement(&CSV[sessionIndex].IO_Count);
					--CSV[sessionIndex].ReceiveCount;
				}
			}

			if (&CSV[sessionIndex].OverlappedSend == overlapped)
			{
				if (transferredSize)
				{
					UINT sendedSize;

					sendedSize = transferredSize;

					while (sendedSize)
					{
						SerialPacketQueue<IOCP_PacketHeader>* sendedPacketSPQ_Address;

						sendedPacketSPQ_Address = CSV[sessionIndex].SendQ.Dequeue();

						sendedSize -= sendedPacketSPQ_Address->GetTotalSize();

						if (!sendedPacketSPQ_Address->DecreaseReferenceCount())
						{
							InterlockedDecrement(&CURRENT_SPQ_COUNT);
							InterlockedIncrement(&DEALLOCATED_SPQ_COUNT);
							//delete sendedPacketSPQ_Address;
							PACKET_SPQ_TLP.Deallocate(sendedPacketSPQ_Address);
						}
					}

					if (sendedSize) sendedSize = transferredSize - sendedSize;
					else sendedSize = transferredSize;

					CSV[sessionIndex].CurrentPhase = IOCP_CLIENT_PHASE_UPDATE;

					OnSend(CSV[sessionIndex].Key, sendedSize);

					//InterlockedExchange(&CSV[sessionIndex].Sendable, 1);
				}
				//else
				//sndPost
				//if (InterlockedExchange(&CSV[sessionIndex].Sendable, 0))
				{
					WSABUF sendingWSA_BufferA[IOCP_SERVER_WSA_BUFFER_COUNT];
					UINT sendingWSA_BufferCount;

					sendingWSA_BufferCount = CSV[sessionIndex].SendQ.GetCurrentCount();
					if (IOCP_SERVER_WSA_BUFFER_COUNT < sendingWSA_BufferCount) sendingWSA_BufferCount = IOCP_SERVER_WSA_BUFFER_COUNT;

					if (!sendingWSA_BufferCount) InterlockedExchange(&CSV[sessionIndex].Sendable, 1);
					else
					{
						FOR(i, sendingWSA_BufferCount)
						{
							SerialPacketQueue<IOCP_PacketHeader>* sendingPacketSPQ_Address;
							WSABUF wsaBuffer;

							sendingPacketSPQ_Address = CSV[sessionIndex].SendQ.Peek(i); //Dequeue();
							wsaBuffer.buf = sendingPacketSPQ_Address->GetQueueAddress();
							wsaBuffer.len = sendingPacketSPQ_Address->GetTotalSize();

							sendingWSA_BufferA[i] = wsaBuffer;
						}

						InterlockedIncrement(&CSV[sessionIndex].IO_Count);
						++CSV[sessionIndex].SendCount;

						result = WSASend(CSV[sessionIndex].Socket, sendingWSA_BufferA, sendingWSA_BufferCount, nullptr, flag, &CSV[sessionIndex].OverlappedSend, nullptr);
						if (result == SOCKET_ERROR)
						{
							if (WSAGetLastError() == WSA_IO_PENDING);
							else
							{
								InterlockedDecrement(&CSV[sessionIndex].IO_Count);
								--CSV[sessionIndex].SendCount;

								//if (INDICATOR_ON) printf("[WSASend] sessionIndex : %u, sessionKey : %llu, IO_Count : %u, WSAGetLastError() : %d" CRALF, sessionIndex, CSV[sessionIndex].Key, CSV[sessionIndex].IO_Count, WSAGetLastError());
							}
						}
					}
				}

				//InterlockedDecrement(&CSV[sessionIndex].IO_Count);
				--CSV[sessionIndex].SendCount;
			}

			//if(!CSV[sessionIndex].ReceiveCount && !CSV[sessionIndex].SendCount)
			if (!InterlockedDecrement(&CSV[sessionIndex].IO_Count))
			{
				if (InterlockedExchange(&CSV[sessionIndex].Connected, 0) && CSV[sessionIndex].Key == sessionKey)
				{
					CSV[sessionIndex].CurrentPhase = IOCP_CLIENT_PHASE_DISCONNECT;

					PostQueuedCompletionStatus(DISCONNECT_CP, NULL, CSV[sessionIndex].Key, overlapped);
				}

				//PostDisconnect(AWAN_CSV[sessionIndex].Key);
				//Disconnect(AWAN_CSV[sessionIndex].Key);
			}
		}
		else
		{
			if (GetLastError() == ERROR_ABANDONED_WAIT_0) break;

			if (completionKey || transferredSize) continue;
			else break;
		}
	}

	return 0;
}