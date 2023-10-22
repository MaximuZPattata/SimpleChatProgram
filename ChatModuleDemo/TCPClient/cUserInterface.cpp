#include "pch.h"
#include "cUserInterface.h"

cUserInterface::cUserInterface()
{

}
cUserInterface::~cUserInterface()
{

}

int cUserInterface::GetClientName(cNetworkManager& networkManager, SOCKET& serverSocket, cBuffer& buffer, sChatMessage& message)
{
	std::string userInput;
	std::string tempString;

	//Asking for client name
	printf("\nENTER YOUR NAME : ");

	user_writing = true;

	std::getline(std::cin, userInput);

	user_writing = false;

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

void cUserInterface::ErasePreviousLines(int lineEraseCount)
{
	for (int i = 0; i < lineEraseCount; i++)
		printf("\r\033[K\033[1A");

	printf("\r\033[K");

}

int cUserInterface::CheckUserInput(cNetworkManager& networkManager, int key, SOCKET& serverSocket, cBuffer& buffer, sChatMessage& message)
{
	std::string userInput;
	std::string tempString;
	int result = 0;

	if (!type_ready && key == 27 /*'ESC'*/)
	{
		if (!escape_sequence_pressed)
		{
			escape_sequence_pressed = true;

			if (!chat_ready)
				printf("\n**PRESS 'J' TO JOIN A ROOM**\n");

			else
				printf("\n**PRESS 'E' TO EXIT THE ROOM**\n");
		}
		else
		{
			ErasePreviousLines(2);
			escape_sequence_pressed = false;
		}
	}

	else if (joined_room == false && escape_sequence_pressed == true && (key == 74 /*'J'*/ || key == 106 /*'j'*/))
	{
		escape_sequence_pressed = false;

		ErasePreviousLines(2);
		printf("\nENTER ROOM NAME : ");

		user_writing = true;

		std::getline(std::cin, userInput);

		user_writing = false;

		tempString = "$JOIN$" + userInput;

		result = networkManager.WriteResponse(message, buffer, tempString, serverSocket);

		if (result != 0)
			return 1;
		else
			joined_room = true;

		printf("\n");
	}

	else if (chat_ready == true && key == 9 /*'TAB'*/)
	{
		if (!type_ready)
		{
			type_ready = true;

			printf("\nTYPE HERE : ");

			user_writing = true;

			std::getline(std::cin, userInput);

			user_writing = false;

			tempString = "$CHAT$" + userInput;

			result = networkManager.WriteResponse(message, buffer, tempString, serverSocket);

			if (result != 0)
				return 1;

			else
			{
				type_ready = false;
				printf("\033[1A\r\033[K");
			}
		}
		else
		{
			type_ready = false;
			printf("\r\033[K");
		}
	}
	else if (joined_room == true && escape_sequence_pressed == true && (key == 69 /*'E'*/ || key == 101 /*'e'*/))
	{
		escape_sequence_pressed = false;

		ErasePreviousLines(2);

		tempString = "$EXIT$";

		result = networkManager.WriteResponse(message, buffer, tempString, serverSocket);

		if (result != 0)
			return 1;
	}

	return 0;
}


bool cUserInterface::isUserWriting()
{
	return user_writing;
}

void cUserInterface::PrintReceivedMessage(cBuffer& buffer)
{
	uint32_t messageLength = buffer.ReadUInt32BE();
	std::string msg = buffer.ReadString(messageLength);

	std::string tempString = msg.substr(0, 7);

	if (tempString == "$READY$")
	{
		chat_ready = true;
		printf("%s\n", msg.substr(7).c_str());
		printf("\n[PRESS 'TAB' TO TYPE MESSAGE | PRESS 'ESCAPE' FOR OPTIONS]\n");
		printf("\n-----------------------------<<YOUR CHAT BEGINS HERE>>-----------------------------------\n");
	}
	else if (msg == "$LEFT$")
	{
		joined_room = false;
		chat_ready = false;
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