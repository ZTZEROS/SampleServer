#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "IOCP_Client.h"

unsigned int WINAPI IOCP_Client::ConnectServer(LPVOID selfInstanceAddress)
{
	IOCP_Client* thisInstanceAddress;

	thisInstanceAddress = (IOCP_Client*)selfInstanceAddress;

	thisInstanceAddress->ConnectServer();

	return 0;
}

unsigned int WINAPI IOCP_Client::DisconnectServer(LPVOID selfInstanceAddress)
{
	IOCP_Client* thisInstanceAddress;

	thisInstanceAddress = (IOCP_Client*)selfInstanceAddress;

	thisInstanceAddress->DisconnectServer();

	return 0;
}

unsigned int WINAPI IOCP_Client::WorkClient(LPVOID selfInstanceAddress)
{
	IOCP_Client* thisInstanceAddress;

	thisInstanceAddress = (IOCP_Client*)selfInstanceAddress;

	thisInstanceAddress->OnWorkerThreadBegin();
	thisInstanceAddress->WorkClient();
	thisInstanceAddress->OnWorkerThreadEnd();

	return 0;
}

unsigned int IOCP_Client::ConnectServer()
{
	DWORD transferredSize;
	ULONG_PTR completionKey;
	LPOVERLAPPED overlapped;
	UINT64 sessionKey;
	UINT sessionIndex;

	IOCP_ClientSession clientSession;
	//INT addressLength;
	BOOL keepAliveOn;
	BOOL nagleAlgorithmOff;

	INT result;

	//addressLength = sizeof(clientSession.Address);

	result = 0;

	PostQueuedCompletionStatus(CONNECT_CP, NULL, 0xffffffffffffffff, &CS.OverlappedSend);

	while (SUB_THREAD_ON)
	{
		transferredSize = 0;
		completionKey = 0;
		overlapped = nullptr;
		sessionKey = 0xffffffffffffffff;
		sessionIndex = 0xffffffff;

		GetQueuedCompletionStatus(CONNECT_CP, &transferredSize, &completionKey, &overlapped, INFINITE);

		sessionKey = completionKey;
		sessionIndex = sessionKey >> IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;

		if (overlapped)
		{
			clientSession.Socket = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
			CHECK_INVALID_SOCKET(clientSession.Socket);

			{
				keepAliveOn = 1;
				CHECK_SOCKET_ERROR(setsockopt(clientSession.Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAliveOn, sizeof(keepAliveOn)));
			}

			{
				nagleAlgorithmOff = 1;
				CHECK_SOCKET_ERROR(setsockopt(clientSession.Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&nagleAlgorithmOff, sizeof(nagleAlgorithmOff)));
			}

			clientSession.Address.sin_family = AF_INET;
			//SS.Address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			InetPton(AF_INET, IP_Address.c_str(), &clientSession.Address.sin_addr); //inet_addr(INADDR_ANY); //InetNtop();
			//WSAAddressToString();
			//WSAStringToAddress();
			clientSession.Address.sin_port = htons(Port);

			//CHECK_SOCKET_ERROR(WSAConnect(CS.Socket, (SOCKADDR*)&CS.Address, sizeof(CS.Address), nullptr, nullptr, nullptr, nullptr));
			//CHECK_SOCKET_ERROR(connect(clientSession.Socket, (SOCKADDR*)&clientSession.Address, sizeof(clientSession.Address)));
			result = connect(clientSession.Socket, (SOCKADDR*)&clientSession.Address, sizeof(clientSession.Address));

			//IF_INVALID_SOCKET(clientSession.Socket);
			if (SOCKET_ERROR == result)	PostQueuedCompletionStatus(CONNECT_CP, NULL, sessionKey, &CS.OverlappedSend);
			//ELSE
			else
			{
				if (OnConnectionRequest(CS.Address, CS.Address.sin_port));

				{
					++sessionIndex;

					if (sessionIndex);
					else sessionIndex = 0;

					if (CS.CurrentPhase == IOCP_CLIENT_PHASE_CONNECT)
					{
						++CURRENT_CONNECTION_COUNT;
						sessionKey = sessionIndex;
						sessionKey = sessionKey << IOCP_CLIENT_INDEX_SHIFTED_BIT_COUNT;
						sessionKey += CURRENT_CONNECTION_COUNT;

						IF_NULL(CreateIoCompletionPort((HANDLE)clientSession.Socket, COMMON_CP, sessionKey, NULL))
						ELSE
						{
							CS.Socket = clientSession.Socket;
							CS.Address = clientSession.Address;
							CS.Key = sessionKey;
							CS.Index = sessionIndex;
							CS.Connected = 1;
							CS.Sendable = 1;
							CS.ReceiveCount = 1;
							CS.SendCount = 0;

							ZeroMemory(&CS.OverlappedReceive, sizeof(CS.OverlappedReceive));
							ZeroMemory(&CS.OverlappedSend, sizeof(CS.OverlappedSend));

							InterlockedIncrement(&CS.IO_Count);

							InitializeSRWLock(&CS.Lock);

							OnClientJoin(CS.Address, sessionKey);

							PostQueuedCompletionStatus(COMMON_CP, NULL, sessionKey, &CS.OverlappedReceive);

							CS.CurrentPhase = IOCP_CLIENT_PHASE_UPDATE;
						}
					}
				}
			}
		}
		else break;
	}

	return 0;
}

