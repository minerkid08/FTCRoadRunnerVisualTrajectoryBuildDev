#include "glm/gtc/type_ptr.hpp"
#include "ui.hpp"

#include "imgui/imgui.h"
#include <vector>

void drawTrajectoryEditor(NodeGrid* grid)
{
	if (ImGui::Button("flipHoriz"))
		grid->flipHoriz();
	ImGui::SameLine();
	if (ImGui::Button("flipVert"))
		grid->flipVert();
	ImGui::Checkbox("grid snap", &(grid->gridSnap));
	ImGui::Checkbox("globally visible", &(grid->visible));
	if (ImGui::Button("^"))
	{
		if (grid->selected.type == TypeNode)
		{
			if (grid->nodes.moveUp(grid->selected.ind))
				grid->selected.ind++;
		}
		else
		{
			if (grid->segs.moveUp(grid->selected.ind))
				grid->selected.ind++;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("v"))
	{
		if (grid->selected.type == TypeNode)
		{
			if (grid->nodes.moveDown(grid->selected.ind))
				grid->selected.ind--;
		}
		else
		{
			if (grid->segs.moveDown(grid->selected.ind))
				grid->selected.ind--;
		}
	}
	int id = 0;
	ImGui::Text("nodes: %d/%d", grid->nodes.count, maxNodes);
	int j = 0;
	if (grid->nodes.count > 0)
	{
		for (int i = 0; i < grid->nodes.count; i++)
		{
			PathNode* node = grid->nodes.get(i);

			ImVec4 tint(0.25f, 0.25f, 0.25f, 1);
			if (grid->selected.ind == i && grid->selected.type == TypeNode)
				tint.x = 0.75f;

			ImGui::PushStyleColor(ImGuiCol_Button, tint);
			ImGui::PushID(id++);
			if (ImGui::Button(std::to_string(i).c_str()))
			{
				grid->selected.ind = i;
				grid->selected.type = TypeNode;
			}
			ImGui::PopID();
			ImGui::PopStyleColor();
			if (j < 16 && i < grid->nodes.count - 1)
			{
				ImGui::SameLine();
				j++;
			}
			else
				j = 0;
		}
	}

	ImGui::Text("segments: %d/%d", grid->segs.count, maxSegs);
	j = 0;
	if (grid->segs.count > 0)
	{
		for (int i = 0; i < grid->segs.count; i++)
		{
			PathSegment* seg = grid->segs.get(i);

			ImVec4 tint(0.25f, 0.25f, 0.25f, 1);
			if (grid->selected.ind == i && grid->selected.type == TypeSegment)
				tint.x = 0.75f;

			ImGui::PushStyleColor(ImGuiCol_Button, tint);
			ImGui::PushID(id++);
			if (ImGui::Button(std::to_string(i).c_str()))
			{
				grid->selected.ind = i;
				grid->selected.type = TypeSegment;
			}
			ImGui::PopID();
			ImGui::PopStyleColor();
			if (j < 16 && i < grid->segs.count - 1)
			{
				ImGui::SameLine();
				j++;
			}
			else
				j = 0;
		}
		ImGui::Text(" ");
	}

	ImGui::Separator();

	if (grid->selected.type == TypeNode)
	{
		if (grid->selected.ind > -1 && grid->selected.ind < grid->nodes.count)
		{
			PathNode* node = grid->nodes.get(grid->selected.ind);
			ImGui::Text("node: %d", grid->selected.ind);
			if (ImGui::Button("remove"))
			{
				grid->nodes.remove(grid->selected.ind);
				std::vector<int> toRemove;
				for (int i = 0; i < grid->segs.count; i++)
				{
					PathSegment* seg = grid->segs.get(i);
					if (seg->startNode == grid->selected.ind || seg->endNode == grid->selected.ind)
						toRemove.push_back(i);
					if (seg->startNode > grid->selected.ind)
						seg->startNode--;
					if (seg->endNode > grid->selected.ind)
						seg->endNode--;
				}
				int j = 0;
				for (int i : toRemove)
					grid->segs.remove(i - j++);
				return;
			}
			ImGui::InputFloat2("pos", glm::value_ptr(node->pos));
			ImGui::InputFloat("heading", &node->heading);
		}
	}
	else
	{
		if (grid->selected.ind > -1 && grid->selected.ind < grid->segs.count)
		{
			PathSegment* seg = grid->segs.get(grid->selected.ind);
			ImGui::Text("segment: %d", grid->selected.ind);
			if (ImGui::Button("remove"))
				grid->segs.remove(grid->selected.ind);
			static const char* headingModes[] = {"none", "linear", "constant", nullptr};

			if (ImGui::BeginCombo("heading mode", headingModes[seg->headingMode]))
			{
				for (int i = 0; i < 3; i++)
				{
					if (ImGui::Selectable(headingModes[i]))
						seg->headingMode = i;
				}
				ImGui::EndCombo();
			}
			ImGui::InputFloat("start tangent", &seg->startTan);
			ImGui::InputFloat("end tangent", &seg->endTan);
		}
	}
}
