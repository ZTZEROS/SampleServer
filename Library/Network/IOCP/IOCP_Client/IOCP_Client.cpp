#include "FirstOrderLibrary.h"
#include "SecondOrderLibrary.h"
#include "IOCP_Client.h"

IOCP_Client::IOCP_Client()
{
	IP_Address = L"127.0.0.1";
	Port = 27050;
	//MaximumClientCount = 1;
	WorkThreadCount = 2;
}

IOCP_Client::~IOCP_Client() {}

bool IOCP_Client::Initialize()
{
	WSADATA wsaDatum;
	//WSAPROTOCOL_INFO wsaProtocolInformation;
	//LINGER lingeringTime;
	//BOOL addressReuseOn;
	//BOOL keepAliveOn;
	//BOOL nagleAlgorithmOff;
	//INT socketReceiveBufferSize;
	//INT socketSendBufferSize;
	//u_long nonblocked;

	SYSTEM_INFO systemInformation;
	IOCP_SubThread subThread;

	{
		{
			SUB_THREAD_V.resize(IOCP_CLIENT_SUB_THREAD_COUNT + WorkThreadCount);

			{
				CS.Socket = INVALID_SOCKET;

				ZeroMemory(&CS.Address, sizeof(CS.Address));

				CS.Key = 0;
				CS.Index = 0;
				CS.Connected = 0;
				CS.Sendable = 0;
				CS.ReceiveCount = 0;
				CS.SendCount = 0;

				ZeroMemory(&CS.OverlappedReceive, sizeof(CS.OverlappedReceive));
				ZeroMemory(&CS.OverlappedSend, sizeof(CS.OverlappedSend));

				CS.IO_Count = 0;

				CS.CurrentPhase = IOCP_CLIENT_PHASE_CONNECT;

				InitializeSRWLock(&CS.Lock);
			}

			SUB_THREAD_ON = 1;
			CURRENT_CONNECTION_COUNT = 0;

			COMMON_CP = nullptr;
			CONNECT_CP = nullptr;
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

		//CS.Socket = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
		//CHECK_INVALID_SOCKET(CS.Socket);

		//{
		//	lingeringTime.l_onoff = 1;
		//	lingeringTime.l_linger = 0;
		//	CHECK_SOCKET_ERROR(setsockopt(CS.Socket, SOL_SOCKET, SO_LINGER, (char*)&lingeringTime, sizeof(lingeringTime)));
		//}

		//{
		//	addressReuseOn = 1;
		//	CHECK_SOCKET_ERROR(setsockopt(CS.Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&addressReuseOn, sizeof(addressReuseOn)));
		//}

		//{
		//	keepAliveOn = 1;
		//	CHECK_SOCKET_ERROR(setsockopt(CS.Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAliveOn, sizeof(keepAliveOn)));
		//}

		//{
		//	nagleAlgorithmOff = 1;
		//	CHECK_SOCKET_ERROR(setsockopt(CS.Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&nagleAlgorithmOff, sizeof(nagleAlgorithmOff)));
		//}

		//{
		//	socketReceiveBufferSize = 0;
		//	CHECK_SOCKET_ERROR(setsockopt(CS.Socket, SOL_SOCKET, SO_RCVBUF, (char*)&socketReceiveBufferSize, sizeof(socketReceiveBufferSize)));
		//}

		//{
		//	socketSendBufferSize = 0;
		//	CHECK_SOCKET_ERROR(setsockopt(CS.Socket, SOL_SOCKET, SO_SNDBUF, (char*)&socketSendBufferSize, sizeof(socketSendBufferSize)));
		//}

		//{
		//	nonblocked = 1;
		//	CHECK_SOCKET_ERROR(ioctlsocket(CS.Socket, FIONBIO, &nonblocked));
		//}

		CS.Address.sin_family = AF_INET;
		//CS.Address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		InetPton(AF_INET, IP_Address.c_str(), &CS.Address.sin_addr); //inet_addr(INADDR_ANY); //InetNtop();
		//WSAAddressToString();
		//WSAStringToAddress();
		CS.Address.sin_port = htons(Port);

		//CHECK_SOCKET_ERROR(WSAConnect(CS.Socket, (SOCKADDR*)&CS.Address, sizeof(CS.Address), nullptr, nullptr, nullptr, nullptr));
		//CHECK_SOCKET_ERROR(connect(CS.Socket, (SOCKADDR*)&CS.Address, sizeof(CS.Address)));
	}

	{
		timeBeginPeriod(1);

		TIMER.Initialize();

		srand(time(nullptr));
		//srand(2705);

		GetSystemInfo(&systemInformation);
	}

	{
		if (SUB_THREAD_V.size() - IOCP_CLIENT_SUB_THREAD_COUNT);
		else SUB_THREAD_V.resize(IOCP_CLIENT_SUB_THREAD_COUNT + systemInformation.dwNumberOfProcessors * 2);

		COMMON_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, SUB_THREAD_V.size() - IOCP_CLIENT_SUB_THREAD_COUNT);
		CONNECT_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);
		//RECEIVE_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);
		//UPDATE_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);
		//SEND_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);
		DISCONNECT_CP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 1);

		subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, ConnectServer, this, 0, &subThread.IdentificationNumber);
		SUB_THREAD_V[IOCP_CLIENT_SUB_THREAD_CONNECT] = subThread;

		//subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, ReceivePacket, this, 0, &subThread.IdentificationNumber);
		//SUB_THREAD_V[IOCP_CLIENT_SUB_THREAD_RECEIVE] = subThread;
		//
		//subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, UpdateClient, this, 0, &subThread.IdentificationNumber);
		//SUB_THREAD_V[IOCP_CLIENT_SUB_THREAD_UPDATE] = subThread;
		//
		//subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, SendPacket, this, 0, &subThread.IdentificationNumber);
		//SUB_THREAD_V[IOCP_CLIENT_SUB_THREAD_SEND] = subThread;

		subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, DisconnectServer, this, 0, &subThread.IdentificationNumber);
		SUB_THREAD_V[IOCP_CLIENT_SUB_THREAD_DISCONNECT] = subThread;

		FOR(i, SUB_THREAD_V.size() - IOCP_CLIENT_SUB_THREAD_COUNT)
		{
			subThread.Handle = (HANDLE)_beginthreadex(nullptr, 0, WorkClient, this, 0, &subThread.IdentificationNumber);
			SUB_THREAD_V[IOCP_CLIENT_SUB_THREAD_COUNT + i] = subThread;
		}
	}

	return 0;
}

