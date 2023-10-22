#pragma once
#include "pch.h"
#include "cBuffer.h"

// Structure to represent the packet header
struct sPacketHeader
{
	uint32_t packetSize;
	uint32_t messageType;
};

// Structure to represent message details
struct sChatMessage
{
	sPacketHeader header;
	uint32_t messageLength;
	std::string message;
};

// This class is created to manage the network related functionalities
class cNetworkManager
{
private:

	bool mFDCleared = false;

public:

	// Pointers and structures for managing network connections
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

