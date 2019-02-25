#pragma once

template<typename PacketHeaderType>
class SerialPacketQueue
{
private:
	unsigned int HeaderSize;
	unsigned int BodySize;

	unsigned int FrontIndex;
	unsigned int CurrentSize;
	unsigned int MaximumSize;

	unsigned int ReferenceCount;

	char ByteA[SERIAL_PACKET_QUEUE_SIZE]; //Queue
	//vector<char> ByteV;

public:
	SerialPacketQueue() : HeaderSize(0), BodySize(0), FrontIndex(0), CurrentSize(0), MaximumSize(0), ReferenceCount(0)
	{
		HeaderSize = sizeof(PacketHeaderType);
		BodySize = 0;

		FrontIndex = HeaderSize;
		CurrentSize = 0;
		MaximumSize = sizeof(ByteA) - HeaderSize;

		memset(ByteA, 0, sizeof(ByteA));

		++ReferenceCount;
	}
	~SerialPacketQueue()
	{

	}

	bool Initialize()
	{
		HeaderSize = sizeof(PacketHeaderType);
		BodySize = 0;

		FrontIndex = HeaderSize;
		CurrentSize = 0;
		MaximumSize = sizeof(ByteA) - HeaderSize;
		ReferenceCount = 0;

		//memset(ByteA, 0, sizeof(ByteA));

		++ReferenceCount;

		return 0;
	}

	//bool Update();
	//bool Render();

	bool Terminalize()
	{
		return 0;
	}

	template<typename DatumType>
	unsigned int Enqueue(DatumType datum)
	{
		unsigned int datumSize;

		datumSize = sizeof(datum);

		if (datumSize <= MaximumSize - CurrentSize)
		//if (datumSize <= MaximumSize - BodySize)
		{
			*(DatumType*)&(ByteA[FrontIndex + CurrentSize]) = datum;

			BodySize += datumSize;

			CurrentSize += datumSize;

			return datumSize;
		}
		else return 0;
	}

	template<typename DatumType>
	unsigned int Dequeue(DatumType& datum)
	{
		unsigned int datumSize;

		datumSize = sizeof(datum);

		if (datumSize <= CurrentSize)
		//if (datumSize <= BodySize)
		{
			datum = *(DatumType*)&(ByteA[FrontIndex]);

			BodySize -= datumSize;

			FrontIndex += datumSize;
			CurrentSize -= datumSize;

			return datumSize;
		}
		else return 0;
	}

	/*
	unsigned int Encrypt()
	{
		return 0;
	}
	
	unsigned int Decrypt()
	{
		return 0;
	}

	unsigned int RSA_Encrypt()
	{
		return 0;
	}
	
	unsigned int RSA_Decrypt()
	{
		return 0;
	}
	*/

	unsigned int UnpackPacket()
	{
		PacketHeaderType header;
		CHAR* frontAddress;
		BYTE checksum;

		UINT result;

		header = GetHeader();
		frontAddress = GetFrontAddress();
		checksum = 0;

		if (header.IdentificationCode == 0x77)
		{
			if (header.BodySize == CurrentSize)
			{
				header.Checksum = header.Checksum ^ header.CryptoCode;

				for (unsigned int i = 0; i < header.BodySize; ++i)
				{
					frontAddress[i] = frontAddress[i] ^ header.CryptoCode;

					checksum += frontAddress[i];
				}

				//for (unsigned int i = 0; i < header.BodySize; ++i)
				//{
				//	checksum += frontAddress[i];
				//}

				if (header.Checksum == checksum) result = header.BodySize;
				else result = 0;
			}
			else result = 0;
		} 
		else result = 0;

		return result;
	}

	unsigned int PackPacket()
	{
		PacketHeaderType header;
		CHAR* frontAddress;
		BYTE checksum;

		UINT result;

		header.IdentificationCode = 0x77;
		header.BodySize = BodySize;
		header.CryptoCode = 0x27;
		header.Checksum = 0;

		frontAddress = GetFrontAddress();

		for (unsigned int i = 0; i < header.BodySize; ++i)
		{
			header.Checksum += frontAddress[i];

			frontAddress[i] = frontAddress[i] ^ header.CryptoCode;
		}

		header.Checksum = header.Checksum ^ header.CryptoCode;

		//for (unsigned int i = 0; i < header.BodySize; ++i)
		//{
		//	frontAddress[i] = frontAddress[i] ^ header.CryptoCode;
		//}

		SetHeader(header);

		result = sizeof(header) + header.BodySize;

		return result;
	}

