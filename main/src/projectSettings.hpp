#include "json/json.hpp"
#define PathType_Pedro 0
#define PathType_RR 1

struct ProjectSettings
{
  int pathType;
};

extern ProjectSettings projectSettings;

void generateActionNames();
int loadProjectSettings(const nlohmann::json& json);
void saveProjectSettings(nlohmann::json& json);
