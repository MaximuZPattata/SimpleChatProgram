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

	// Initialize the Windows Sockets API (WSA) for network communication
	result = networkManager.InitializeWSA();

	if (result != 0)
		return 1;

	// Initialize address information for server socket.
	result = networkManager.InitializeAddrInfo(NULL, DEFAULT_PORT);

	if (result != 0)
		return 1;

	//Socket creation.
	SOCKET serverSocket;

	result = networkManager.CreateSocket(serverSocket);

	if (result != 0)
		return 1;

	// Bind the server socket.
	result = networkManager.Bind(serverSocket);

	if (result != 0)
		return 1;

	// Listening for incoming connections on the server socket.
	result = networkManager.Listen(serverSocket);

	if (result != 0)
		return 1;

	// Initialize sets.
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

		// Add all connected clients to the sets.
		networkManager.AddAllClientsToFDSET(socketsReadyForReading);

		networkManager.readCount = select(0, &socketsReadyForReading, NULL, NULL, &tv); // Use the `select` function to check for sockets ready for reading.

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

		// Loop through the client list to process for messages.
		result = networkManager.LoopThroughClientList(socketsReadyForReading, activeSockets, message, buffer, bufSize);

		if (result != 0)
			break;

		if (networkManager.readCount > 0)
		{	
			if (FD_ISSET(serverSocket, &socketsReadyForReading))
			{	
				result = networkManager.AddNewClientToList(serverSocket, socketsReadyForReading, activeSockets); // Adding new client to the list.

				if (result != 0)
					break;
			}
		}
	}

	// Clean up and close the server socket.
	networkManager.CleanSocket(serverSocket, networkManager.info);
	return 0;
}
