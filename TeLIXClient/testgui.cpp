#include "overlay.h"
#include "imgui/imgui.h"

#include "iracing.h"

void overlay::draw_test_gui() {
	ImGui::SetNextWindowSize(ImVec2(600, 400));

	float speed = ir_Speed.getFloat();
	float brake = ir_Brake.getFloat();
	float throttle = ir_Throttle.getFloat();
	float steeringAngle = ir_SteeringWheelAngle.getFloat();

	float gear = ir_Gear.getInt();
	float rpm = ir_RPM.getFloat();


	ImGui::Begin("gui", nullptr, ImGuiWindowFlags_NoDecoration);
	{
		
		ImGui::Text("Speed: %f m/s", speed);
		ImGui::Text("Brake: %f", brake);
		ImGui::Text("Throttle: %f", throttle);
		ImGui::Text("Steering Angle: %f degrees", steeringAngle);
		ImGui::Text("Gear: %d", gear);
		ImGui::Text("RPM: %f r/m", rpm);

	}
	ImGui::End();
}