#include "projectSettings.hpp"
#include "Save.hpp"
#include "actions/CustomAction.hpp"
#include "global.hpp"
#include "preview.hpp"
#include "json/json.hpp"
#include <vector>

extern std::vector<CustomActionDef> settingsActions;

int loadProjectSettings(const nlohmann::json& json)
{
	{
		tryGet(json, "trajectoryType", is_number, projectSettings.pathType);
		tryGet(json, "robotSizeX", is_number, preview.sizeX);
		tryGet(json, "robotSizeY", is_number, preview.sizeY);

		global.customActionDefs.clear();

		if (!json.contains("customActions"))
		{
			std::cerr << "custom actions no exist\n";
			goto err;
		}

		if (json["customActions"].is_array())
		{
			nlohmann::json customActionJson = json["customActions"];
			int i = 0;
			for (const nlohmann::json& action : customActionJson)
			{
				global.customActionDefs.emplace_back();
				CustomActionDef& a = global.customActionDefs[i];
				std::string name;
				tryGet(action, "name", is_string, name);
				strcpy(a.name, name.c_str());
				int j = 0;
				if (action["fields"].is_array())
				{
					typeCheck(action, "fields", is_array);
					for (const nlohmann::json& field : action["fields"])
					{
						if (!field.is_object())
						{
							std::cerr << "custom action field is not an object";
							goto err;
						}
						a.fields.emplace_back();
						CustomActionField& f = a.fields[j];
						std::string name;
						tryGet(field, "name", is_string, name);
						strcpy(f.name, name.c_str());
						f.type = field["type"];
						tryGet(field, "type", is_number, f.type);

						switch (f.type)
						{
						case FIELDTYPE_INT: {
							tryGet(field, "min", is_number, f.min.i);
							tryGet(field, "max", is_number, f.max.i);
							tryGet(field, "value", is_number, f.value.i);
							tryGet(field, "rangeChecks", is_boolean, f.rangeChecks);
							break;
						}
						case FIELDTYPE_DOUBLE: {
							tryGet(field, "min", is_number, f.min.f);
							tryGet(field, "max", is_number, f.max.f);
							tryGet(field, "value", is_number, f.value.f);
							tryGet(field, "rangeChecks", is_boolean, f.rangeChecks);
							break;
						}
						case FIELDTYPE_BOOL: {
							tryGet(field, "value", is_number, f.value.b);
							break;
						}
						case FIELDTYPE_STRING: {
							std::string v;
							tryGet(field, "value", is_string, v);
							f.value.s = (char*)malloc(64);
							strcpy(f.value.s, v.c_str());
							break;
						}
						}
						j++;
					}
				}
				i++;
			}
		}
	}
	settingsActions = global.customActionDefs;
	generateActionNames();
	return 0;
err:
	return 1;
}

void generateActionNames()
{
	const char* sequentional = "sequential";
	const char* parallel = "parallel";
	const char* trajectory = "trajectory";

	int len = 0;
	len += strlen(sequentional) + 1;
	len += strlen(parallel) + 1;
	len += strlen(trajectory) + 1;

	for (CustomActionDef& def : global.customActionDefs)
		len += strlen(def.name) + 1;

	global.actionTypeStr = (char*)malloc(len + 1);
	int l = 0;
	strcpy(global.actionTypeStr, sequentional);
	l += strlen(sequentional) + 1;
	strcpy(global.actionTypeStr + l, parallel);
	l += strlen(parallel) + 1;
	strcpy(global.actionTypeStr + l, trajectory);
	l += strlen(trajectory) + 1;

	for (CustomActionDef& def : global.customActionDefs)
	{
		strcpy(global.actionTypeStr + l, def.name);
		l += strlen(def.name) + 1;
	}
	global.actionTypeStr[len] = 0;

	global.actionTypes.resize(3);
	for (CustomActionDef& def : global.customActionDefs)
		global.actionTypes.push_back(def.name);
}

void saveProjectSettings(nlohmann::json& json)
{
	json["customActions"] = {};
	json["trajectoryType"] = projectSettings.pathType;
	json["robotSizeX"] = preview.sizeX;
	json["robotSizeY"] = preview.sizeY;
	nlohmann::json& customActions = json["customActions"];
	for (int i = 0; i < global.customActionDefs.size(); i++)
	{
		CustomActionDef& def = global.customActionDefs[i];
		customActions[i] = {};
		customActions[i]["name"] = def.name;
		customActions[i]["fields"] = {};
		nlohmann::json& fields = customActions[i]["fields"];
		for (int j = 0; j < def.fields.size(); j++)
		{
			CustomActionField& f = def.fields[j];
			fields[j] = {};
			fields[j]["name"] = f.name;
			fields[j]["type"] = f.type;
			switch (f.type)
			{
			case FIELDTYPE_INT:
				fields[j]["min"] = f.min.i;
				fields[j]["max"] = f.max.i;
				fields[j]["rangeChecks"] = f.rangeChecks;
				fields[j]["value"] = f.value.i;
				break;
			case FIELDTYPE_DOUBLE:
				fields[j]["min"] = f.min.f;
				fields[j]["max"] = f.max.f;
				fields[j]["rangeChecks"] = f.rangeChecks;
				fields[j]["value"] = f.value.f;
				break;
			case FIELDTYPE_BOOL:
				fields[j]["value"] = f.value.b;
				break;
			case FIELDTYPE_STRING:
				fields[j]["value"] = f.value.s;
				break;
			}
		}
	}
}
