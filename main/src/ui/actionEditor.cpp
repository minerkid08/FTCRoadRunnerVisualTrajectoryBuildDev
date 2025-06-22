#include "actions/CustomAction.hpp"
#include "trajectories/NodeGrid.hpp"
#include "ui.hpp"

#include "actions/Action.hpp"
#include "global.hpp"

#include "imgui/imgui.h"
#include <cstdlib>
#include <cstring>
#include <vector>

void drawActionEditor()
{
	ImGui::Begin("actionEditor");
	if (global.currentAction != nullptr)
	{
		Action* a = global.currentAction;
		int prevType = a->type;
		if (ImGui::Combo("type", &a->type, global.actionTypeStr))
		{
			if (prevType != a->type)
			{
				if (prevType == ACTION_TRAJECTORY)
				{
					delete a->data;
					a->data = nullptr;
				}
				else if (prevType > 2)
				{
					std::vector<CustomActionField>* data = (std::vector<CustomActionField>*)a->data;
					delete data;
					a->data = nullptr;
				}
				if (a->type == ACTION_TRAJECTORY)
				{
					if (a->data == nullptr)
						a->data = new NodeGrid();
				}
				else if (a->type > 2)
					initCustomAction(a);
			}
		}
		if (a->type < 2)
		{
			if (ImGui::Button("addAction"))
			{
				addAction(a);
			}
			ImGui::SameLine();
		}
		if (ImGui::Button("removeAction"))
		{
			if (!a->actions)
			{
			}
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
					ImGui::InputInt(field.name, (int*)&field.value);
					break;
				case FIELDTYPE_BOOL:
					ImGui::Checkbox(field.name, (bool*)&field.value);
					break;
				case FIELDTYPE_DOUBLE:
					ImGui::InputDouble(field.name, (double*)&field.value);
					break;
				}
			}
		}
	}
	ImGui::End();
}