bool IOCP_Client::Terminalize()
{
	HANDLE_V subThreadHandleV;

	subThreadHandleV.resize(SUB_THREAD_V.size());

	{
		closesocket(CS.Socket); //close connect thread and induct disconnection

		//shutdown(CS.Socket, SD_BOTH); //induct disconnection

		while (CS.CurrentPhase != IOCP_CLIENT_PHASE_CONNECT)
		{
			Sleep(0);
		}

		SUB_THREAD_ON = 0;

		PostQueuedCompletionStatus(CONNECT_CP, NULL, NULL, nullptr);
		PostQueuedCompletionStatus(DISCONNECT_CP, NULL, NULL, nullptr); //close disconnect thread

		FOR(i, SUB_THREAD_V.size() - IOCP_CLIENT_SUB_THREAD_COUNT)
		{
			PostQueuedCompletionStatus(COMMON_CP, NULL, NULL, nullptr);
		}

		FOR(i, subThreadHandleV.size())
		{
			subThreadHandleV[i] = SUB_THREAD_V[i].Handle;
		}

		WaitForMultipleObjects(SUB_THREAD_V.size(), &subThreadHandleV[0], 1, INFINITE); //confirm thread closed

		CloseHandle(COMMON_CP);
		//CloseHandle(CONNECT_CP);
		//CloseHandle(RECEIVE_CP);
		//CloseHandle(UPDATE_CP);
		CloseHandle(DISCONNECT_CP);

		FOR(i, SUB_THREAD_V.size())
		{
			CloseHandle(SUB_THREAD_V[i].Handle);
		}

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