#pragma once
#include "pch.h"
#include "cNetworkManager.h"
#include "cBuffer.h"

//This class is created to implement the user interface
class cUserInterface
{
private:

	bool mUserWriting = false; // To check if the user is typing on the console
	bool mEscapeSequencePressed = false; // To check if the user has pressed the 'ESC' button
	bool mJoinedRoom = false; // To check if the user has joined a room
	bool mChatReady = false; // To check if the user is allowed to chat
	bool mTypeReady = false; // To check if the user has entered the typing zone

public:

	cUserInterface();
	~cUserInterface();

	int GetClientName(cNetworkManager& networkManager, SOCKET& serverSocket, cBuffer& buffer, sChatMessage& message);
	int CheckUserInput(cNetworkManager& networkManager, int key, SOCKET& serverSocket, cBuffer& buffer, sChatMessage& message);

	void PrintReceivedMessage(cBuffer& buffer);

	bool isUserWriting();

	void ErasePreviousLines(int lineEraseCount);
};

