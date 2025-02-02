#include "Action.hpp"
#include "global.hpp"

void addAction(Action* parent)
{
	int actionInd = global.actions.size();
	Action* action = new Action();
	global.actions.emplace_back(action);

	action->id = actionInd;
	action->parrent = parent;

	if (parent->actions)
	{
		Action* a = parent->actions;
		while (true)
		{
			if (a->next)
				a = a->next;
			else
				break;
		}
		a->next = action;
		action->prev = a;
	}
	else
	{
		parent->actions = action;
	}
}

void reset()
{
	for (const Action* action : global.actions)
	{
		if (action->type == ACTION_TRAJECTORY)
		{
			delete action->data;
		}
		delete action;
	}
	global.actions.clear();
	global.rootAction = nullptr;
	global.currentAction = nullptr;
}

void moveAction(Action* a, Action* action)
{
	if (a->prev)
	{
		a->prev->next = a->next;
	}
	else
	{
		a->parrent->actions = a->next;
	}
	if (a->next)
	{
		a->next->prev = a->prev;
	}

	a->next = nullptr;
	a->parrent = action;
	if (action->actions == nullptr)
	{
		action->actions = a;
		a->prev = nullptr;
	}
	else
	{
		Action* action2 = action->actions;
		while (true)
		{
			if (action2->next == nullptr)
				break;
			action2 = action2->next;
		}
		action2->next = a;
		a->prev = action2;
	}
}

void moveActionDown(Action* action)
{
	if (action->next == nullptr)
		return;

	Action* action2 = action->next;
	Action* temp;

  action->next = action2->next;
  action2->prev = action->prev;
  action2->next = action;
  action->prev = action2;

  if(action2->prev == nullptr)
    action2->parrent->actions = action2;
}

void moveActionUp(Action* action)
{
	if (action->prev == nullptr)
		return;

	Action* action2 = action->prev;
	Action* temp;

  action2->next = action->next;
  action->prev = action2->prev;
  action->next = action2;
  action2->prev = action;

  if(action->prev == nullptr)
    action->parrent->actions = action;
}
