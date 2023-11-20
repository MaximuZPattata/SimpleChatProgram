#include "pch.h"
#include "cUserInterface.h"

//Constructor
cUserInterface::cUserInterface()
{

}

//Destructor
cUserInterface::~cUserInterface()
{

}

// Function to prompt the user for their name and send it to the server.
// [param_1]: Reference to the cNetworkManager instance.
// [param_2]: Reference to the server socket instance.
// [param_3]: Reference to the cBuffer instance for handling messages.
// [param_4]: Reference to the sChatMessage instance.
// [return_value]: The error indication is passed as an integer value (1 - error, 0 - success).
int cUserInterface::GetClientName(cNetworkManager& networkManager, SOCKET& serverSocket, cBuffer& buffer, sChatMessage& message)
{
	std::string userInput;
	std::string tempString;

	//Asking for client name
	printf("\nENTER YOUR NAME : ");

	mUserWriting = true;

	std::getline(std::cin, userInput);

	mUserWriting = false;

	tempString = "$NAME$" + userInput;

	int result = networkManager.WriteResponse(message, buffer, tempString, serverSocket);

	if (result == SOCKET_ERROR)
	{
		printf("\nMessage Sending ---> FAILED | ERROR : %d\n", WSAGetLastError());
		networkManager.CleanSocket(serverSocket, networkManager.info);
		return 1;
	}
	else
	{
		printf("\n<<WELCOME %s>>\n", userInput.c_str());
		printf("\n[PRESS ESCAPE FOR OPTIONS]\n");
		return 0;
	}
}

// Function to erase the previous lines in the console.
// [param_1]: Number of lines to erase.
void cUserInterface::ErasePreviousLines(int lineEraseCount)
{
	for (int i = 0; i < lineEraseCount; i++)
		printf("\r\033[K\033[1A");

	printf("\r\033[K");

}


// Function to check and handle user input.
// [param_1]: Reference to the cNetworkManager instance.
// [param_2]: The key code(integer value) corresponding to the user's input.
// [param_3]: Reference to the server socket instance.
// [param_4]: Reference to the cBuffer instance for handling messages.
// [param_5]: Reference to the sChatMessage instance.
// [return_value]: The error indication is passed as an integer value (1 - error, 0 - success).
int cUserInterface::CheckUserInput(cNetworkManager& networkManager, int key, SOCKET& serverSocket, cBuffer& buffer, sChatMessage& message)
{
	std::string userInput;
	std::string tempString;
	int result = 0;

	if (!mTypeReady && key == 27 /*'ESC'*/)
	{
		if (!mEscapeSequencePressed)
		{
			mEscapeSequencePressed = true;

			if (!mChatReady)
				printf("\n**PRESS 'J' TO JOIN A ROOM**\n");

			else
				printf("\n**PRESS 'E' TO EXIT THE ROOM**\n");
		}
		else
		{
			ErasePreviousLines(2);
			mEscapeSequencePressed = false;
		}
	}

	else if (mJoinedRoom == false && mEscapeSequencePressed == true && (key == 74 /*'J'*/ || key == 106 /*'j'*/))
	{
		mEscapeSequencePressed = false;

		ErasePreviousLines(2);
		printf("\nENTER ROOM NAME : ");

		mUserWriting = true;

		std::getline(std::cin, userInput);

		mUserWriting = false;

		tempString = "$JOIN$" + userInput;

		result = networkManager.WriteResponse(message, buffer, tempString, serverSocket);

		if (result != 0)
			return 1;
		else
			mJoinedRoom = true;

		printf("\n");
	}

	else if (mChatReady == true && key == 9 /*'TAB'*/)
	{
		if (!mTypeReady)
		{
			mTypeReady = true;

			printf("\nTYPE HERE : ");

			mUserWriting = true;

			std::getline(std::cin, userInput);

			mUserWriting = false;

			tempString = "$CHAT$" + userInput;

			result = networkManager.WriteResponse(message, buffer, tempString, serverSocket);

			if (result != 0)
				return 1;

			else
			{
				mTypeReady = false;
				printf("\033[1A\r\033[K");
			}
		}
		else
		{
			mTypeReady = false;
			printf("\r\033[K");
		}
	}
	else if (mJoinedRoom == true && mEscapeSequencePressed == true && (key == 69 /*'E'*/ || key == 101 /*'e'*/))
	{
		mEscapeSequencePressed = false;

		ErasePreviousLines(2);

		tempString = "$EXIT$";

		result = networkManager.WriteResponse(message, buffer, tempString, serverSocket);

		if (result != 0)
			return 1;
	}

	return 0;
}

// Function to check if the user is currently entering text.
// [return_value]: Returns true if the user is writing; otherwise, returns false.
bool cUserInterface::isUserWriting()
{
	return mUserWriting;
}

// Function to print received messages in the console.
// [param_1]: Reference to the cBuffer instance containing the received message.
void cUserInterface::PrintReceivedMessage(cBuffer& buffer)
{
	uint32_t messageLength = buffer.ReadUInt16BE();
	std::string msg = buffer.ReadString(messageLength);

	std::string tempString = msg.substr(0, 7);

	if (tempString == "$READY$")
	{
		mChatReady = true;
		printf("%s\n", msg.substr(7).c_str());
		printf("\n[PRESS 'TAB' TO TYPE MESSAGE | PRESS 'ESCAPE' FOR OPTIONS]\n");
		printf("\n-----------------------------<<YOUR CHAT BEGINS HERE>>-----------------------------------\n");
	}
	else if (msg == "$LEFT$")
	{
		mJoinedRoom = false;
		mChatReady = false;
		printf("\n-----------------------------<<YOUR CHAT ENDS HERE>>-----------------------------------\n");
		printf("\n**YOU HAVE LEFT THE ROOM**\n");
		printf("\n[PRESS 'ESCAPE' FOR OPTIONS]\n");
	}
	else if (tempString == "$REPLY$")
	{
		printf("\n%s\n", msg.substr(7).c_str());
	}
	else
	{
		printf("%s\n", msg.c_str());
	}

}