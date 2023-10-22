#include "pch.h"
#include "cNetworkManager.h"
#include "cBuffer.h"

cNetworkManager::cNetworkManager()
{

}

cNetworkManager::~cNetworkManager()
{

}

void cNetworkManager::SendMessageToBuffer(sChatMessage& message, cBuffer& buffer, std::string input)
{
	message.message = input;
	message.messageLength = message.message.length();
	message.header.messageType = 1;// Can use an enum to determine this
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

int cNetworkManager::WriteResponse(sChatMessage& message, cBuffer& buffer, std::string response, SOCKET& serverSocket)
{
	SendMessageToBuffer(message, buffer, response);

	int sendResult = send(serverSocket, (const char*)(&buffer.mBufferData[0]), message.header.packetSize, 0);

	if (sendResult == SOCKET_ERROR)
	{
		printf("Message sending ---> FAILED | ERROR : %d\n", WSAGetLastError());
		CleanSocket(serverSocket, info);
		return sendResult;
	}

	return 0;
}

void cNetworkManager::CleanSocket(SOCKET& serverSocket, PADDRINFOA info)
{
	closesocket(serverSocket);
	freeaddrinfo(info);
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

int cNetworkManager::ConnectSocket(SOCKET& serverSocket)
{
	connect(serverSocket, info->ai_addr, (int)info->ai_addrlen);

	if (serverSocket == INVALID_SOCKET)
	{
		printf("Connection ---> FAILED | ERROR : %d\n", WSAGetLastError());
		CleanSocket(serverSocket, info);
		return 1;
	}
	printf("Connection ---> SUCCESS \n");
	printf("---------------------------------------------------------------------------------------------\n");
	return 0;
}

int cNetworkManager::ReceiveMessageFromServer(SOCKET& serverSocket, cBuffer& buffer, int bufSize)
{
	int result = recv(serverSocket, (char*)(&buffer.mBufferData[0]), bufSize, 0);

	if (result == SOCKET_ERROR)
	{
		printf("Receive ---> FAILED | ERROR : %d\n", WSAGetLastError());
		CleanSocket(serverSocket, info);
		return 1;
	}

	return 0;
}

int cNetworkManager::CheckForResponseFromServer(SOCKET& serverSocket, cBuffer& buffer, int bufSize, FD_SET& socketsReadyForReading, timeval& tv)
{
	int count = select(0, &socketsReadyForReading, NULL, NULL, &tv);

	if (count == 0)
	{
		return 2;
	}
	if (count == SOCKET_ERROR)
	{
		// Handle an error
		printf("Select() ---> FAILED | ERROR : %d\n", WSAGetLastError());
		return 1;
	}
	if (count > 0)
	{
		if (!fd_cleared)
		{
			FD_CLR(serverSocket, &socketsReadyForReading);
			fd_cleared = true;
		}
		else
		{
			if (FD_ISSET(serverSocket, &socketsReadyForReading))
			{
				int result = ReceiveMessageFromServer(serverSocket, buffer, bufSize);
				return result;
			}
		}
	}

	return 2;
}