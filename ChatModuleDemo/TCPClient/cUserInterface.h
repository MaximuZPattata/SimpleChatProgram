#pragma once
#include "pch.h"
#include "cNetworkManager.h"
#include "cBuffer.h"

class cUserInterface
{
private:

	bool user_writing = false;
	bool fd_cleared = false;
	bool escape_sequence_pressed = false;
	bool joined_room = false;
	bool chat_ready = false;
	bool type_ready = false;

public:

	cUserInterface();
	~cUserInterface();

	int GetClientName(cNetworkManager& networkManager, SOCKET& serverSocket, cBuffer& buffer, sChatMessage& message);
	int CheckUserInput(cNetworkManager& networkManager, int key, SOCKET& serverSocket, cBuffer& buffer, sChatMessage& message);

	void PrintReceivedMessage(cBuffer& buffer);

	bool isUserWriting();

	void ErasePreviousLines(int lineEraseCount);
	//void setUserWriting(bool value);
};

