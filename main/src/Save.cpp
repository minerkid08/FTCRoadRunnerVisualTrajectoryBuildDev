#include "Save.hpp"
#include "actions/Action.hpp"
#include "trajectories/NodeGrid.hpp"
#include "global.hpp"

#include <fstream>
#include <json/json.hpp>

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
	std::vector<NodeGrid*> trajectories;
	nlohmann::json json;
	json["actions"] = {};
	json["trajectories"] = {};
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
		i++;
	}
	i = 0;
	for (NodeGrid* trajectory : trajectories)
	{
		nlohmann::json& j = json["trajectories"][i];
		j["nodes"] = {};
		j["segments"] = {};
		for (int i2 = 0; i2 < trajectory->nodes.count; i2++)
		{
			PathNode* node = trajectory->nodes.get(i2);
			nlohmann::json& nodeJson = j["nodes"][i2];
			nodeJson["x"] = node->pos.x;
			nodeJson["y"] = node->pos.y;
			nodeJson["h"] = node->heading;
		}
		for (int i2 = 0; i2 < trajectory->segs.count; i2++)
		{
			PathSegment* segment = trajectory->segs.get(i2);
			nlohmann::json& segmentJson = j["segments"][i2];
			segmentJson["startNode"] = segment->startNode;
			segmentJson["endNode"] = segment->endNode;
			segmentJson["startTangent"] = segment->startTan;
			segmentJson["endTangent"] = segment->endTan;
			segmentJson["headingMode"] = segment->headingMode;
		}
		i++;
	}
	std::ofstream stream(filename);
	stream << json.dump(2);
}

NodeGrid* parseTrajectory(const nlohmann::json& json, int ind)
{
	NodeGrid* grid = new NodeGrid();
	for (const nlohmann::json& jnode : json[ind]["nodes"])
	{
		PathNode* node = grid->nodes.add();
		node->pos.x = jnode["x"];
		node->pos.y = jnode["y"];
		node->heading = jnode["h"];
	}

	for (const nlohmann::json& segmentJson : json[ind]["segments"])
	{
		PathSegment* segment = grid->segs.add();
		segment->startNode = segmentJson["startNode"];
		segment->endNode = segmentJson["endNode"];
		segment->startTan = segmentJson["startTangent"];
		segment->endTan = segmentJson["endTangent"];
		segment->headingMode = segmentJson["headingMode"];
	}
	return grid;
}

void load(const std::string& filename)
{
  reset();
	std::ifstream stream(filename);
	nlohmann::json json;
	stream >> json;
	nlohmann::json& trajectoryJson = json["trajectories"];

	for (const nlohmann::json& node : json["actions"])
	{
		Action* action = new Action();
		action->type = node["type"];
		action->parrent = (Action*)(long long)node["parent"];
		action->next = (Action*)(long long)node["next"];
		action->prev = (Action*)(long long)node["prev"];
		action->actions = (Action*)(long long)node["actions"];
		if (action->type == ACTION_TRAJECTORY)
			action->data = parseTrajectory(trajectoryJson, node["trajectory"]);
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
}
