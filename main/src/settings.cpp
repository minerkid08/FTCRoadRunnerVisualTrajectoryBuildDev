#include "settings.hpp"

#include "actions/CustomAction.hpp"
#include "global.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <json/json.hpp>
#include <vector>
#include <sstream>

extern std::vector<CustomActionDef> settingsActions;

void loadSettings()
{
	int languageStrLen = 0;

	for (std::filesystem::path p : std::filesystem::directory_iterator("lang"))
	{
		std::string filename = p.filename().string();
		char* name = (char*)malloc(filename.length() - 4);
		memcpy(name, filename.c_str(), filename.length() - 5);
		name[filename.length() - 5] = 0;
		languageStrLen += strlen(name) + 1;
		global.languages.emplace_back(name);
	}

	languageStrLen += 1;
	char* languageStr = (char*)malloc(languageStrLen);

	int i = 0;
	for (const char* l : global.languages)
	{
		strcpy(languageStr + i, l);
		i += strlen(l) + 1;
	}

	languageStr[languageStrLen - 1] = 0;

	global.languageStr = languageStr;

	settings.savePath = (char*)malloc(512);
	settings.exportPath = (char*)malloc(512);

	if (std::filesystem::exists("settings.json"))
	{
		std::ifstream ifstream("settings.json");
		std::stringstream sstream;
		sstream << ifstream.rdbuf();
		nlohmann::json settingsJson = nlohmann::json::parse(sstream.str());

		int i = 0;
		for (nlohmann::json& action : settingsJson["customActions"])
		{
			global.customActionDefs.emplace_back();
			CustomActionDef& a = global.customActionDefs[i];
			std::string name = action["name"];
			strcpy(a.name, name.c_str());
			int j = 0;
			for (nlohmann::json& field : action["fields"])
			{
				a.fields.emplace_back();
				CustomActionField& f = a.fields[j];
				std::string name = field["name"];
				strcpy(f.name, name.c_str());
				f.type = field["type"];

				switch (f.type)
				{
				case FIELDTYPE_INT: {
					int min = field["min"];
					int max = field["max"];
					f.rangeChecks = field["rangeChecks"];
					int v = field["value"];
					f.value.i = v;
					f.min.i = min;
					f.max.i = max;
					break;
				}
				case FIELDTYPE_DOUBLE: {
					float min = field["min"];
					float max = field["max"];
					float v = field["value"];
					f.rangeChecks = field["rangeChecks"];
					f.value.f = v;
					f.min.f = min;
					f.max.f = max;
					break;
				}
				case FIELDTYPE_BOOL: {
					bool v = field["value"];
					f.value.b = v;
					break;
				}
				case FIELDTYPE_STRING: {
					std::string v = field["value"];
					f.value.s = (char*)malloc(64);
					strcpy(f.value.s, v.c_str());
					break;
				}
				}
				j++;
			}
			i++;
		}

		strcpy(settings.savePath, ((std::string)settingsJson["savePath"]).c_str());
		strcpy(settings.exportPath, ((std::string)settingsJson["exportPath"]).c_str());
		settings.language = settingsJson["exportLanguage"];
	}
	else
	{
		strcpy(settings.savePath, "./save");
		strcpy(settings.exportPath, "./export");
	}
	const char* sequentional = "sequentional";
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

	settingsActions = global.customActionDefs;
}

void saveSettings()
{
	nlohmann::json settingsJson;
	settingsJson["savePath"] = settings.savePath;
	settingsJson["exportPath"] = settings.exportPath;
	settingsJson["exportLanguage"] = settings.language;

	settingsJson["customActions"] = {};
	nlohmann::json& customActions = settingsJson["customActions"];
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

	std::ofstream ofstream("settings.json");
	ofstream << settingsJson.dump(2);
	ofstream.close();
}
