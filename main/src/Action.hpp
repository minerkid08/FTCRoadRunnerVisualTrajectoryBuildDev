#pragma once

#include "NodeGrid.hpp"
#define ActionFlags_Deleted 1

#define ACTION_SEQUENTIONAL 0
#define ACTION_PARALLEL 1
#define ACTION_TRAJECTORY 2

struct Action
{
	int type = 0;
  char flags = 0;
  unsigned long long id = 0;
  NodeGrid* data = nullptr;
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
