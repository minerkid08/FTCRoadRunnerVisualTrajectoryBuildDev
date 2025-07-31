#include "ui.hpp"

#include "actions/CustomAction.hpp"
#include "global.hpp"
#include "settings.hpp"

#include "imgui/imgui.h"
#include <cstring>
#include <vector>

static bool open = false;

std::vector<CustomActionDef> settingsActions;
static int toRemove = -1;

static const char* str = "int\0double\0bool\0string\0";

static void drawFields(CustomActionDef& def, bool selected);
static void applyCustomFields(bool force = false);

static std::string usedAction;

void drawSettingsMenu()
{
	if (open)
	{
		ImGui::Begin("settings", &open, ImGuiWindowFlags_NoDocking);
		if (!open)
		{
			saveSettings();
		}

		ImGui::SeparatorText("export");
		ImGui::InputText("save path", settings.savePath, 512);
		ImGui::InputText("export path", settings.exportPath, 512);
		ImGui::Combo("export language", &settings.language, global.languageStr);
		ImGui::SeparatorText("trajectories");
		ImGui::SliderFloat("trajectory alpha", &settings.trajectoryOpac, 0.0f, 1.0f);

		ImGui::SeparatorText("custom actions");

		if (ImGui::Button("add"))
			settingsActions.emplace_back();

		static int selectedInd = 0;

		unsigned long long i = 0;

		for (CustomActionDef& def : settingsActions)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
									   ImGuiTreeNodeFlags_AllowOverlap |
									   (i == selectedInd ? ImGuiTreeNodeFlags_Selected : 0);
			bool opened = ImGui::TreeNodeEx((void*)i, flags, "%s", def.name);
			if (ImGui::IsItemClicked())
				selectedInd = i;

			if (selectedInd == i)
			{
				ImVec2 size = ImGui::GetItemRectSize();
				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 0));
				if (ImGui::Button("^", ImVec2(size.y, size.y)))
				{
					if (i > 0)
					{
						CustomActionDef a = settingsActions[i];
						settingsActions[i] = settingsActions[i - 1];
						settingsActions[i - 1] = a;
						selectedInd--;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("v", ImVec2(size.y, size.y)))
				{
					if (i < settingsActions.size() - 1)
					{
						CustomActionDef a = settingsActions[i];
						settingsActions[i] = settingsActions[i + 1];
						settingsActions[i + 1] = a;
						selectedInd++;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("x", ImVec2(size.y, size.y)))
					toRemove = i;
				ImGui::PopStyleVar();
			}
			if (opened)
			{
				ImGui::InputText("name", def.name, 64);

				drawFields(def, i == selectedInd);

				ImGui::TreePop();
			}
			i++;
		}

		if (ImGui::Button("apply"))
		{
			applyCustomFields();
		}

		if (ImGui::BeginPopupModal("apply removing used trajectory"))
		{
			ImGui::Text("Applying changes will delete an with a type '%s'", usedAction.c_str());
			ImGui::Text("Are you sure you want to continue.");
			if (ImGui::Button("confirm"))
			{
				applyCustomFields(true);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("cancel"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		ImGui::End();

		if (toRemove != -1)
		{
			CustomActionDef& def = settingsActions[toRemove];

			for (int i = toRemove; i < settingsActions.size() - 1; i++)
				settingsActions[i] = settingsActions[i + 1];
			settingsActions.resize(i - 1);
			if (i >= toRemove)
				selectedInd--;
			toRemove = -1;
		}
	}
}

void openSettings()
{
	open = !open;
	if (open == false)
		saveSettings();
}

static void drawFields(CustomActionDef& def, bool selected)
{
	ImGui::SeparatorText("fields");
	if (ImGui::Button("add"))
		def.fields.emplace_back();

	static int selectedInd = 0;

	unsigned long long i = 0;

	static int toRemove = -1;

	for (CustomActionField& field : def.fields)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
								   ImGuiTreeNodeFlags_AllowOverlap |
								   (i == selectedInd && selected ? ImGuiTreeNodeFlags_Selected : 0);
		bool opened = ImGui::TreeNodeEx((void*)(i + 512), flags, "%s", field.name);
		if (ImGui::IsItemClicked())
			selectedInd = i;

		if (selectedInd == i && selected)
		{
			ImVec2 size = ImGui::GetItemRectSize();
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 0));
			if (ImGui::Button("-", ImVec2(0, size.y)))
				toRemove = i;
			ImGui::SameLine();
			if (ImGui::Button("^", ImVec2(0, size.y)))
			{
				if (i > 0)
				{
					CustomActionField a = def.fields[i];
					def.fields[i] = def.fields[i - 1];
					def.fields[i - 1] = a;
					selectedInd--;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("v", ImVec2(0, size.y)))
			{
				if (i < settingsActions.size() - 1)
				{
					CustomActionField a = def.fields[i];
					def.fields[i] = def.fields[i + 1];
					def.fields[i + 1] = a;
					selectedInd++;
				}
			}
			ImGui::PopStyleVar();
		}
		if (opened)
		{
			ImGui::InputText("name", field.name, 64);
			int oldValue = field.type;
			if (ImGui::Combo("type", &field.type, str))
			{
				if (oldValue != field.type)
				{
					if (oldValue == FIELDTYPE_STRING)
						free(field.value);
					field.value = 0;
					if (field.type == FIELDTYPE_STRING)
					{
						field.value = malloc(64);
						strcpy((char*)field.value, "");
					}
				}
			}
			switch (field.type)
			{
			case FIELDTYPE_STRING:
				ImGui::InputText("default value", (char*)field.value, 64);
				break;
			case FIELDTYPE_INT:
				ImGui::InputInt("default value", (int*)&field.value);
				ImGui::Checkbox("min/max", &field.rangeChecks);

				ImGui::BeginDisabled(!field.rangeChecks);
				ImGui::InputInt("min value", (int*)&field.min);
				ImGui::InputInt("max value", (int*)&field.max);
				ImGui::EndDisabled();
				break;
			case FIELDTYPE_BOOL:
				ImGui::Checkbox("default value", (bool*)&field.value);
				break;
			case FIELDTYPE_DOUBLE:
				ImGui::InputFloat("default value", (float*)&field.value);
				ImGui::Checkbox("min/max", &field.rangeChecks);

				ImGui::BeginDisabled(!field.rangeChecks);
				ImGui::InputFloat("min value", (float*)&field.min);
				ImGui::InputFloat("max value", (float*)&field.max);
				ImGui::EndDisabled();
				break;
			}
			ImGui::TreePop();
		}
		i++;
	}
	if (toRemove != -1)
	{
		CustomActionField& field = def.fields[toRemove];

		for (int i = toRemove; i < def.fields.size() - 1; i++)
			def.fields[i] = def.fields[i + 1];
		def.fields.resize(i - 1);
		if (i >= toRemove)
			selectedInd--;
		toRemove = -1;
	}
}

