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
	std::string userName;
	int irating = 0;
	std::string licenseStr;
	int lapCount = 0;
	float delta = 0.0f;
	int position = 0;
	float best = 0.0f;
	float last = 0.0f;
	bool hasFastestLap = false;
};

// read standings data
static int GetStandings(std::vector<CarInfo>& carInfoList) {
	const int maxCars = 64;
	carInfoList.clear();

	int leaderIdx = -1;
	for (int i = 0; i < maxCars; ++i) {
		if (ir_getPosition(i) == 1) {
			leaderIdx = i;
			break;
		}
	}

	for (int i = 0; i < maxCars; ++i) {
		const Car& car = ir_session.cars[i];
		if (car.isPaceCar || car.isSpectator || car.userName.empty())
			continue;

		CarInfo info;
		info.carIdx = i;
		info.userName = car.userName;
		info.irating = car.irating;
		info.licenseStr = car.licenseStr;
		info.lapCount = ir_CarIdxLapCompleted.getInt(i);
		info.delta = (ir_session.sessionType == SessionType::RACE) ? -ir_CarIdxF2Time.getFloat(i) : 0.0f;
		info.position = ir_getPosition(i);
		info.best = ir_CarIdxBestLapTime.getFloat(i);
		info.last = ir_CarIdxLastLapTime.getFloat(i);

		if ((ir_session.sessionType == SessionType::RACE && ir_SessionState.getInt() <= irsdk_StateWarmup) ||
			(ir_session.sessionType == SessionType::QUALIFY && info.best <= 0))
			info.best = car.qualTime;

		carInfoList.push_back(info);
	}

	std::sort(carInfoList.begin(), carInfoList.end(), [](const CarInfo& a, const CarInfo& b) {
		return a.position < b.position;
		});

	return static_cast<int>(carInfoList.size());
}


void overlay::draw_standings() {
    std::vector<CarInfo> cars;
    int numCars = GetStandings(cars);
    if (numCars == 0) return;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 screenSize = io.DisplaySize;

    float x = 0.0f;
    float y = 0.0f;

    // Calculate dynamic height
    float rowHeight = ImGui::GetTextLineHeightWithSpacing();
    float headerHeight = rowHeight * 2.0f; // for header
    float tableHeight = rowHeight * (float)numCars;
    float desiredHeight = headerHeight + tableHeight + 16.0f; // some padding

    float width = screenSize.x * 0.3f; // screen width percentage

    // Only set position/size on first use, so user can move/resize after
    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(width, desiredHeight), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(width, 120), ImVec2(width, 3000));

    if (ImGui::Begin("TeLIX Standings", nullptr,
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoFocusOnAppearing)) {

        ImGui::Text("Live Race Standings");

        if (ImGui::BeginTable("StandingsTable", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("CarIdx");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("iRating");
            ImGui::TableSetupColumn("License");
            ImGui::TableSetupColumn("Lap");
            ImGui::TableSetupColumn("DeltaToLeader");
            ImGui::TableSetupColumn("Pos");
            ImGui::TableSetupColumn("Last");

            ImGui::TableHeadersRow();

            for (const auto& car : cars) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", car.carIdx);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", car.userName.c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%d", car.irating);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%s", car.licenseStr.c_str());
                ImGui::TableSetColumnIndex(4); ImGui::Text("%d", car.lapCount);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.3f", car.delta);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%d", car.position);
                ImGui::TableSetColumnIndex(7); ImGui::Text("%.3f", car.last);
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
}