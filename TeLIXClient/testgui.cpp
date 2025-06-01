#include "overlay.h"
#include "imgui/imgui.h"

#include "iracing.h"

void overlay::draw_test_gui() {
	ImGui::SetNextWindowSize(ImVec2(900, 200));

	float speed = ir_Speed.getFloat();
	float brake = ir_Brake.getFloat();
	float throttle = ir_Throttle.getFloat();
	float steeringAngle = ir_SteeringWheelAngle.getFloat();

	float gear = ir_Gear.getInt();
	float rpm = ir_RPM.getFloat();

	float bestLapTime = ir_LapBestLapTime.getFloat();
	double sessionTime = ir_SessionTime.getDouble();

	int currentLap = ir_Lap.getInt();
	int totalLaps = ir_SessionLapsTotal.getInt();

	float lastLapTime = ir_LapLastLapTime.getFloat();
	float p1LapTime = ir_LapDeltaToSessionBestLap.getFloat(); // prob +current lap time


	ImGui::Begin("Telemetry Dashboard", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);

	// Top Row
	ImGui::Text("Best: %.3f", bestLapTime);
	ImGui::SameLine(); ImGui::Text("Session: %.2f", sessionTime);
	ImGui::SameLine(); ImGui::Text("Lap: %d / %d (%d to go)", currentLap, totalLaps, totalLaps - currentLap);

	// Second Row
	ImGui::Text("Last Lap: %.3f", lastLapTime);
	ImGui::SameLine(); ImGui::Text("P1 Last: %.3f", p1LapTime);

	float oilTemp = ir_OilTemp.getFloat();
	float waterTemp = ir_WaterTemp.getFloat();




	// Temps
	ImGui::Separator();
	ImGui::Text("Oil: %.2fF", oilTemp);
	ImGui::SameLine(); ImGui::Text("Water: %.2fF", waterTemp);

	float brakeBias = ir_dcBrakeBias.getFloat();
	float lapDelta = ir_LapDeltaToOptimalLap.getFloat();

	// Incidents, Bias, Delta
	ImGui::Separator();
	ImGui::Text("Incidents: %d", 0);
	ImGui::SameLine(); ImGui::Text("Lap Delta: %.2f", lapDelta);
	ImGui::SameLine(); ImGui::Text("Bias: %.1f", brakeBias);

	ImGui::End();
}