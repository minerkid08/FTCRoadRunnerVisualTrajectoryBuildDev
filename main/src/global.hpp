#include "actions/Action.hpp"
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

	const char* actionTypes[3] = {"sequentional", "parallel", "trajectory"};

	const int actionTypeCount = 3;
};

extern Globals global;
