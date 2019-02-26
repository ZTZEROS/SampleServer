#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "IOCP_Server.h"

IOCP_Server::IOCP_Server()
{
	IP_Address = L"127.0.0.1";
	Port = 27050;
	MaximumClientCount = 16384;
	WorkThreadCount = 4;
}

IOCP_Server::~IOCP_Server() {}

bool IOCP_Server::Initialize()
{
	WSADATA wsaDatum;
	//WSAPROTOCOL_INFO wsaProtocolInformation;
	//LINGER lingeringTime;
	//BOOL addressReuseOn;
	BOOL keepAliveOn;
	BOOL nagleAlgorithmOff;
	//INT socketReceiveBufferSize;
	//INT socketSendBufferSize;
	//u_long nonblocked;

	SYSTEM_INFO systemInformation;
	IOCP_SubThread subThread;

	{
		{
			CSV.resize(MaximumClientCount);
			SUB_THREAD_V.resize(IOCP_SERVER_SUB_THREAD_COUNT + WorkThreadCount);

			{
				SS.Socket = INVALID_SOCKET;

				ZeroMemory(&SS.Address, sizeof(SS.Address));

				SS.Key = 0;
				SS.Index = 0;
				SS.Connected = 0;
				SS.Sendable = 0;
				SS.ReceiveCount = 0;
				SS.SendCount = 0;

				ZeroMemory(&SS.OverlappedReceive, sizeof(SS.OverlappedReceive));
				ZeroMemory(&SS.OverlappedSend, sizeof(SS.OverlappedSend));

				InitializeSRWLock(&SS.Lock);
			}

			FOR(i, CSV.size())
			{
				CSV[i].Socket = INVALID_SOCKET;

				ZeroMemory(&CSV[i].Address, sizeof(CSV[i].Address));

				CSV[i].Key = 0;
				CSV[i].Index = 0;
				CSV[i].Connected = 0;
				CSV[i].Sendable = 0;
				CSV[i].ReceiveCount = 0;
				CSV[i].SendCount = 0;

				ZeroMemory(&CSV[i].OverlappedReceive, sizeof(CSV[i].OverlappedReceive));
				ZeroMemory(&CSV[i].OverlappedSend, sizeof(CSV[i].OverlappedSend));

				CSV[i].IO_Count = 0;

				CSV[i].CurrentPhase = IOCP_CLIENT_PHASE_ACCEPT;

				InitializeSRWLock(&CSV[i].Lock);
			}

			SUB_THREAD_ON = 1;
			CURRENT_ACCEPTION_COUNT = 0;

			COMMON_CP = nullptr;
			DISCONNECT_CP = nullptr;

			SelfInstanceAddress = this;
		}

		{
			//PACKET_SPQ_TLP.Initialize();

			CURRENT_SPQ_COUNT = 0;
			ALLOCATED_SPQ_COUNT = 0;
			DEALLOCATED_SPQ_COUNT = 0;

			THREAD_CONTEXT_SWITCH_ON = 0;
			INDICATOR_ON = 0;
		}
	}

	{
		CHECK_ERROR(WSAStartup(0x0202, &wsaDatum));

		SS.Socket = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
		CHECK_INVALID_SOCKET(SS.Socket);

		//{
		//	lingeringTime.l_onoff = 1;
		//	lingeringTime.l_linger = 0;
		//	CHECK_SOCKET_ERROR(setsockopt(SS.Socket, SOL_SOCKET, SO_LINGER, (char*)&lingeringTime, sizeof(lingeringTime)));
		//}

		//{
		//	addressReuseOn = 1;
		//	CHECK_SOCKET_ERROR(setsockopt(SS.Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&addressReuseOn, sizeof(addressReuseOn)));
		//}

		{
			keepAliveOn = 1;
			CHECK_SOCKET_ERROR(setsockopt(SS.Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAliveOn, sizeof(keepAliveOn)));
		}

		{
			nagleAlgorithmOff = 1;
			CHECK_SOCKET_ERROR(setsockopt(SS.Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&nagleAlgorithmOff, sizeof(nagleAlgorithmOff)));
		}

		//{
		//	socketReceiveBufferSize = 0;
		//	CHECK_SOCKET_ERROR(setsockopt(SS.Socket, SOL_SOCKET, SO_RCVBUF, (char*)&socketReceiveBufferSize, sizeof(socketReceiveBufferSize)));
		//}

		//{
		//	socketSendBufferSize = 0;
		//	CHECK_SOCKET_ERROR(setsockopt(SS.Socket, SOL_SOCKET, SO_SNDBUF, (char*)&socketSendBufferSize, sizeof(socketSendBufferSize)));
		//}

		//{
		//	nonblocked = 1;
		//	CHECK_SOCKET_ERROR(ioctlsocket(SS.Socket, FIONBIO, &nonblocked));
		//}

		SS.Address.sin_family = AF_INET;
		SS.Address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		//InetPton(AF_INET, ipAddress, &SS.Address.sin_addr); //inet_addr(INADDR_ANY); //InetNtop();
		//WSAAddressToString();
		//WSAStringToAddress();
		SS.Address.sin_port = htons(Port);

		CHECK_SOCKET_ERROR(bind(SS.Socket, (SOCKADDR*)&SS.Address, sizeof(SS.Address)));
		CHECK_SOCKET_ERROR(listen(SS.Socket, SOMAXCONN));
	}

	{
		timeBeginPeriod(1);

		TIMER.Initialize();

		srand(time(nullptr));
		//srand(2705);

		GetSystemInfo(&systemInformation);
	}

	{
		if (SUB_THREAD_V.size() - IOCP_SERVER_SUB_THREAD_COUNT);
		else SUB_THREAD_V.resize(IOCP_SERVER_SUB_THREAD_COUNT + systemInformation.dwNumberOfProcessors * 2);

		COMMON_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, SUB_THREAD_V.size() - IOCP_SERVER_SUB_THREAD_COUNT);
		//ACCEPT_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);
		//RECEIVE_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);
		//UPDATE_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);
		//SEND_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);
		DISCONNECT_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);

		subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, AcceptClient, this, 0, &subThread.IdentificationNumber);
		SUB_THREAD_V[IOCP_SERVER_SUB_THREAD_ACCEPT] = subThread;

		//subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, ReceivePacket, this, 0, &subThread.IdentificationNumber);
		//AWAN_SUB_THREAD_V[IOCP_SERVER_SUB_THREAD_RECEIVE] = subThread;
		//
		//subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, UpdateServer, this, 0, &subThread.IdentificationNumber);
		//AWAN_SUB_THREAD_V[IOCP_SERVER_SUB_THREAD_UPDATE] = subThread;
		//
		//subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, SendPacket, this, 0, &subThread.IdentificationNumber);
		//AWAN_SUB_THREAD_V[IOCP_SERVER_SUB_THREAD_SEND] = subThread;

		subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, DisconnectClient, this, 0, &subThread.IdentificationNumber);
		SUB_THREAD_V[IOCP_SERVER_SUB_THREAD_DISCONNECT] = subThread;

		FOR(i, SUB_THREAD_V.size() - IOCP_SERVER_SUB_THREAD_COUNT)
		{
			subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, WorkServer, this, 0, &subThread.IdentificationNumber);
			SUB_THREAD_V[IOCP_SERVER_SUB_THREAD_COUNT + i] = subThread;
		}
	}
	
	return 0;
}

