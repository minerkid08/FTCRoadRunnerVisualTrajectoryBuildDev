#include <imgui/imgui.h>
#include <string>
#include <vector>

#include "ui.hpp"

struct LogElement
{
	std::string text;
	int level;
	LogElement(const std::string& msg, int l) : level(l), text(msg)
	{
	}
};

static std::vector<LogElement> elements;
static ImGuiTextFilter filter;
static bool autoScroll = true;

void clearLog()
{
	elements.clear();
}

void addLog(int level, const std::string& msg)
{
	if (level == LEVEL_INFO)
		elements.emplace_back("[info] " + msg, level);
	if (level == LEVEL_WARN)
		elements.emplace_back("[warn] " + msg, level);
	if (level == LEVEL_ERROR)
		elements.emplace_back("[error] " + msg, level);
}

void drawLog(bool* open)
{
	if (*open == false)
		return;
	ImGui::Begin("logs", open);

	// Main window
	bool clear = ImGui::Button("clear");
	ImGui::SameLine();
	filter.Draw("filter", -100.0f);

	ImGui::Separator();

	if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (clear)
			clearLog();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		if (filter.IsActive())
		{
			for (const LogElement& elem : elements)
			{
				if (filter.PassFilter(elem.text.c_str()))
				{
					if (elem.level == LEVEL_ERROR)
						ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.0f, 0.0f, 1.0f});
					if (elem.level == LEVEL_WARN)
						ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 1.0f, 0.0f, 1.0f});
					ImGui::Text("%s", elem.text.c_str());
					if (elem.level == LEVEL_WARN || elem.level == LEVEL_ERROR)
						ImGui::PopStyleColor();
				}
			}
		}
		else
		{
			ImGuiListClipper clipper;
			clipper.Begin(elements.size());
			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
				{
					const LogElement& elem = elements[i];
					if (elem.level == LEVEL_ERROR)
						ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.0f, 0.0f, 1.0f});
					if (elem.level == LEVEL_WARN)
						ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 1.0f, 0.0f, 1.0f});
					ImGui::Text("%s", elem.text.c_str());
					if (elem.level == LEVEL_WARN || elem.level == LEVEL_ERROR)
						ImGui::PopStyleColor();
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
	}
	ImGui::End();
}
