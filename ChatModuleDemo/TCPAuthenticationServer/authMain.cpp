#include <AuthProtocolMessages.pb.h>
#include <string>
#include <iostream>

int main(int argc, char** argv) 
{
	accountAuthProtocol::AccountCreation* registerAccount = new accountAuthProtocol::AccountCreation;

	registerAccount->set_requestid(1);
	registerAccount->set_email("MalluArjunEsnapur@gmail.com");
	registerAccount->set_plaintextpassword("Its_my_new_password");

	std::string serializedString;
	registerAccount->SerializeToString(&serializedString);

	std::cout << serializedString << std::endl;

	for (int indexString = 0; indexString < serializedString.length(); indexString++)
		printf("%02X ", serializedString[indexString]);

	printf("\n\n");

	accountAuthProtocol::AccountCreation deserializeData;

	bool success = deserializeData.ParseFromString(serializedString);

	if (!success)
		std::cout << "Failed to parse address book\n" << std::endl;
	
	std::cout << "Parsing successful\n" << std::endl;

	std::cout << "Email: " << deserializeData.email() << std::endl;
	std::cout << "Pwd: " << deserializeData.plaintextpassword() << std::endl;
}