unsigned int IOCP_Client::DisconnectServer()
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
			//if (InterlockedExchange(&CS.Connected, 0) && CS.Key == sessionKey)
			{
				if (CS.CurrentPhase == IOCP_CLIENT_PHASE_DISCONNECT)
				{
					//ProcessRemainedDatum
					{

					}

					OnClientLeave(sessionKey);

					closesocket(CS.Socket);

					CS.Socket = INVALID_SOCKET;
					CS.Address;

					CS.Key = 0xffffffffffffffff;
					CS.Index = 0xffffffff;

					CS.Connected = 0;
					CS.Sendable = 0;
					CS.ReceiveCount = 0;
					CS.SendCount = 0;

					ZeroMemory(&CS.OverlappedReceive, sizeof(CS.OverlappedReceive));
					ZeroMemory(&CS.OverlappedSend, sizeof(CS.OverlappedSend));

					while (1) //CS.SendQ.GetCurrentCount();
					{
						SerialPacketQueue<IOCP_PacketHeader>* sendedPacketSPQ_Address;

						sendedPacketSPQ_Address = CS.SendQ.Dequeue();

						if (!sendedPacketSPQ_Address) break;

						if (!sendedPacketSPQ_Address->DecreaseReferenceCount())
						{
							InterlockedDecrement(&CURRENT_SPQ_COUNT);
							InterlockedIncrement(&DEALLOCATED_SPQ_COUNT);
							PACKET_SPQ_TLP.Deallocate(sendedPacketSPQ_Address);
						}
					}

					CS.ReceiveQ.Terminalize();
					CS.SendQ.Terminalize();
					CS.ReceiveBQ.Terminalize();
					CS.SendBQ.Terminalize();

					CS.CurrentPhase = IOCP_CLIENT_PHASE_CONNECT;

					PostQueuedCompletionStatus(CONNECT_CP, NULL, CS.Key, &CS.OverlappedSend);
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

unsigned int IOCP_Client::WorkClient()
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
			//if (CS.CurrentPhase == IOCP_CLIENT_PHASE_UPDATE)
			{
				if (&CS.OverlappedReceive == overlapped)
				{
					ZeroMemory(&CS.OverlappedReceive, sizeof(CS.OverlappedReceive));

					if (transferredSize)
					{
						UINT receivedSize;

						receivedSize = transferredSize;
						CS.ReceiveBQ.IncreaseCurrentSize(receivedSize);

						OnRecv(CS.Key, receivedSize);

						CS.CurrentPhase = IOCP_CLIENT_PHASE_UPDATE;
					}

					InterlockedIncrement(&CS.IO_Count);
					++CS.ReceiveCount;

					result = CS.ReceiveBQ.WSAReceive(WSARecv, CS.Socket, &flag, &CS.OverlappedReceive);
					if (result == SOCKET_ERROR)
					{
						if (WSAGetLastError() == WSA_IO_PENDING);
						else
						{
							InterlockedDecrement(&CS.IO_Count);
							--CS.ReceiveCount;

							if (INDICATOR_ON) printf("[WSARecv] sessionIndex : %u, sessionKey : %llu, IO_Count : %u, WSAGetLastError() : %d" CRALF, sessionIndex, CS.Key, CS.IO_Count, WSAGetLastError());
						}
					}

					//InterlockedDecrement(&CS.IO_Count);
					--CS.ReceiveCount;
				}
			}

			if (&CS.OverlappedSend == overlapped)
			{
				if (transferredSize)
				{
					UINT sendedSize;

					sendedSize = transferredSize;

					while (sendedSize)
					{
						SerialPacketQueue<IOCP_PacketHeader>* sendedPacketSPQ_Address;

						sendedPacketSPQ_Address = CS.SendQ.Dequeue();

						sendedSize -= sendedPacketSPQ_Address->GetTotalSize();

						if (!sendedPacketSPQ_Address->DecreaseReferenceCount())
						{
							InterlockedDecrement(&CURRENT_SPQ_COUNT);
							InterlockedIncrement(&DEALLOCATED_SPQ_COUNT);
							PACKET_SPQ_TLP.Deallocate(sendedPacketSPQ_Address);
						}
					}

					if (sendedSize) sendedSize = transferredSize - sendedSize;
					else sendedSize = transferredSize;

					CS.CurrentPhase = IOCP_CLIENT_PHASE_UPDATE;

					OnSend(CS.Key, sendedSize);

					//InterlockedExchange(&CS.Sendable, 1);
				}
				//else
				//sndPost
				//if (InterlockedExchange(&CS.Sendable, 0))
				{
					WSABUF sendingWSA_BufferA[IOCP_CLIENT_WSA_BUFFER_COUNT];
					UINT sendingWSA_BufferCount;

					sendingWSA_BufferCount = CS.SendQ.GetCurrentCount();
					if (IOCP_CLIENT_WSA_BUFFER_COUNT < sendingWSA_BufferCount) sendingWSA_BufferCount = IOCP_CLIENT_WSA_BUFFER_COUNT;

					if (!sendingWSA_BufferCount) InterlockedExchange(&CS.Sendable, 1);
					else
					{
						FOR(i, sendingWSA_BufferCount)
						{
							SerialPacketQueue<IOCP_PacketHeader>* sendingPacketSPQ_Address;
							WSABUF wsaBuffer;

							sendingPacketSPQ_Address = CS.SendQ.Peek(i); //Dequeue(); //improvable point
							wsaBuffer.buf = sendingPacketSPQ_Address->GetQueueAddress();
							wsaBuffer.len = sendingPacketSPQ_Address->GetTotalSize();

							sendingWSA_BufferA[i] = wsaBuffer;
						}

						InterlockedIncrement(&CS.IO_Count);
						++CS.SendCount;

						result = WSASend(CS.Socket, sendingWSA_BufferA, sendingWSA_BufferCount, nullptr, flag, &CS.OverlappedSend, nullptr);
						if (result == SOCKET_ERROR)
						{
							if (WSAGetLastError() == WSA_IO_PENDING);
							else
							{
								InterlockedDecrement(&CS.IO_Count);
								--CS.SendCount;

								if (INDICATOR_ON) printf("[WSASend] sessionIndex : %u, sessionKey : %llu, IO_Count : %u, WSAGetLastError() : %d" CRALF, sessionIndex, CS.Key, CS.IO_Count, WSAGetLastError());
							}
						}
					}
				}

				//InterlockedDecrement(&CS.IO_Count);
				--CS.SendCount;
			}

			//if (!CS.ReceiveCount && !CS.SendCount)
			if (!InterlockedDecrement(&CS.IO_Count))
			{
				if (InterlockedExchange(&CS.Connected, 0) && CS.Key == sessionKey)
				{
					CS.CurrentPhase = IOCP_CLIENT_PHASE_DISCONNECT;

					PostQueuedCompletionStatus(DISCONNECT_CP, NULL, CS.Key, overlapped);
				}

				//PostDisconnect(CS.Key);
				//Disconnect(CS.Key);
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
