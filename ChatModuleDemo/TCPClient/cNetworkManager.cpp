#include "pch.h"
#include "cNetworkManager.h"
#include "cBuffer.h"

//Constructor
cNetworkManager::cNetworkManager()
{ }

//Destructor
cNetworkManager::~cNetworkManager()
{ }


// [param_1]: Reference to the sChatMessage for message details.
// [param_2]: Reference to the cBuffer instance for message storage.
// [param_3]: The input message to send.
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
	buffer.WriteUInt16BE(message.header.messageType);
	buffer.WriteUInt16BE(message.messageLength);
	buffer.WriteString(message.message);
}

// Function to send a response message to the server.
// [param_1]: Reference to the sChatMessage for message details.
// [param_2]: Reference to the cBuffer instance for message storage.
// [param_3]: Message to send.
// [param_4]: Reference to the Server socket instance
// [return_value]: The error indication is passed as an integer value (1 - error, 0 - success).
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

// Function to close and clean up the socket and Windows Sockets API.
// [param_1]: Reference to the Server socket instance.
// [param_2]: Address information.
void cNetworkManager::CleanSocket(SOCKET& serverSocket, PADDRINFOA info)
{
	closesocket(serverSocket);
	freeaddrinfo(info);
	WSACleanup();
}

// Function to initialize the Windows Sockets API (WSA).
// [return_value]: The error indication is passed as an integer value (1 - error, 0 - success).
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

// Function to initialize address information.
// [param_1]: IP address as a string 
// [param_2]: Port number as a string
// [return_value]: The error indication is passed as an integer value (1 - error, 0 - success).
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

// Function to create a socket for the server connection.
// [param_1]: Reference to server socket instance.
// [return_value]: The error indication is passed as an integer value (1 - error, 0 - success).
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

// Function to connect the socket to the server.
// [param_1]: Reference to server socket instance.
// [return_value]: The error indication is passed as an integer value (1 - error, 0 - success).
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

// Function to receive a message from the server.
// [param_1]: Reference to server socket instance.
// [param_2]: Reference to the cBuffer instance for message storage.
// [param_3]: Size of the buffer in integer.
// [return_value]: The error indication is passed as an integer value (1 - error, 0 - success).
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

// Function to check for a response from the server using select.
// [param_1]: Reference to server socket instance.
// [param_2]: Reference to the cBuffer instance for message storage.
// [param_3]: Size of the buffer in integer.
// [param_4]: Set of sockets to check.
// [param_5]: Timeval for timeout.
// [return_value]: The error indication is passed as an integer value (2 - continue loop, 1 - error, 0 - success).
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
		if (!mFDCleared)
		{
			FD_CLR(serverSocket, &socketsReadyForReading);
			mFDCleared = true;
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