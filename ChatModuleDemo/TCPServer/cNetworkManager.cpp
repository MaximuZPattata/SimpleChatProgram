#include "pch.h"
#include "cNetworkManager.h"

void cNetworkManager::SendMessageToBuffer(sChatMessage& message, cBuffer& buffer, std::string input)
{
	message.message = input;
	message.messageLength = message.message.length();
	message.header.messageType = 1;
	message.header.packetSize =
		message.message.length()
		+ sizeof(message.messageLength)
		+ sizeof(message.header.messageType)
		+ sizeof(message.header.packetSize);

	// Write our packet to the buffer
	buffer.WriteUInt32BE(message.header.packetSize);
	buffer.WriteUInt32BE(message.header.messageType);
	buffer.WriteUInt32BE(message.messageLength);
	buffer.WriteString(message.message);
}

void cNetworkManager::CleanSocket(SOCKET& serverSocket, PADDRINFOA info)
{
	freeaddrinfo(info);
	closesocket(serverSocket);
	WSACleanup();
}

int cNetworkManager::InitializeWSA()
{
	WSADATA wsaData;
	int result;

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		printf("WSA Startup ---> FAILED | ERROR : %d\n", result);
		return 1;
	}

	printf("WSA Startup ---> SUCCESS \n");
	return 0;
}

int cNetworkManager::InitializeAddrInfo(const char* ipaddress, const char* port)
{
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int result = getaddrinfo(ipaddress, port, &hints, &info);

	if (result != 0)
	{
		printf("getaddrinfo ---> FAILED | ERROR : %d\n", result);
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}
	printf("getaddrinfo ---> SUCCESS \n");
	return 0;
}

int cNetworkManager::CreateSocket(SOCKET& serverSocket)
{
	serverSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

	if (serverSocket == INVALID_SOCKET)
	{
		printf("Socket Creation ---> FAILED | ERROR : %d\n", WSAGetLastError());
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}
	printf("Socket Creation ---> SUCCESS \n");
	return 0;

	/*unsigned long NonBlock = 1;
	result = ioctlsocket(serverSocket, FIONBIO, &NonBlock);*/
}

int cNetworkManager::Bind(SOCKET& serverSocket)
{
	int result = bind(serverSocket, info->ai_addr, (int)info->ai_addrlen);

	if (result == SOCKET_ERROR)
	{
		printf("Binding ---> FAILED | ERROR : %d\n", WSAGetLastError());
		CleanSocket(serverSocket, info);
		return 1;
	}
	printf("Binding ---> SUCCESS \n");
	return 0;
}

int cNetworkManager::Listen(SOCKET& serverSocket)
{
	int result = listen(serverSocket, SOMAXCONN);

	if (result == SOCKET_ERROR)
	{
		printf("Listening ---> FAILED | ERROR : %d\n", WSAGetLastError());
		CleanSocket(serverSocket, info);
		return 1;
	}
	printf("Listening ---> SUCCESS \n");
	printf("--------------------------------------------------------------------------------------------------\n");
	return 0;
}

int cNetworkManager::SendMessageToClient(sChatMessage& message, cBuffer& buffer, std::string fullMessage, SOCKET clientSocket)
{
	SendMessageToBuffer(message, buffer, fullMessage);

	int result = send(clientSocket, (const char*)(&buffer.mBufferData[0]), message.header.packetSize, 0);

	if (result == SOCKET_ERROR)
	{
		printf("\nMessage sending ---> FAILED | ERROR : %d\n", WSAGetLastError());
		CleanSocket(clientSocket, info);
		return 1;
	}

	return 0;
}

int cNetworkManager::ReceiveMessageFromClient(SOCKET clientSocket, cBuffer& buffer, int bufSize)
{
	int result = recv(clientSocket, (char*)(&buffer.mBufferData[0]), bufSize, 0);

	if (result == SOCKET_ERROR)
	{
		printf("\nMessage Receiving ---> FAILED | ERROR : %d\n", WSAGetLastError());
		CleanSocket(clientSocket, info);
		return 1;
	}

	return 0;
}

void cNetworkManager::AddAllClientsToFDSET(FD_SET& socketsReadyForReading)
{
	for (int i = 0; i < mClientConnectionList.size(); i++)
	{
		FD_SET(mClientConnectionList[i], &socketsReadyForReading);
	}
}

int cNetworkManager::AddNewClientToList(SOCKET& clientSocket, FD_SET& socketsReadyForReading, FD_SET& activeSockets)
{
	SOCKET newConnection = accept(clientSocket, NULL, NULL);
	if (newConnection == INVALID_SOCKET)
	{
		printf("\nAccepting new Client ---> FAILED | ERROR : %d\n", WSAGetLastError());
		return 1;
	}

	mClientConnectionList.push_back(newConnection);
	FD_SET(newConnection, &activeSockets);
	FD_CLR(clientSocket, &socketsReadyForReading);

	printf("\nAdding new Client ---> SUCCESS | SOCKET : %d\n", (int)newConnection);
	return 0;
}

void cNetworkManager::RemoveClientFromList(SOCKET& clientSocket)
{
	mClientConnectionList.erase(std::remove(mClientConnectionList.begin(), mClientConnectionList.end(), clientSocket), mClientConnectionList.end());
}

