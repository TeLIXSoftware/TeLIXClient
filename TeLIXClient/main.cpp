#include "iracing.h"
#include "overlay.h"
#include <iostream>

std::string takePasswdFromUser()
{
	HANDLE hStdInput
		= GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;

	// Create a restore point Mode
	// is know 503
	GetConsoleMode(hStdInput, &mode);

	// Enable echo input
	// set to 499
	SetConsoleMode(
		hStdInput,
		mode & (~ENABLE_ECHO_INPUT));

	// Take input
	std::string ipt;
	std::getline(std::cin, ipt);

	// Otherwise next cout will print
	// into the same line
	std::cout << std::endl;

	// Restore the mode
	SetConsoleMode(hStdInput, mode);

	return ipt;
}

void hook() {

	// Initialize the iRacing SDK
	ConnectionStatus status = ConnectionStatus::UNKNOWN;
	unsigned frameNumber = 0;

	while (true) {
		ConnectionStatus pStatus = status;
		SessionType prevSessionType = ir_session.sessionType;

		status = ir_tick();

		if (status != pStatus) {
			switch (status) {
			case ConnectionStatus::CONNECTED:
				printf("Connected to iRacing\n");
				if (overlay::isImGuiContext()) {
					overlay::cleanup();
				}
				break;
			case ConnectionStatus::DISCONNECTED:
				printf("Disconnected from iRacing\n");
				if (overlay::isImGuiContext()) {
					overlay::cleanup();
				}
				break;
			case ConnectionStatus::DRIVING:
				if (!overlay::isImGuiContext()) {
					overlay::init();
				}
				break;
			default:
				break;
			}
		}

		if (overlay::isImGuiContext()) {
			overlay::render();
		}

		// handle lap uploading here
		// keep track of lap count and then every lap upload data


	}
	//
	if (overlay::isImGuiContext()) {
		overlay::cleanup();
	}
}


int main(int argc, char* argv[])
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	std::string username = "";
	std::string password = "";
	bool valid = false;
	do {
		std::cout << "Email: ";
		std::getline(std::cin, username);

		std::cout << "Password: ";
		password = takePasswdFromUser();

		valid = username == "admin" && password == "1234";

		if (!valid) {
			std::cout << "Incorrect Credentials, try again!\n" << std::endl;
		}

	} while (valid != true);



	if (valid) {
		std::cout << "Login successful!\n" << std::endl;
		// handle api call here
		hook();
	}

	return 0;
}