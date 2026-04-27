#include "actions/CustomAction.hpp"
#include "projectSettings.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"
#include "ui.hpp"

#include "actions/Action.hpp"
#include "global.hpp"

#include "imgui/imgui.h"
#include "ui/popup.hpp"
#include <vector>

void drawActionEditor()
{
	ImGui::Begin("Action Editor");
	if (global.currentAction != nullptr)
	{
		Action* a = global.currentAction;
		int newtype = a->type;
		ImGui::BeginDisabled(a == global.rootAction);
		if (ImGui::Combo("Type", &newtype, global.actionTypeStr))
		{
			if (newtype != a->type)
				tryChangeType(a, newtype);
		}
		ImGui::InputText("Label", a->label, sizeof(a->label));
		drawChangeFromPopups();
		ImGui::EndDisabled();
		if (a->type < 2)
		{
			if (ImGui::Button("Add action"))
				addAction(a);
		}
		ImGui::Separator();
		if (a->type == ACTION_TRAJECTORY)
		{
			if (projectSettings.pathType == PathType_RR)
				drawTrajectoryEditorRR((RoadRunner::TrajectoryRR*)a->traj);
			if (projectSettings.pathType == PathType_Pedro)
				drawTrajectoryEditorPedro((PedroPathing::TrajectoryPedro*)a->traj);
		}
		else if (a->type > 2)
		{
			std::vector<CustomActionField>* data = a->fields;
			for (CustomActionField& field : *data)
			{
				switch (field.type)
				{
				case FIELDTYPE_STRING:
					ImGui::InputText(field.name, field.value.s, 64);
					break;
				case FIELDTYPE_INT:
					if (field.rangeChecks)
					{
						int min = field.min.i;
						int max = field.max.i;
						ImGui::SliderInt(field.name, &field.value.i, min, max);
					}
					else
						ImGui::InputInt(field.name, &field.value.i);
					break;
				case FIELDTYPE_BOOL:
					ImGui::Checkbox(field.name, &field.value.b);
					break;
				case FIELDTYPE_DOUBLE:
					if (field.rangeChecks)
					{
						float min = field.min.f;
						float max = field.max.f;
						ImGui::SliderFloat(field.name, &field.value.f, min, max);
					}
					else
						ImGui::InputFloat(field.name, &field.value.f);
					break;
				}
			}
		}
	}
	ImGui::End();
}
