#pragma once

struct CommonVector
{
	int X;
	int Y;
	int Z;
	int W;
};

struct CommonVelocity : public CommonVector
{

};

struct CommonPosition
{
	unsigned int X;
	unsigned int Y;
	unsigned int Z;
};

struct CommonSession
{
	SOCKET Socket;
	SOCKADDR_IN Address;
	volatile unsigned long long Key;
	volatile unsigned int Index;

	volatile unsigned int Connected;
	volatile unsigned int Receivable;
	volatile unsigned int Sendable;

	volatile unsigned int ReceiveCount;
	volatile unsigned int SendCount;
};

struct ServerSession : public CommonSession
{
	
};

struct ClientSession : public CommonSession
{
	
};

struct CommonSubThread
{
	void* Handle;
	unsigned int IdentificationNumber; //SubThreadKey
};



#pragma pack(push, 1) //#pragma pack(2) //#pragma pack(push, 2) 

struct CommonPacketHeader //PacketHead
{
	unsigned char IdentificationCode;
	unsigned char Checksum;
	unsigned char PacketType;
	unsigned char PacketBodySize;
	unsigned char TemporaryByte;
};

struct CommonPacketBody
{

};

struct CommonPacketTrailer //PacketTail
{
	unsigned char IdentificationCode;
};

struct CommonPacket
{
	CommonPacketHeader Header;
	CommonPacketBody Body;
	CommonPacketTrailer Trailer;
};

#pragma pack(pop) //#pragma pack(4)

#pragma pack(push, 1)

struct CommonQueryPacketHeader
{
	QUERY_INDEX Index;
	unsigned short int Size;
};

struct CommonQueryPacketBody
{

};

struct CommonQueryPacketTrailer //QueryPacketTail
{

};

struct CommonQueryPacket
{
	CommonQueryPacketHeader Header;
	//CommonQueryPacketBody Body;
	CommonQueryPacketTrailer Trailer;
};

#pragma pack(pop)

struct CommonWorkHistory //LifeCycle
{
	unsigned int ThreadKey; //ThreadId
	unsigned int WorkIndex; //RecentWork;
};