bool IOCP_Server::Terminalize()
{
	HANDLE_V subThreadHandleV;

	subThreadHandleV.resize(SUB_THREAD_V.size());

	{
		closesocket(SS.Socket); //close accept thread

		FOR(i, CSV.size()) //induct client disconnection
		{
			shutdown(CSV[i].Socket, SD_BOTH);
		}

		FOR(i, CSV.size()) //confirm disconnection
		{
			if (CSV[i].CurrentPhase != IOCP_CLIENT_PHASE_ACCEPT)
			{
				--i;

				Sleep(0);
			}
		}

		SUB_THREAD_ON = 0;
		
		PostQueuedCompletionStatus(DISCONNECT_CP, NULL, NULL, nullptr); //close disconnect thread

		FOR(i, SUB_THREAD_V.size() - IOCP_SERVER_SUB_THREAD_COUNT) //close work thread
		{
			PostQueuedCompletionStatus(COMMON_CP, NULL, NULL, nullptr);
		}

		FOR(i, subThreadHandleV.size())
		{
			subThreadHandleV[i] = SUB_THREAD_V[i].Handle;
		}

		WaitForMultipleObjects(SUB_THREAD_V.size(), &subThreadHandleV[0], 1, INFINITE); //confirm thread closed

		CloseHandle(COMMON_CP);
		//CloseHandle(ACCEPT_CP);
		//CloseHandle(RECEIVE_CP);
		//CloseHandle(UPDATE_CP);
		//CloseHandle(SEND_CP);
		CloseHandle(DISCONNECT_CP);

		FOR(i, SUB_THREAD_V.size())
		{
			CloseHandle(SUB_THREAD_V[i].Handle);
		}

		CSV.clear();
		SUB_THREAD_V.clear();
	}

	{
		timeEndPeriod(1);

		TIMER.Termimalize();

		PACKET_SPQ_TLP.Terminalize();
	}

	{
		WSACleanup();
	}

	{
		printf(
			"CURRENT_SPQ_COUNT : %u" CRALF
			"ALLOCATED_SPQ_COUNT : %u" CRALF
			"DEALLOCATED_SPQ_COUNT : %u" CRALF
			CRALF
			,
			CURRENT_SPQ_COUNT,
			ALLOCATED_SPQ_COUNT,
			DEALLOCATED_SPQ_COUNT
			);
	}

	return 0;
}