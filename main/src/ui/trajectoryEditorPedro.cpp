#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/trigonometric.hpp"
#include "preview.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "ui.hpp"

#include "imgui/imgui.h"
#include <array>
#include <vector>

void drawTrajectoryEditorPedro(PedroPathing::TrajectoryPedro* grid)
{
	if (grid == nullptr)
		return;

	if (ImGui::TreeNode("RobotPreview"))
	{
		ImGui::Checkbox("active", &preview.active);
		ImGui::BeginDisabled(!preview.active);
		ImGui::SliderFloat("t", &preview.t, 0, 1);
		if (ImGui::Button("|>"))
			preview.playing = true;
		ImGui::SameLine();
		if (ImGui::Button("||"))
			preview.playing = false;

		ImGui::InputFloat("playback length", &preview.playbackLength);
		if (ImGui::Checkbox("single segment", &preview.useSingleSegment))
			preview.trajectory.pedro = grid;
		if (preview.useSingleSegment)
		{
			if (ImGui::InputInt("segment id", &preview.singleSegmentId))
			{
				if (preview.singleSegmentId < 0)
					preview.singleSegmentId = 0;
				if (preview.singleSegmentId >= grid->segs.count)
					preview.singleSegmentId = grid->segs.count - 1;
			}
		}
		else
		{
			if (ImGui::Button("generate path"))
				generatePathPedro(grid);
		}

		ImGui::Text("robot position: x %.2f, y %.2f, h %.2f", preview.curPos.x, preview.curPos.y,
					glm::degrees(preview.curPos.z));
		ImGui::EndDisabled();
		ImGui::TreePop();
	}

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
			PedroPathing::PathNode* node = grid->nodes.get(i);

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
			PedroPathing::PathSegment* seg = grid->segs.get(i);

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
			PedroPathing::PathNode* node = grid->nodes.get(grid->selected.ind);
			ImGui::Text("node: %d", grid->selected.ind);
			if (ImGui::Button("remove"))
			{
				grid->nodes.remove(grid->selected.ind);
				std::vector<int> toRemove;
				for (int i = 0; i < grid->segs.count; i++)
				{
					PedroPathing::PathSegment* seg = grid->segs.get(i);
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
        if(grid->nodes.count == 0)
          grid->canDelete = false;
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
			PedroPathing::PathSegment* seg = grid->segs.get(grid->selected.ind);
			PedroPathing::PathNode* startNode = grid->nodes.get(seg->startNode);
			PedroPathing::PathNode* endNode = grid->nodes.get(seg->endNode);
			std::array<glm::vec2, maxCtrlPts>& ctrlPts = seg->controlPoints;

			ctrlPts[0] = startNode->pos;
			ctrlPts[seg->controlPointsCount - 1] = endNode->pos;

			ImGui::Text("segment: %d", grid->selected.ind);
			if (ImGui::Button("remove##X"))
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
			ImGui::SeparatorText("control points");
			if (ImGui::Button("add"))
			{
				if (seg->controlPointsCount < maxCtrlPts - 1)
				{
					glm::vec2 newPoint = ctrlPts[seg->controlPointsCount - 1];
					ctrlPts[seg->controlPointsCount] = newPoint;
					ctrlPts[seg->controlPointsCount - 1] = {0, 0};
					seg->controlPointsCount++;
				}
			}
			if (ImGui::Button("remove"))
			{
				if (seg->controlPointsCount > 2)
				{
					glm::vec2 oldPoint = ctrlPts[seg->controlPointsCount - 1];
					ctrlPts[seg->controlPointsCount - 2] = oldPoint;
					seg->controlPointsCount--;
				}
			}
			if (seg->controlPointsCount > 2)
			{
				for (int i = 1; i < seg->controlPointsCount - 1; i++)
				{
					ImGui::PushID(i);
					ImGui::InputFloat2("", glm::value_ptr(ctrlPts[i]));
					ImGui::PopID();
				}
			}
		}
	}
}
