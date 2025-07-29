#include "Save.hpp"
#include "actions/Action.hpp"
#include "actions/CustomAction.hpp"
#include "global.hpp"
#include "trajectories/NodeGrid.hpp"
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
		if (action->type > 2)
		{
			j["fields"] = {};
			std::vector<CustomActionField>* fields = (std::vector<CustomActionField>*)action->data;
			for (CustomActionField& f : *fields)
			{
				switch (f.type)
				{
				case FIELDTYPE_INT:
					j["fields"][f.name] = *(int*)&f.value;
					break;
				case FIELDTYPE_DOUBLE:
					j["fields"][f.name] = *(float*)&f.value;
					break;
				case FIELDTYPE_BOOL:
					j["fields"][f.name] = *(bool*)&f.value;
					break;
				case FIELDTYPE_STRING:
					j["fields"][f.name] = (char*)f.value;
					break;
				}
			}
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
	setNotif("saved " + filename);
}

#define tryGet(json, key, type2, out, err)                                                                             \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		std::cerr << "cant get key: " << key << '\n';                                                                  \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	if (!json[key].type2())                                                                                            \
	{                                                                                                                  \
		std::cerr << "wrong type for key: " << key << " expected " << json[key].type_name() << '\n';                   \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	out = json[key];

#define tryGetc(json, key, type2, out, err, cast)                                                                      \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		std::cerr << "cant get key: " << key << '\n';                                                                  \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	if (!json[key].type2())                                                                                            \
	{                                                                                                                  \
		std::cerr << "wrong type for key: " << key << " expected " << json[key].type_name() << '\n';                   \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	out = cast json[key];

#define typeCheck(json, key, type2, err)                                                                               \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		std::cerr << "cant get key: " << key << '\n';                                                                  \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	if (!json[key].type2())                                                                                            \
	{                                                                                                                  \
		std::cerr << "wrong type for key: " << key << " expected " << json[key].type_name() << '\n';                   \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}

#define typeCheck2(json, key, type2)                                                                                   \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		std::cerr << "cant get key: " << key << '\n';                                                                  \
		setErr("load failed");                                                                                         \
		reset();                                                                                                       \
		return;                                                                                                        \
	}                                                                                                                  \
	if (!json[key].type2())                                                                                            \
	{                                                                                                                  \
		std::cerr << "wrong type for key: " << key << " expected " << json[key].type_name() << '\n';                   \
		setErr("load failed");                                                                                         \
		reset();                                                                                                       \
		return;                                                                                                        \
	}

NodeGrid* parseTrajectory(const nlohmann::json& json, int ind)
{
	NodeGrid* grid = new NodeGrid();
	const nlohmann::json& traj = json[ind];
	typeCheck(traj, "nodes", is_array, grid);
	typeCheck(traj, "segments", is_array, grid);
	for (const nlohmann::json& jnode : traj["nodes"])
	{
		PathNode* node = grid->nodes.add();
		tryGet(jnode, "x", is_number, node->pos.x, grid);
		tryGet(jnode, "y", is_number, node->pos.y, grid);
		tryGet(jnode, "h", is_number, node->heading, grid);
	}

	for (const nlohmann::json& segmentJson : traj["segments"])
	{
		PathSegment* segment = grid->segs.add();
		tryGet(segmentJson, "startNode", is_number, segment->startNode, grid);
		tryGet(segmentJson, "endNode", is_number, segment->endNode, grid);
		tryGet(segmentJson, "startTangent", is_number, segment->startTan, grid);
		tryGet(segmentJson, "endTangent", is_number, segment->endTan, grid);
		tryGet(segmentJson, "headingMode", is_number, segment->headingMode, grid);
	}
	return grid;
}

Action* parseAction(const nlohmann::json& node, const nlohmann::json& trajectoryJson)
{
	Action* action = new Action();
	tryGet(node, "type", is_number, action->type, action);
	tryGetc(node, "parent", is_number, action->parrent, action, (Action*)(long long));
	tryGetc(node, "next", is_number, action->next, action, (Action*)(long long));
	tryGetc(node, "prev", is_number, action->prev, action, (Action*)(long long));
	tryGetc(node, "actions", is_number, action->actions, action, (Action*)(long long));

	if (action->type == ACTION_TRAJECTORY)
	{
		action->data = parseTrajectory(trajectoryJson, node["trajectory"]);
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
		typeCheck(node, "fields", is_object, action);
		for (CustomActionField& field : *data)
		{
			switch (field.type)
			{
			case FIELDTYPE_INT: {
				long long v;
				tryGet(node["fields"], field.name, is_number, v, action);
				field.value = (void*)v;
				break;
			}
			case FIELDTYPE_DOUBLE: {
				float v;
				tryGet(node["fields"], field.name, is_number, v, action);
				field.value = *(void**)&v;
				break;
			}
			case FIELDTYPE_BOOL: {
				bool v;
				tryGet(node["fields"], field.name, is_boolean, v, action);
				field.value = (void*)v;
				break;
			}
			case FIELDTYPE_STRING: {
				std::string v;
				tryGet(node["fields"], field.name, is_string, v, action);
				strcpy((char*)field.value, v.c_str());
				break;
			}
			}
		}
	}
	return action;
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

	typeCheck2(json, "actions", is_array);

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
	setNotif("loaded" + filename);
}
