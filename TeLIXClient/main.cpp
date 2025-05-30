
#include "iracing.h"


int main(int argc, char* argv[])
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);




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
				break;
			case ConnectionStatus::DISCONNECTED:
				printf("Disconnected from iRacing\n");
				break;
			default:
				break;
			}
		}

		if (status == ConnectionStatus::DRIVING) {
			// print current speed
			printf("Speed: %.2f m/s\n", ir_Speed.getFloat());

		}
	}
}