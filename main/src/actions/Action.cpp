#include "Action.hpp"
#include "global.hpp"
#include "projectSettings.hpp"
#include "trajectories/Trajectory.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"
#include "ui/popup.hpp"
#include <cstring>

void addAction(Action* parent)
{
	Action* action = nullptr;
	for (Action* a : global.actions)
	{
		if (a->parrent == nullptr && a != global.rootAction)
		{
			action = a;
			break;
		}
	}
	if (action == nullptr)
	{
		int actionInd = global.actions.size();
		action = new Action();
		global.actions.emplace_back(action);

		action->id = actionInd;
	}
	action->parrent = parent;
	action->prev = nullptr;
	action->next = nullptr;
	action->actions = nullptr;
	action->traj = nullptr;
	action->type = 0;

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
		parent->actions = action;
	memset(action->label, 0, sizeof(action->label));
}

void deleteAction(Action* action)
{
	if (action->type == ACTION_TRAJECTORY)
		delete action->traj;
	if (action->type < ACTION_TRAJECTORY)
		deleteActionList(action->actions);
	if (action->type > ACTION_TRAJECTORY)
		delete action->fields;

	if (action->next != nullptr)
	{
		action->next->prev = action->prev;
		if (action->prev == nullptr)
		{
			action->parrent->actions = action->next;
		}
	}

	if (action->prev != nullptr)
	{
		action->prev->next = action->next;
	}

	if (action->next == nullptr && action->prev == nullptr)
		action->parrent->actions = nullptr;
	action->parrent = nullptr;
	action->actions = nullptr;
	if (global.currentAction == action)
		global.currentAction = nullptr;
}

void deleteActionList(Action* action)
{
	Action* a = action;
	while (a != nullptr)
	{
		if (a->actions != nullptr)
			deleteActionList(a);
		a->parrent = nullptr;
		a = a->next;
	}
}

void reset()
{
	for (const Action* action : global.actions)
	{
		if (action->type == ACTION_TRAJECTORY)
		{
			delete action->traj;
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
	Action* prev = action->prev;
	Action* next = action2->next;

	if (next != nullptr)
		next->prev = action;
	if (prev != nullptr)
		prev->next = action2;

	action->next = action2->next;
	action2->prev = action->prev;
	action2->next = action;
	action->prev = action2;

	if (action2->prev == nullptr)
		action2->parrent->actions = action2;
}

void moveActionUp(Action* action)
{
	if (action->prev == nullptr)
		return;

	Action* action2 = action->prev;
	Action* prev = action2->prev;
	Action* next = action->next;

	if (next != nullptr)
		next->prev = action2;
	if (prev != nullptr)
		prev->next = action;
	action2->next = action->next;
	action->prev = action2->prev;
	action->next = action2;
	action2->prev = action;

	if (action->prev == nullptr)
		action->parrent->actions = action;
}

void initCustomAction(Action* action)
{
	std::vector<CustomActionField>* data = action->fields;
	if (action->fields != nullptr)
		data->resize(0);
	else
		data = new std::vector<CustomActionField>;

	CustomActionDef* def = &global.customActionDefs[action->type - 3];

	for (int i = 0; i < def->fields.size(); i++)
	{
		data->emplace_back();
		CustomActionField* fa = &def->fields[i];
		CustomActionField* fb = &(*data)[i];

		fb->type = fa->type;
		fb->min = fa->min;
		fb->max = fa->max;
		fb->rangeChecks = fa->rangeChecks;
		strcpy((char*)fb->name, (char*)fa->name);

		if (fa->type == FIELDTYPE_STRING)
		{
			fb->value.s = (char*)malloc(64);
			strcpy(fb->value.s, fa->value.s);
		}
		else
			fb->value = fa->value;
	}
	action->fields = data;
}

void tryDelete(Action* action)
{
	global.toChange.action = action;
	global.toChange.toDo = ToDo_Delete;
	if (action->type == ACTION_TRAJECTORY)
	{
		Trajectory* traj = action->traj;
		if (traj->canDelete)
		{
			openDeleteTrajectory();
			return;
		}
	}
	if (action->type < 2)
	{
		if (action->actions != nullptr)
		{
			openDeleteContainer();
			return;
		}
	}
	confermAction();
}

void tryChangeType(Action* action, int newType)
{
	global.toChange.action = action;
	global.toChange.toDo = ToDo_ChangeType;
	global.toChange.arg = newType;
	if (action->type == ACTION_TRAJECTORY)
	{
		Trajectory* traj = action->traj;
		if (traj->canDelete)
		{
			openChangeFromTrajectory();
			return;
		}
	}
	else if (action->type < 2)
	{
		if (action->actions != nullptr)
		{
			openChangeFromContainer();
			return;
		}
	}
	confermAction();
}

void confermAction()
{
	Action* action = global.toChange.action;
	global.toChange.action = nullptr;
	if (global.toChange.toDo == ToDo_Delete)
	{
		if (action->type < 2)
		{
			if (action->actions != nullptr)
				deleteActionList(action->actions);
		}
		else if (action->type == ACTION_TRAJECTORY)
		{
			delete action->traj;
			action->traj = nullptr;
		}
		else if (action->type > 2)
		{
			std::vector<CustomActionField>* data = action->fields;
			delete data;
			action->fields = nullptr;
		}
		deleteAction(action);
	}
	if (global.toChange.toDo == ToDo_ChangeType)
	{
		int newtype = global.toChange.arg;
		if (action->type == ACTION_TRAJECTORY)
		{
			delete action->traj;
			action->traj = nullptr;
		}
		else if (action->type < 2 && newtype > 1)
		{
			deleteActionList(action->actions);
			action->actions = nullptr;
		}
		else if (action->type > 2 && newtype <= 2)
		{
			std::vector<CustomActionField>* data = action->fields;
			delete data;
			action->fields = nullptr;
		}
		action->type = newtype;
		if (newtype == ACTION_TRAJECTORY)
		{
			if (projectSettings.pathType == PathType_RR)
				action->traj = new RoadRunner::TrajectoryRR();
			if (projectSettings.pathType == PathType_Pedro)
				action->traj = new PedroPathing::TrajectoryPedro();
		}
		else if (newtype > 2)
			initCustomAction(action);
	}
}
