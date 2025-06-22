#include "actions/Action.hpp"
#include "actions/CustomAction.hpp"
#include <vector>

struct Globals
{
	int mouseOffsetX;
	int mouseOffsetY;
	bool onViewport;

  float uiScale = 0.0;

	std::vector<Action*> actions;
	Action* rootAction = nullptr;
	Action* currentAction = nullptr;

  std::string filename;
  int explorerMode;

  std::vector<const char*> actionTypes = {"sequentional", "parallel", "trajectory"};
  char* actionTypeStr;

  const char* languageStr;

  std::vector<const char*> languages;

  std::vector<CustomActionDef> customActionDefs;
};

extern Globals global;
