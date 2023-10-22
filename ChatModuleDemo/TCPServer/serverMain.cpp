#pragma once

#include "pch.h"
#include "cBuffer.h"
#include "cNetworkManager.h"

#define DEFAULT_PORT "8400"

int main(int arg, char** argv)
{
	const int bufSize = 512;
	int result = 0;

	cNetworkManager networkManager;
	sChatMessage message;
	cBuffer buffer(bufSize);

	result = networkManager.InitializeWSA();

	if (result != 0)
		return 1;

	result = networkManager.InitializeAddrInfo(NULL, DEFAULT_PORT);

	if (result != 0)
		return 1;

	SOCKET serverSocket;

	result = networkManager.CreateSocket(serverSocket);

	if (result != 0)
		return 1;

	result = networkManager.Bind(serverSocket);

	if (result != 0)
		return 1;

	result = networkManager.Listen(serverSocket);

	if (result != 0)
		return 1;

	FD_SET activeSockets;
	FD_SET socketsReadyForReading;

	FD_ZERO(&activeSockets);

	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while (true)
	{
		FD_ZERO(&socketsReadyForReading);

		FD_SET(serverSocket, &socketsReadyForReading);

		networkManager.AddAllClientsToFDSET(socketsReadyForReading);

		networkManager.readCount = select(0, &socketsReadyForReading, NULL, NULL, &tv);

		if (networkManager.readCount == 0)
		{
			continue;
		}

		if (networkManager.readCount == SOCKET_ERROR)
		{
			printf("Select() ---> FAILED | ERROR : %d\n", WSAGetLastError());
			closesocket(serverSocket);
			return 1;
		}

		result = networkManager.LoopThroughClientList(socketsReadyForReading, activeSockets, message, buffer, bufSize);

		if (result != 0)
			break;

		if (networkManager.readCount > 0)
		{
			if (FD_ISSET(serverSocket, &socketsReadyForReading))
			{
				result = networkManager.AddNewClientToList(serverSocket, socketsReadyForReading, activeSockets);

				if (result != 0)
					break;
			}
		}
	}

	networkManager.CleanSocket(serverSocket, networkManager.info);
	return 0;
}
