#include "actions/CustomAction.hpp"
#include "trajectories/NodeGrid.hpp"
#include "ui.hpp"

#include "actions/Action.hpp"
#include "global.hpp"

#include "imgui/imgui.h"
#include "ui/popup.hpp"
#include <vector>

void drawActionEditor()
{
	ImGui::Begin("actionEditor");
	if (global.currentAction != nullptr)
	{
		Action* a = global.currentAction;
		int newtype = a->type;
		ImGui::BeginDisabled(a == global.rootAction);
		if (ImGui::Combo("type", &newtype, global.actionTypeStr))
		{
			if (newtype != a->type)
				tryChangeType(a, newtype);
		}
		drawChangeFromPopups();
		ImGui::EndDisabled();
		if (a->type < 2)
		{
			if (ImGui::Button("addAction"))
				addAction(a);
		}
		ImGui::Separator();
		if (a->type == ACTION_TRAJECTORY)
			drawTrajectoryEditor(a->data);
		else if (a->type > 2)
		{
			std::vector<CustomActionField>* data = (std::vector<CustomActionField>*)a->data;
			for (CustomActionField& field : *data)
			{
				switch (field.type)
				{
				case FIELDTYPE_STRING:
					ImGui::InputText(field.name, (char*)field.value, 64);
					break;
				case FIELDTYPE_INT:
					if (field.rangeChecks)
					{
						int min = *(int*)&field.min;
						int max = *(int*)&field.max;
						ImGui::SliderInt(field.name, (int*)&field.value, min, max);
					}
					else
						ImGui::InputInt(field.name, (int*)&field.value);
					break;
				case FIELDTYPE_BOOL:
					ImGui::Checkbox(field.name, (bool*)&field.value);
					break;
				case FIELDTYPE_DOUBLE:
					if (field.rangeChecks)
					{
						float min = *(float*)&field.min;
						float max = *(float*)&field.max;
						ImGui::SliderFloat(field.name, (float*)&field.value, min, max);
					}
					else
						ImGui::InputFloat(field.name, (float*)&field.value);
					break;
				}
			}
		}
	}
	ImGui::End();
}
