#include "imgui/imgui.h"

static bool open = false;
void drawAboutWindow()
{
	if (open)
	{
		ImGui::Begin("About", &open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
		ImGui::Text("FTC Roadrunner Visual Trajectory Builder version 1.1");
		ImGui::Text("Made by Minerkid08 from team 21502");
		ImGui::TextLinkOpenURL("Github repository",
							   "https://github.com/minerkid08/FTCRoadRunnerVisualTrajectoryBuildDev");
		ImGui::End();
	}
}

void openAboutWindow()
{
	open = true;
}
