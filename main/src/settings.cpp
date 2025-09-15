#include "settings.hpp"

#include "global.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <json/json.hpp>
#include <sstream>
#include <vector>

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

		strcpy(settings.savePath, ((std::string)settingsJson["savePath"]).c_str());
		strcpy(settings.exportPath, ((std::string)settingsJson["exportPath"]).c_str());
		settings.language = settingsJson["exportLanguage"];
		settings.trajectoryOpac = settingsJson["trajectoryAlpha"];
	}
	else
	{
		strcpy(settings.savePath, "./save");
		strcpy(settings.exportPath, "./export");
	}
}

void saveSettings()
{
	nlohmann::json settingsJson;
	settingsJson["savePath"] = settings.savePath;
	settingsJson["exportPath"] = settings.exportPath;
	settingsJson["exportLanguage"] = settings.language;
	settingsJson["trajectoryAlpha"] = settings.trajectoryOpac;

	std::ofstream ofstream("settings.json");
	ofstream << settingsJson.dump(2);
	ofstream.close();
}
