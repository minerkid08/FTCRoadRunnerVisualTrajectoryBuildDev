#include "actions/Action.hpp"
#include "trajectories/NodeGrid.hpp"
#include "ui.hpp"

#include "global.hpp"

#include "imgui/imgui.h"
#include "ui/popup.hpp"

static void drawAction(Action* action)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
							   ImGuiTreeNodeFlags_AllowItemOverlap |
							   (action == global.currentAction ? ImGuiTreeNodeFlags_Selected : 0);
	if (action->type > 1)
		flags |= ImGuiTreeNodeFlags_Bullet;

	bool opened;
	if (action->type == ACTION_TRAJECTORY)
	{
		NodeGrid* grid = action->data;
		if (grid->visible)
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 120, 255, 255));
		opened = ImGui::TreeNodeEx((void*)action->id, flags, "%s", global.actionTypes[action->type]);
		if (grid->visible)
			ImGui::PopStyleColor();
	}
	else
		opened = ImGui::TreeNodeEx((void*)action->id, flags, "%s", global.actionTypes[action->type]);
	ImVec2 size = ImGui::GetItemRectSize();
	if (ImGui::IsItemClicked())
		global.currentAction = action;

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("action");
		if (payload)
		{
			Action* a = *(Action**)payload->Data;
			if (action->type < 2)
				moveAction(a, action);
		}
		ImGui::EndDragDropTarget();
	}

	if (action != global.rootAction)
	{
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("action", (void*)&action, sizeof(void*));
			ImGui::EndDragDropSource();
		}
	}
	if (action == global.currentAction)
	{
		ImGui::SameLine();
		ImGui::BeginDisabled(action == global.rootAction);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 0));
		if (ImGui::Button("^", ImVec2(size.y, size.y)))
			moveActionUp(action);
		ImGui::SameLine();
		if (ImGui::Button("v", ImVec2(size.y, size.y)))
			moveActionDown(action);
		ImGui::SameLine();
		if (ImGui::Button("x", ImVec2(size.y, size.y)))
			tryDelete(action);
		ImGui::PopStyleVar();
		ImGui::EndDisabled();
	}

	if (opened)
	{
		if (action->actions)
		{
			Action* action2 = action->actions;
			while (true)
			{
				drawAction(action2);
				if (action2->next == nullptr)
					break;
				action2 = action2->next;
			}
		}
		drawDeletePopups();
		ImGui::TreePop();
	}
}
void drawActionList()
{
	if (global.currentAction != nullptr)
	{
		if (global.currentAction->parrent == nullptr && global.currentAction != global.rootAction)
			global.currentAction = nullptr;
	}
	ImGui::Begin("actionList");
	if (global.rootAction == nullptr)
	{
		global.rootAction = new Action();
		global.actions.push_back(global.rootAction);
	}
	drawAction(global.rootAction);

	ImGui::End();
}