int cNetworkManager::LoopThroughClientList(FD_SET& socketsReadyForReading, FD_SET& activeSockets, sChatMessage& message, cBuffer& buffer, int bufSize)
{
	int result = 0;
	std::string roomName;
	std::string fullMessage;

	for (int i = 0; i < mClientConnectionList.size(); i++)
	{
		SOCKET socket = mClientConnectionList[i];

		if (FD_ISSET(socket, &socketsReadyForReading))
		{
			result = ReceiveMessageFromClient(socket, buffer, bufSize);

			if (result != 0)
			{
				RemoveClientFromList(socket);
				FD_CLR(socket, &activeSockets);
				FD_CLR(socket, &socketsReadyForReading);
				CleanSocket(socket, info);
				return 1;
			}

			uint32_t packetSize = buffer.ReadUInt32BE();
			uint32_t messageType = buffer.ReadUInt32BE();

			if (buffer.mBufferData.size() >= packetSize)
			{
				buffer.mBufferData.resize(bufSize * 2);
			}

			if (messageType == 1)
			{
				uint32_t messageLength = buffer.ReadUInt32BE();
				std::string msg = buffer.ReadString(messageLength);
				std::string tempString;

				tempString = msg.substr(0, 6);

				if (tempString == "$NAME$")
				{
					printf("\n[%s] IS CONNECTED TO THE SERVER\n", msg.substr(6).c_str());

					clientName[socket] = msg.substr(6);
				}

				else if (tempString == "$JOIN$")
				{
					roomName = msg.substr(6);

					// Check if the room exists, if not, create it
					if (rooms.find(roomName) == rooms.end())
					{
						sRoom room;
						rooms[roomName] = room;

						printf("\nNEW ROOM CREATED | ROOM NAME : %s\n", roomName.c_str());
					}

					// Add the client to the room
					rooms[roomName].clients.push_back(socket);
					clientRooms[socket] = roomName;

					printf("\n[%s] HAS JOINED THE ROOM (%s)\n", clientName[socket].c_str(), roomName.c_str());

					fullMessage = "$READY$[" + clientName[socket] + "] IS ADDED TO THE ROOM (" + roomName + ")";

					result = SendMessageToClient(message, buffer, fullMessage, socket);

					if (result != 0)
						return 1;
					else
					{
						buffer.ClearBuffer();

						result = BroadcastToAll(socket, true, buffer, message, roomName);

						if (result != 0)
							return 1;
					}

				}

				else if (tempString == "$CHAT$")
				{
					std::string username = clientName[socket];
					std::string userRoom = clientRooms[socket];

					sRoom& room = rooms[userRoom];

					for (SOCKET clientSocket : room.clients)
					{
						if (clientSocket != socket)
						{
							fullMessage = "$REPLY$" + username + " : " + msg.substr(6);
							result = SendMessageToClient(message, buffer, fullMessage, clientSocket);

							if (result != 0)
								return 1;

							buffer.ClearBuffer();
						}
						else
						{
							fullMessage = "YOU : " + msg.substr(6);
							result = SendMessageToClient(message, buffer, fullMessage, clientSocket);

							if (result != 0)
								return 1;

							buffer.ClearBuffer();
						}
					}
				}

				else if (tempString == "$EXIT$")
				{
					// Client disconnected
					roomName = clientRooms[socket];
					rooms[roomName].clients.erase(std::remove(rooms[roomName].clients.begin(), rooms[roomName].clients.end(), socket), rooms[roomName].clients.end());
					clientRooms.erase(socket);

					fullMessage = "$LEFT$";

					result = SendMessageToClient(message, buffer, fullMessage, socket);

					if (result != 0)
						return 1;
					else
						printf("\n[%s] HAS LEFT THE ROOM (%s)\n", clientName[socket].c_str(), roomName.c_str());

					buffer.ClearBuffer();

					result = BroadcastToAll(socket, false, buffer, message, roomName);

					if (result != 0)
						return 1;
				}

			}
			buffer.ClearBuffer();

			FD_CLR(socket, &socketsReadyForReading);
			readCount--;
		}
	}

	return 0;
}

int cNetworkManager::BroadcastToAll(SOCKET& clientSocket, bool newClientJoined, cBuffer& buffer, sChatMessage message, std::string userRoom)
{
	int result = 0;
	std::string fullMessage;

	if (newClientJoined)
	{
		std::string username = clientName[clientSocket];
		sRoom& room = rooms[userRoom];

		for (SOCKET otherClientSocket : room.clients)
		{
			if (otherClientSocket != clientSocket)
			{
				fullMessage = "$REPLY$**[" + username + "] HAS JOINED THE ROOM**";
				result = SendMessageToClient(message, buffer, fullMessage, otherClientSocket);

				if (result != 0)
					return 1;

				buffer.ClearBuffer();
			}

		}

	}
	else
	{
		std::string username = clientName[clientSocket];
		sRoom& room = rooms[userRoom];

		for (SOCKET otherClientSocket : room.clients)
		{
			fullMessage = "$REPLY$**[" + username + "] HAS LEFT THE ROOM**";
			result = SendMessageToClient(message, buffer, fullMessage, otherClientSocket);

			if (result != 0)
				return 1;

			buffer.ClearBuffer();
		}

	}

	return 0;
}