#include "settings.hpp"

#include "global.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <json/json.hpp>
#include <set>
#include <sstream>
#include <vector>

extern bool logOpen;
extern bool viewportOpen;
extern bool actionEditorOpen;
extern bool actionListOpen;
extern bool settingsOpen;
extern bool previewOpen;

void loadSettings()
{
	int languageStrLen = 0;

	for (const std::filesystem::path& p : std::filesystem::directory_iterator("lang"))
	{
		if (std::filesystem::is_directory(p))
			continue;
		int extLen = p.extension().string().length() - 1;
		std::string filename = p.filename().string();
		char* name = (char*)malloc(filename.length() - extLen);
		memcpy(name, filename.c_str(), filename.length() - extLen - 1);
		name[filename.length() - extLen - 1] = 0;
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
		nlohmann::json windowJson = settingsJson["windows"];

		logOpen = windowJson["log"];
		viewportOpen = windowJson["viewport"];
		actionEditorOpen = windowJson["editor"];
		actionListOpen = windowJson["list"];
		previewOpen = windowJson["preview"];
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

	nlohmann::json windowJson;
	windowJson["log"] = logOpen;
	windowJson["viewport"] = viewportOpen;
	windowJson["editor"] = actionEditorOpen;
	windowJson["list"] = actionListOpen;
	windowJson["preview"] = previewOpen;

	settingsJson["windows"] = windowJson;
	std::ofstream ofstream("settings.json");
	ofstream << settingsJson.dump(2);
	ofstream.close();
}
