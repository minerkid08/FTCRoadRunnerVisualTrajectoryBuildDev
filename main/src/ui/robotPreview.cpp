#include "actions/Action.hpp"
#include "global.hpp"
#include "imgui/imgui.h"
#include "preview.hpp"
#include "projectSettings.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"

void drawPreviewWindow(bool* open)
{
	if (*open)
	{
		ImGui::Begin("Robot Preview", open);
		ImGui::Checkbox("active", &preview.active);
		if (preview.trajectory.pedro == nullptr)
			ImGui::Text("Trajectory: None");
		else
			ImGui::Text("Trajectory: Trajectory");
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("action");
			if (payload)
			{
				Action* a = *(Action**)payload->Data;
				if (a->type == ACTION_TRAJECTORY)
				{
					if (projectSettings.pathType == PathType_Pedro)
						preview.trajectory.pedro = (PedroPathing::TrajectoryPedro*)a->traj;
					else
						preview.trajectory.rr = (RoadRunner::TrajectoryRR*)a->traj;
				}
			}
			else
			{
				if (projectSettings.pathType == PathType_Pedro)
					preview.trajectory.pedro = nullptr;
				else
					preview.trajectory.rr = nullptr;
			}
		}
		ImGui::BeginDisabled(!(preview.active && preview.trajectory.pedro != nullptr));
		ImGui::SliderFloat("t", &preview.t, 0, 1);
		if (!preview.playing)
		{
			if (ImGui::Button("|>"))
				preview.playing = true;
		}
		else
		{
			if (ImGui::Button("||"))
				preview.playing = false;
		}

		ImGui::InputFloat("playback length", &preview.playbackLength);
		ImGui::Checkbox("single segment", &preview.useSingleSegment);
		if (preview.useSingleSegment)
		{
			if (ImGui::InputInt("segment id", &preview.singleSegmentId))
			{
				if (preview.singleSegmentId < 0)
					preview.singleSegmentId = 0;
				if (projectSettings.pathType == PathType_Pedro)
				{
					if (preview.singleSegmentId >= preview.trajectory.pedro->segs.count)
						preview.singleSegmentId = preview.trajectory.pedro->segs.count - 1;
				}
				else
				{
					if (preview.singleSegmentId >= preview.trajectory.rr->segs.count)
						preview.singleSegmentId = preview.trajectory.rr->segs.count - 1;
				}
			}
		}
		else
		{
			if (ImGui::Button("generate path"))
			{
				if (projectSettings.pathType == PathType_Pedro)
					generatePathPedro(preview.trajectory.pedro);
				else
					generatePathRR(preview.trajectory.rr);
			}
		}

		ImGui::Text("robot position: x %.2f, y %.2f, h %.2f", preview.curPos.x, preview.curPos.y,
					glm::degrees(preview.curPos.z));
		ImGui::EndDisabled();
		ImGui::End();
	}
}
