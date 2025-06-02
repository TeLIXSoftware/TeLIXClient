#include "iracing.h"
#include "overlay.h"

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
				break;
			case ConnectionStatus::DRIVING:
				if (!overlay::isImGuiContext()) {
					printf("init");
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


	}
	//
	if (overlay::isImGuiContext()) {
		overlay::cleanup();
	}
}


int main(int argc, char* argv[])
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	hook();

	// overlay cleanup
}