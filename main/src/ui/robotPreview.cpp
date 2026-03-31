#include "actions/Action.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/trigonometric.hpp"
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
		float deg = glm::degrees(preview.curPos.z);
		if (ImGui::DragFloat("robot heading", &deg))
			preview.curPos.z = glm::radians(deg);
		if (preview.trajectory.pedro == nullptr)
			ImGui::Button("Trajectory: None");
		else
		{
			bool pressed = false;
			if (projectSettings.pathType == PathType_Pedro)
			{
				if (preview.trajectory.pedro->label[0] != 0)
					pressed = ImGui::Button(preview.trajectory.pedro->label);
				else
					pressed = ImGui::Button("Trajectory: Trajectory");

				if (pressed)
				{
					preview.trajectory.pedro = nullptr;
					preview.playing = false;
				}
			}
			else
			{
				if (preview.trajectory.rr->label[0] != 0)
					pressed = ImGui::Button(preview.trajectory.rr->label);
				else
					pressed = ImGui::Button("Trajectory: Trajectory");

				if (pressed)
				{
					preview.trajectory.rr = nullptr;
					preview.playing = false;
				}
			}
		}
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("action");
			if (payload)
			{
				Action* a = *(Action**)payload->Data;
				if (a->type == ACTION_TRAJECTORY)
				{
					if (projectSettings.pathType == PathType_Pedro)
					{
						preview.trajectory.pedro = (PedroPathing::TrajectoryPedro*)a->traj;
						snprintf(preview.trajectory.pedro->label, 45, "Trajectory: %s", a->label);
						generatePathPedro(preview.trajectory.pedro);
					}
					else
					{
						preview.trajectory.rr = (RoadRunner::TrajectoryRR*)a->traj;
						snprintf(preview.trajectory.rr->label, 45, "Trajectory: %s", a->label);
						generatePathRR(preview.trajectory.rr);
					}
				}
			}
			else
			{
				preview.playing = false;
				if (projectSettings.pathType == PathType_Pedro)
					preview.trajectory.pedro = nullptr;
				else
					preview.trajectory.rr = nullptr;
			}
		}
		ImGui::BeginDisabled(!(preview.active && preview.trajectory.pedro != nullptr));
		ImGui::Checkbox("active", &preview.active);
		ImGui::DragFloat2("robot postiion", glm::value_ptr(preview.curPos));
		if (ImGui::Button("generate"))
		{
			if (projectSettings.pathType == PathType_Pedro)
				generatePathPedro(preview.trajectory.pedro);
			else
				generatePathRR(preview.trajectory.rr);
		}
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
		ImGui::EndDisabled();
		ImGui::End();
	}
}
