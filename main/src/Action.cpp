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
