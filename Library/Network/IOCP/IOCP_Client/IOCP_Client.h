#pragma once
#include "..\IOCP.h"

class IOCP_Client
{
private:
	IOCP_ClientSession CS;
	IOCP_SubThreadV SUB_THREAD_V;

	BOOL SUB_THREAD_ON;
	UINT CURRENT_CONNECTION_COUNT;

	HANDLE COMMON_CP;
	HANDLE CONNECT_CP;
	//HANDLE RECEIVE_CP;
	//HANDLE UPDATE_CP;
	//HANDLE SEND_CP;
	HANDLE DISCONNECT_CP;

	IOCP_Client* SelfInstanceAddress;

	wstring IP_Address;
	UINT Port;
	//UINT MaximumClientCount;
	UINT WorkThreadCount;

protected:
	ThreadLocalPool<SerialPacketQueue<IOCP_PacketHeader>> PACKET_SPQ_TLP;

	UINT CURRENT_SPQ_COUNT;
	UINT ALLOCATED_SPQ_COUNT;
	UINT DEALLOCATED_SPQ_COUNT;

	BOOL THREAD_CONTEXT_SWITCH_ON;
	BOOL INDICATOR_ON;

public:
	IOCP_Client();
	virtual ~IOCP_Client();

	bool Initialize();
	//virtual bool Update() = 0;
	//viurtual bool Render() = 0;
	bool Terminalize();



	static unsigned int WINAPI ConnectServer(LPVOID selfInstanceAddress);
	static unsigned int WINAPI DisconnectServer(LPVOID selfInstanceAddress);
	static unsigned int WINAPI WorkClient(LPVOID selfInstanceAddress);

	unsigned int ConnectServer();
	unsigned int DisconnectServer();
	unsigned int WorkClient();



	unsigned int ReceivePacket(UINT64 sessionKey, SerialPacketQueue<IOCP_PacketHeader>* receivedPacketSPQ_Address);
	unsigned int SendPacket(UINT64 sessionKey, SerialPacketQueue<IOCP_PacketHeader>* sendingPacketSPQ_Address);

	bool Disconnect(UINT64 sessionKey);

	//unsigned int PostAccept(UINT64 sessionKey); //deprecated
	//unsigned int PostReceive(UINT64 sessionKey); //deprecated
	unsigned int PostSend(UINT64 sessionKey);
	unsigned int PostDisconnect(UINT64 sessionKey);

	bool Start(LPCWSTR ipAddress = L"127.0.0.1", UINT port = 27050, UINT workThreadCount = 4, BOOL nagleOff = 1, UINT maximumClientCount = 16384);
	void Stop();



	virtual void OnClientJoin(SOCKADDR_IN clientAddress, UINT64 sessionKey) = 0;
	virtual void OnClientLeave(UINT64 sessionKey) = 0;
	virtual bool OnConnectionRequest(SOCKADDR_IN clientAddress, UINT port) = 0;

	virtual void OnRecv(UINT64 sessionKey, UINT receivedSize) = 0;
	virtual void OnSend(UINT64 sessionKey, UINT sendedSize) = 0;
	virtual void OnWorkerThreadBegin() = 0;
	virtual void OnWorkerThreadEnd() = 0;

	virtual void OnError(UINT errorNumber, LPCWSTR errorText) = 0;
};