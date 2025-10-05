#pragma once

#include "actions/CustomAction.hpp"
#include "trajectories/Trajectory.hpp"
#include <vector>

#define ActionFlags_Deleted 1

#define ACTION_SEQUENTIONAL 0
#define ACTION_PARALLEL 1
#define ACTION_TRAJECTORY 2

struct Action
{
	int type = 0;
  char flags = 0;
  unsigned long long id = 0;
  union
  {
    Trajectory* traj;
    std::vector<CustomActionField>* fields;
  };
  int dataSize;

  Action* next = nullptr;
  Action* prev = nullptr;
  Action* actions = nullptr;
  Action* parrent = nullptr;
};

void reset();
void addAction(Action* parent);
void moveAction(Action* action, Action* newParent);
void moveActionDown(Action* action);
void moveActionUp(Action* action);
void deleteAction(Action* action);
void deleteActionList(Action* action);

void initCustomAction(Action* action);
void tryDelete(Action* action);
void tryChangeType(Action* action, int newType);
void confermAction();
