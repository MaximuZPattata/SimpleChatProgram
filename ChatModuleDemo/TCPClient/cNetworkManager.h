#pragma once
#include "pch.h"
#include "cBuffer.h"

struct sPacketHeader
{
	uint32_t packetSize;
	uint32_t messageType;
};

struct sChatMessage
{
	sPacketHeader header;
	uint32_t messageLength;
	std::string message;
};

class cNetworkManager
{
private:

	bool fd_cleared = false;

public:

	struct addrinfo* info = nullptr;
	struct addrinfo hints;

	cNetworkManager();
	~cNetworkManager();

	void SendMessageToBuffer(sChatMessage& message, cBuffer& buffer, std::string input);
	int WriteResponse(sChatMessage& message, cBuffer& buffer, std::string response, SOCKET& serverSocket);
	void CleanSocket(SOCKET& serverSocket, PADDRINFOA info);

	int InitializeWSA();
	int InitializeAddrInfo(const char* ipaddress, const char* port);

	int CreateSocket(SOCKET& serverSocket);
	int ConnectSocket(SOCKET& serverSocket);

	int ReceiveMessageFromServer(SOCKET& serverSocket, cBuffer& buffer, int bufSize);
	int CheckForResponseFromServer(SOCKET& serverSocket, cBuffer& buffer, int bufSize, FD_SET& socketsReadyForReading, timeval& tv);

};