	/*
	unsigned int XorEncrypt(char xorCode)
	{
		unsigned int frontIndex;
		unsigned int currentSize;

		frontIndex = FrontIndex;
		currentSize = CurrentSize;

		for (unsigned int i = 0; i < currentSize; ++i)
		{
			ByteA[frontIndex + i] = ByteA[frontIndex + i] ^ xorCode;
		}

		return xorCode;
	}

	unsigned int XorDecrypt(char xorCode)
	{
		unsigned int frontIndex;
		unsigned int currentSize;

		frontIndex = FrontIndex;
		currentSize = CurrentSize;

		for (unsigned int i = 0; i < currentSize; ++i)
		{
			ByteA[frontIndex + i] = ByteA[frontIndex + i] ^ xorCode;
		}

		return xorCode;
	}

	unsigned int CalculateChecksum()
	{
		unsigned char checksum;
		unsigned int frontIndex;
		unsigned int currentSize;

		checksum = 0;
		frontIndex = FrontIndex;
		currentSize = CurrentSize;

		for (unsigned int i = 0; i < currentSize; ++i)
		{
			checksum += ByteA[frontIndex + i];
		}

		return checksum;
	}
	*/

	template<typename DatumType>
	unsigned int operator<<(DatumType datum)
	{
		unsigned int datumSize;

		datumSize = sizeof(datum);

		if (datumSize <= MaximumSize - CurrentSize)
		//if (datumSize <= MaximumSize - BodySize)
		{
			*(DatumType*)&(ByteA[FrontIndex + CurrentSize]) = datum;

			BodySize += datumSize;

			CurrentSize += datumSize;

			return datumSize;
		}
		else return 0;
	}

	template<typename DatumType>
	unsigned int operator>>(DatumType& datum)
	{
		unsigned int datumSize;

		datumSize = sizeof(datum);

		if (datumSize <= CurrentSize)
		//if (datumSize <= BodySize)
		{
			datum = *(DatumType*)&(ByteA[FrontIndex]);

			BodySize -= datumSize;

			FrontIndex += datumSize;
			CurrentSize -= datumSize;

			return datumSize;
		}
		else return 0;
	}



	unsigned int IncreaseBodySize(unsigned int increasingSize) { BodySize += increasingSize;  CurrentSize += increasingSize; return BodySize; }
	unsigned int IncreaseCurrentSize(unsigned int increasingSize) { CurrentSize += increasingSize; BodySize += increasingSize; return CurrentSize; }
	unsigned int IncreaseReferenceCount() { return InterlockedIncrement(&ReferenceCount); }

	unsigned int DecreaseBodySize(unsigned int decreasingSize) { BodySize -= decreasingSize; CurrentSize -= decreasingSize; }
	unsigned int DecreaseCurrentSize(unsigned int decreasingSize) { CurrentSize -= decreasingSize; BodySize -= decreasingSize; }
	unsigned int DecreaseReferenceCount() { return InterlockedDecrement(&ReferenceCount); }



	unsigned int GetHeaderSize() { return HeaderSize; }
	unsigned int GetBodySize() { return BodySize; }

	unsigned int GetFrontIndex() { return FrontIndex; }
	unsigned int GetCurrentSize() { return CurrentSize; }
	//unsigned int GetNextIndex() { return FrontIndex + CurrentSize; } //RearIndex + 1 //deprecated
	unsigned int GetMaximumSize() { return MaximumSize; }

	unsigned int GetTotalSize() { return HeaderSize + CurrentSize; }

	unsigned int GetReferenceCount() { return ReferenceCount; }

	char* GetHeaderAddress() { return &ByteA[0]; }
	char* GetBodyAddress() { return &ByteA[HeaderSize]; }

	char* GetQueueAddress() { return &ByteA[0]; }
	char* GetFrontAddress() { return &ByteA[FrontIndex]; }
	//char* GetNextAddress() { return &ByteA[FrontIndex + CurrentSize]; } //deprecated

	PacketHeaderType GetHeader()
	{
		PacketHeaderType header;

		header = *(PacketHeaderType*)&ByteA[0];

		return header;
	}



	void SetHeader(PacketHeaderType header)
	{
		unsigned int headerSize;

		headerSize = sizeof(header);

		HeaderSize = headerSize;
		FrontIndex = headerSize;
		MaximumSize = sizeof(ByteA) - headerSize;

		*(PacketHeaderType*)&ByteA[0] = header;
	}
};

