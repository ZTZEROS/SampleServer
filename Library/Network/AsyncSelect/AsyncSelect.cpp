#include "FirstOrderLibrary.h"

/*
LRESULT CALLBACK NetworkProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PacketHeader packetHeader;
	PacketTrailer packetTrailer;
	int returnValue;

	char receiveBuffer[BUFFER_SIZE];
	char sendBuffer[BUFFER_SIZE];

	if (WSAGETSELECTERROR(lParam))
	{
		closesocket(wParam);

		return 0;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
		{
			returnValue = 0;

			if (CLIENT.Socket == INVALID_SOCKET) return 0;
			else
			{
				returnValue = WSAAsyncSelect(wParam, hWnd, WM_NETWORK, FD_READ | FD_WRITE | FD_CLOSE);
				if (returnValue == SOCKET_ERROR)
				{
					CLIENT.Connected = 0;
					closesocket(CLIENT.Socket);
				}
				else CLIENT.Connected = 1;
			}
		}
		break;
	case FD_READ:
		{
			returnValue = 0;

			while (1)
			{
				returnValue = recv(wParam, receiveBuffer, sizeof(packetHeader), 0);
				if (returnValue == SOCKET_ERROR || returnValue == 0)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						CLIENT.Connected = 0;
						closesocket(wParam);
					}

					break;
				}
				else
				{
					CLIENT.ReceiveQ.Enqueue(receiveBuffer, returnValue);

					returnValue = recv(wParam, receiveBuffer, packetHeader.PacketBodySize + 1, 0);
					if (returnValue == SOCKET_ERROR || returnValue == 0)
					{
						if (WSAGetLastError() != WSAEWOULDBLOCK)
						{
							CLIENT.Connected = 0;
							closesocket(wParam);
						}

						break;
					}
					else CLIENT.ReceiveQ.Enqueue(receiveBuffer, returnValue);
				}
			}
		}
		break;
	case FD_WRITE:
		{
			CLIENT.Sendable = 1;

			returnValue = 0;

			while (CLIENT.Sendable)
			{
				returnValue = CLIENT.SendQ.Peek((char*)&packetHeader, sizeof(packetHeader));
				if (0 < returnValue)
				{
					if (packetHeader.IdentificationCode == FIGHTER_PACKET_HEADER_IDENTIFICATION_CODE)
					{
						returnValue = CLIENT.SendQ.Peek(sendBuffer, sizeof(packetHeader) + packetHeader.PacketBodySize + sizeof(packetTrailer));
						if (0 < returnValue)
						{
							returnValue = send(CLIENT.Socket, sendBuffer, returnValue, 0);
							if (returnValue == SOCKET_ERROR)
							{
								if (WSAGetLastError() == WSAEWOULDBLOCK) CLIENT.Sendable = 0;
								else
								{
									CLIENT.Sendable = 0;
									CLIENT.Connected = 0;
									closesocket(CLIENT.Socket);
								}

								break;
							}
							else CLIENT.SendQ.Dequeue(sendBuffer, returnValue);
						}
						else break;

					}
					else
					{
						returnValue *= -1;

						break;
					}
				}
				else break;
			}
		}
		break;
	case FD_CLOSE:
		closesocket(wParam);
		break;
	}

	return 0;
}
//*/