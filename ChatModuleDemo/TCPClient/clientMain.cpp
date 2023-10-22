#pragma once 

#include "pch.h"
#include "cBuffer.h"
#include "cNetworkManager.h"
#include "cUserInterface.h"

#define DEFAULT_PORT "8400"

int main(int arg, char** argv)
{
	int result = 0;
	const int bufSize = 512;

	//Other class instances
	cNetworkManager networkManager;
	cUserInterface userInterface;
	sChatMessage message;

	cBuffer buffer(bufSize);

	// Initialize the Windows Sockets API (WSA).
	result = networkManager.InitializeWSA();

	if (result != 0)
		return 1;

	// Initialize address information.
	result = networkManager.InitializeAddrInfo("127.0.0.1", DEFAULT_PORT);

	if (result != 0)
		return 1;

	SOCKET serverSocket;

	// Create a socket for the server connection.
	result = networkManager.CreateSocket(serverSocket);

	if (result != 0)
		return 1;

	// Connect to the server.
	result = networkManager.ConnectSocket(serverSocket);

	if (result != 0)
		return 1;

	//FD_SET for socket operations				
	FD_SET socketsReadyForReading;

	// timeval to prevent select from waiting forever.
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	// Asking for client name
	result = userInterface.GetClientName(networkManager, serverSocket, buffer, message);

	if (result != 0)
		return 1;

	while (true)
	{
		FD_ZERO(&socketsReadyForReading);

		FD_SET(serverSocket, &socketsReadyForReading);

		if (!userInterface.isUserWriting())
		{
			if (_kbhit())
			{
				int key = _getch();

				// Process user input (e.g., pressing 'ESC' or 'TAB')
				result = userInterface.CheckUserInput(networkManager, key, serverSocket, buffer, message);

				if (result != 0)
					return 1;
			}

			buffer.ReinitializeIndex();
		}
		// Check for responses from the server using select()
		result = networkManager.CheckForResponseFromServer(serverSocket, buffer, bufSize, socketsReadyForReading, tv);

		if (result == 2)
			continue;
		else if (result == 0)
		{
			// Process incoming messages from server.
			uint32_t packetSize = buffer.ReadUInt32BE();
			uint32_t messageType = buffer.ReadUInt32BE();

			if (buffer.mBufferData.size() >= packetSize)
			{
				buffer.mBufferData.resize(bufSize * 2);
			}

			if (messageType == 1)
			{
				userInterface.PrintReceivedMessage(buffer); // Print received messages to the user interface.

				buffer.ReinitializeIndex();
			}
		}
		else
			break;

		// Reset the buffer
		buffer.ReinitializeIndex();

		buffer.mBufferData.clear();
		buffer.mBufferData.resize(512);
	}

	// Clean up and close the connection.
	networkManager.CleanSocket(serverSocket, networkManager.info);

	return 0;
}
