#include "Save.hpp"
#include "actions/Action.hpp"
#include "actions/CustomAction.hpp"
#include "global.hpp"
#include "projectSettings.hpp"
#include "trajectories/Trajectory.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"
#include "ui/ui.hpp"

#include <fstream>
#include <iostream>
#include <json/json.hpp>
#include <vector>

static int indexOfAction(const Action* action)
{
	if (action == nullptr)
		return -1;
	for (int i = 0; i < global.actions.size(); i++)
	{
		if (global.actions[i] == action)
			return i;
	}
	return -1;
}

void save(const std::string& filename)
{
	clearMsg();
	std::vector<Trajectory*> trajectories;
	nlohmann::json json;
	json["actions"] = {};
	json["settings"] = {};
	json["trajectories"] = {};

  saveProjectSettings(json["settings"]);

	int i = 0;
	for (const Action* action : global.actions)
	{
		nlohmann::json& j = json["actions"][i];
		j["type"] = action->type;
		j["parent"] = indexOfAction(action->parrent);
		j["next"] = indexOfAction(action->next);
		j["prev"] = indexOfAction(action->prev);
		j["actions"] = indexOfAction(action->actions);
		if (action->type == ACTION_TRAJECTORY)
		{
			j["trajectory"] = trajectories.size();
			trajectories.push_back(action->data);
		}
		if (action->type > 2)
		{
			j["fields"] = {};
			std::vector<CustomActionField>* fields = (std::vector<CustomActionField>*)action->data;
			for (CustomActionField& f : *fields)
			{
				switch (f.type)
				{
				case FIELDTYPE_INT:
					j["fields"][f.name] = f.value.i;
					break;
				case FIELDTYPE_DOUBLE:
					j["fields"][f.name] = f.value.f;
					break;
				case FIELDTYPE_BOOL:
					j["fields"][f.name] = f.value.b;
					break;
				case FIELDTYPE_STRING:
					j["fields"][f.name] = f.value.s;
					break;
				}
			}
		}
		i++;
	}
	i = 0;
	for (Trajectory* traj : trajectories)
	{
		nlohmann::json& j = json["trajectories"][i];
		if (projectSettings.pathType == PathType_RR)
		{
			RoadRunner::TrajectoryRR* trajectory = (RoadRunner::TrajectoryRR*)traj;
			RoadRunner::saveTrajectory(trajectory, j);
		}
		else if (projectSettings.pathType == PathType_Pedro)
		{
			PedroPathing::TrajectoryPedro* trajectory = (PedroPathing::TrajectoryPedro*)traj;
			PedroPathing::saveTrajectory(trajectory, j);
		}
		i++;
	}
	std::ofstream stream(filename);
	stream << json.dump(2);
	setNotif("saved " + filename);
}

Action* parseAction(const nlohmann::json& node, const nlohmann::json& trajectoryJson)
{
	Action* action = new Action();
	tryGet(node, "type", is_number, action->type);
	tryGetc(node, "parent", is_number, action->parrent, (Action*)(long long));
	tryGetc(node, "next", is_number, action->next, (Action*)(long long));
	tryGetc(node, "prev", is_number, action->prev, (Action*)(long long));
	tryGetc(node, "actions", is_number, action->actions, (Action*)(long long));

	if (action->type == ACTION_TRAJECTORY)
	{
		if (projectSettings.pathType == PathType_RR)
			action->data = RoadRunner::parseTrajectory(trajectoryJson, node["trajectory"]);
		else if (projectSettings.pathType == PathType_Pedro)
			action->data = PedroPathing::parseTrajectory(trajectoryJson, node["trajectory"]);

		if (action->data == 0)
		{
			delete action;
			return 0;
		}
	}

	if (action->type > 2)
	{
		initCustomAction(action);
		std::vector<CustomActionField>* data = (std::vector<CustomActionField>*)action->data;
		typeCheck(node, "fields", is_object);
		for (CustomActionField& field : *data)
		{
			switch (field.type)
			{
			case FIELDTYPE_INT: {
				int v;
				tryGet(node["fields"], field.name, is_number, v);
				field.value.i = v;
				break;
			}
			case FIELDTYPE_DOUBLE: {
				float v;
				tryGet(node["fields"], field.name, is_number, v);
				field.value.f = v;
				break;
			}
			case FIELDTYPE_BOOL: {
				bool v;
				tryGet(node["fields"], field.name, is_boolean, v);
				field.value.b = v;
				break;
			}
			case FIELDTYPE_STRING: {
				std::string v;
				tryGet(node["fields"], field.name, is_string, v);
				strcpy(field.value.s, v.c_str());
				break;
			}
			}
		}
	}
	return action;
err:
	delete action;
	return 0;
}

void load(const std::string& filename)
{
	clearMsg();
	reset();
	std::ifstream stream(filename);
	nlohmann::json json;
	try
	{
		stream >> json;
	}
	catch (nlohmann::json::parse_error e)
	{
		setErr("load failed: can't parse json file");
		return;
	}

	typeCheck(json, "settings", is_object);
	typeCheck(json, "actions", is_array);

	{
	  int r = loadProjectSettings(json["settings"]);
    if(r == 1)
    {
      setErr("load failed: can't load project settings");
      return;
    }

		nlohmann::json& trajectoryJson = json["trajectories"];

		for (const nlohmann::json& node : json["actions"])
		{
			Action* action = parseAction(node, trajectoryJson);
			if (action == 0)
			{
				setErr("load failed");
				reset();
				return;
			}
			action->id = global.actions.size();
			global.actions.push_back(action);
		}
		for (Action* action : global.actions)
		{
			if ((long long)action->parrent == -1)
				action->parrent = nullptr;
			else
				action->parrent = global.actions[(long long)action->parrent];

			if ((long long)action->prev == -1)
				action->prev = nullptr;
			else
				action->prev = global.actions[(long long)action->prev];

			if ((long long)action->next == -1)
				action->next = nullptr;
			else
				action->next = global.actions[(long long)action->next];

			if ((long long)action->actions == -1)
				action->actions = nullptr;
			else
				action->actions = global.actions[(long long)action->actions];
		}
		global.rootAction = global.actions[0];
		setNotif("loaded " + filename);
		return;
	}
err:
	setNotif("failed to load " + filename);
	reset();
	return;
}
