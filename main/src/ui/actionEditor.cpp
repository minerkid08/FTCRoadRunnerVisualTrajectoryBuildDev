#include "actions/CustomAction.hpp"
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
