#include "overlay.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "iracing.h"
#include <functional>
#include <deque>
#include <algorithm>

constexpr size_t MAX_HISTORY_SIZE = 300; // record 5 seconds back

struct TelemetryHistory {
	std::deque<float> throttle;
	std::deque<float> brake;

	void push(float t, float b) {
		if (throttle.size() >= MAX_HISTORY_SIZE) { // get rid of old values
			throttle.pop_front();
			brake.pop_front();
		}
		throttle.push_back(t); // add new values
		brake.push_back(b);
	}
};

static TelemetryHistory history;

//Telemetry graph for seperate graphs, not in same box. ImGUI doesn't native support this. need to switch from Plotlines
 
void DrawTelemetryGraph(float graphWidth, float graphHeight) {
	if (history.throttle.empty()) return; // check if empty

	auto draw_series = [&](const std::deque<float>& data, const char* label, ImVec4 color) {
		std::vector<float> plot(data.begin(), data.end());
		ImGui::TextColored(color, "%s", label);
		ImGui::PlotLines("##Plot", plot.data(), static_cast<int>(plot.size()), 0, nullptr, 0.0f, 1.0f, ImVec2(graphWidth, graphHeight));
		};


	draw_series(history.throttle, "Throttle", ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // green
	draw_series(history.brake, "Brake", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // red
}

void overlay::draw_telemetry() {
	const float graphWidth = 450.0f;
	const float graphHeight = 100.0f;

	// read telemetry data
	float brake = ir_Brake.getFloat();       // range from 0 to 1
	float throttle = ir_Throttle.getFloat(); // range from 0 to 1
	
	//not using yet
	//float steering = ir_SteeringWheelAngle.getFloat(); // range from 0 to 1

	// history buffer
	history.push(throttle, brake);

	// positioning
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 screenSize = io.DisplaySize;

	float x = screenSize.x * 0.0f;
	float y = screenSize.y - 100.0f;

	ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver, ImVec2(0.0f, 1.0f));
	ImGui::SetNextWindowSize(ImVec2(500, 180), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 120), ImVec2(800, 400));

	if (ImGui::Begin("TeLIX Telemetry", nullptr,
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoCollapse)) {

		ImGui::Text("Real-Time Driving Inputs");
		DrawTelemetryGraph(graphWidth, graphHeight);

		ImGui::End();
	}
}
