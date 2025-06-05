#include "overlay.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "iracing.h"
#include <algorithm>
#include <vector>

//struct Car
//{
//	std::string     userName;
//	int             carNumber = 0;
//	std::string     carNumberStr;
//	std::string     licenseStr;
//	char            licenseChar = 'R';
//	float           licenseSR = 0;
//	std::string     licenseColStr;
//	float4          licenseCol = float4(0, 0, 0, 1);
//	int             irating = 0;
//	int             isSelf = 0;
//	int             isPaceCar = 0;
//	int             isSpectator = 0;
//	int             isBuddy = 0;
//	int             isFlagged = 0;
//	int             incidentCount = 0;
//	float           carClassEstLapTime = 0;
//	int             practicePosition = 0;
//	int             qualPosition = 0;
//	float           qualTime = 0;
//	int             racePosition = 0;
//	int             lastLapInPits = 0;
//};

struct CarInfo {
	int carIdx = 0;
	int lapCount = 0;
	int lapDelta = 0;
	float delta = 0.0f;
	int position = 0;
	float best = 0.0f;
	float last = 0.0f;
};

// read standings data
static int GetStandings(std::vector<CarInfo>& carInfoList) {
	const int maxCars = 64;
	carInfoList.clear();

	for (int i = 0; i < maxCars; ++i) {
		// skip invalid cars
		if (ir_CarIdxTrackSurface.getInt(i) == -1) continue;

		CarInfo info;
		info.carIdx = i;
		info.lapCount = ir_CarIdxLap.getInt(i);
		info.lapDelta = ir_CarIdxLapDistPct.getInt(i); 
		//info.delta = ir_getLapDeltaToLeader.getFloat(i);
		//info.position = ir_getPosition.getInt(i);
		info.best = ir_CarIdxBestLapTime.getFloat(i);
		info.last = ir_CarIdxLastLapTime.getFloat(i);

		carInfoList.push_back(info);
	}

	// sort by position
	std::sort(carInfoList.begin(), carInfoList.end(), [](const CarInfo& a, const CarInfo& b) {
		return a.position < b.position;
		});

	return static_cast<int>(carInfoList.size());
}


void overlay::draw_standings() {
	const float tableWidth = 600.0f;
	const float tableHeight = 150.0f;

	// read standings data
	std::vector<CarInfo> cars;
	int numCars = GetStandings(cars);
	if (numCars == 0) return;

	// positioning
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 screenSize = io.DisplaySize;

	float x = 0.0f;
	float y = screenSize.y - 10.0f;

	ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 120), ImVec2(800, 400));

	if (ImGui::Begin("TeLIX Standings", nullptr,
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoFocusOnAppearing)) {

		ImGui::Text("Live Race Standings");

		if (ImGui::BeginTable("StandingsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("CarIdx");
			ImGui::TableSetupColumn("Lap");
			ImGui::TableSetupColumn("DeltaLap");
			ImGui::TableSetupColumn("DeltaToLeader");
			ImGui::TableSetupColumn("Pos");
			ImGui::TableSetupColumn("Last");

			ImGui::TableHeadersRow();

			for (const auto& car : cars) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("%d", car.carIdx);
				ImGui::TableSetColumnIndex(1); ImGui::Text("%d", car.lapCount);
				ImGui::TableSetColumnIndex(2); ImGui::Text("%d", car.lapDelta);
				ImGui::TableSetColumnIndex(3); ImGui::Text("%.3f", car.delta);
				ImGui::TableSetColumnIndex(4); ImGui::Text("%d", car.position);
				ImGui::TableSetColumnIndex(5); ImGui::Text("%.3f", car.last);
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}
}