static void applyCustomFields(bool force)
{
	if (!force)
	{
		for (const Action* action : global.actions)
		{
			if (action->type < 3)
				continue;

			bool found = false;

			const CustomActionDef* def2 = &global.customActionDefs[action->type - 3];
			for (const CustomActionDef& def : settingsActions)
			{
				if (strcmp(def.name, def2->name) == 0)
				{
					found = true;
					break;
				}
			}
			if (found)
				continue;
			usedAction = def2->name;
			ImGui::OpenPopup("apply removing used trajectory");
			return;
		}
	}

	for (Action* action : global.actions)
	{
		if (action->type < 3)
			continue;

		bool found = false;

		const CustomActionDef* def2 = &global.customActionDefs[action->type - 3];
		for (int j = 0; j < settingsActions.size(); j++)
		{
			const CustomActionDef* def = &settingsActions[j];
			if (strcmp(def->name, def2->name) == 0)
			{
				action->type = j + 3;
				found = true;
				break;
			}
		}
		if (found)
			continue;

		deleteAction(action);
	}

	global.customActionDefs.resize(0);
	for (const CustomActionDef& def : settingsActions)
	{
		global.customActionDefs.push_back(def);
	}

	global.actionTypes.resize(3);
	for (CustomActionDef& def : global.customActionDefs)
		global.actionTypes.push_back(def.name);

	const char* sequentional = "sequentional";
	const char* parallel = "parallel";
	const char* trajectory = "trajectory";

	free(global.actionTypeStr);
	int len = 0;
	len += strlen(sequentional) + 1;
	len += strlen(parallel) + 1;
	len += strlen(trajectory) + 1;

	for (CustomActionDef& def : global.customActionDefs)
		len += strlen(def.name) + 1;

	global.actionTypeStr = (char*)malloc(len + 1);
	int i = 0;
	strcpy(global.actionTypeStr, sequentional);
	i += strlen(sequentional) + 1;
	strcpy(global.actionTypeStr + i, parallel);
	i += strlen(parallel) + 1;
	strcpy(global.actionTypeStr + i, trajectory);
	i += strlen(trajectory) + 1;

	for (CustomActionDef& def : global.customActionDefs)
	{
		strcpy(global.actionTypeStr + i, def.name);
		i += strlen(def.name) + 1;
	}
	global.actionTypeStr[len] = 0;

	for (Action* action : global.actions)
	{
		if (action->type < 3)
			continue;

		if (action->parrent == nullptr)
			continue;

		bool found = false;

		const CustomActionDef* def2 = &global.customActionDefs[action->type - 3];
		for (const CustomActionDef& def : global.customActionDefs)
		{
			if (strcmp(def.name, def2->name) == 0)
			{
				customActionReload((std::vector<CustomActionField>*)action->data, *def2);
				found = true;
				break;
			}
		}
		if (found)
			continue;
	}
	saveSettings();
}
