#pragma once

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

struct sRoom
{
	std::vector<SOCKET> clients;
};


class cNetworkManager
{
private:

	std::map<std::string, sRoom> rooms;

	std::map<SOCKET, std::string> clientRooms;

	std::map<SOCKET, std::string> clientName;

	std::vector<SOCKET> mClientConnectionList;

public:

	struct addrinfo* info = nullptr;
	struct addrinfo hints;

	int readCount = 0;

	void SendMessageToBuffer(sChatMessage& message, cBuffer& buffer, std::string input);
	void CleanSocket(SOCKET& serverSocket, PADDRINFOA info);
	int InitializeWSA();
	int InitializeAddrInfo(const char* ipaddress, const char* port);
	int CreateSocket(SOCKET& serverSocket);
	int Bind(SOCKET& serverSocket);
	int Listen(SOCKET& serverSocket);
	int SendMessageToClient(sChatMessage& message, cBuffer& buffer, std::string fullMessage, SOCKET clientSocket);
	int ReceiveMessageFromClient(SOCKET clientSocket, cBuffer& buffer, int bufSize);

	void AddAllClientsToFDSET(FD_SET& socketsReadyForReading);
	int AddNewClientToList(SOCKET& serverSocket, FD_SET& socketsReadyForReading, FD_SET& activeSockets);
	void RemoveClientFromList(SOCKET& clientSocket);

	int LoopThroughClientList(FD_SET& socketsReadyForReading, FD_SET& activeSockets, sChatMessage& message, cBuffer& buffer, int bufSize);
	int BroadcastToAll(SOCKET& clientSocket, bool newClientJoined, cBuffer& buffer, sChatMessage message, std::string userRoom);
};

