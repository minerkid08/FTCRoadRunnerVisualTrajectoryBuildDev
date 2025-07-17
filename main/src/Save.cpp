#include "Save.hpp"
#include "actions/Action.hpp"
#include "actions/CustomAction.hpp"
#include "global.hpp"
#include "trajectories/NodeGrid.hpp"
#include "ui/ui.hpp"

#include <fstream>
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
					j["fields"][f.name] = *(double*)&f.value;
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
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	if (json[key].type() != nlohmann::detail::value_t::type2)                                                          \
	{                                                                                                                  \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	out = json[key];

#define tryGetc(json, key, type2, out, err, cast)                                                                      \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	if (json[key].type() != nlohmann::detail::value_t::type2)                                                          \
	{                                                                                                                  \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	out = cast json[key];

#define typeCheck(json, key, type2, err)                                                                               \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}                                                                                                                  \
	if (json[key].type() != nlohmann::detail::value_t::type2)                                                          \
	{                                                                                                                  \
		delete err;                                                                                                    \
		return 0;                                                                                                      \
	}

#define typeCheck2(json, key, type2)                                                                                   \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		setErr("load failed");                                                                                         \
		reset();                                                                                                       \
		return;                                                                                                        \
	}                                                                                                                  \
	if (json[key].type() != nlohmann::detail::value_t::type2)                                                          \
	{                                                                                                                  \
		setErr("load failed");                                                                                         \
		reset();                                                                                                       \
		return;                                                                                                        \
	}

NodeGrid* parseTrajectory(const nlohmann::json& json, int ind)
{
	NodeGrid* grid = new NodeGrid();
	typeCheck(json, "nodes", object, grid);
	typeCheck(json, "segments", object, grid);
	for (const nlohmann::json& jnode : json[ind]["nodes"])
	{
		PathNode* node = grid->nodes.add();
		tryGet(jnode, "x", number_float, node->pos.x, grid);
		tryGet(jnode, "y", number_float, node->pos.y, grid);
		tryGet(jnode, "h", number_float, node->heading, grid);
	}

	for (const nlohmann::json& segmentJson : json[ind]["segments"])
	{
		PathSegment* segment = grid->segs.add();
		tryGet(segmentJson, "startNode", number_integer, segment->startNode, grid);
		tryGet(segmentJson, "endNode", number_integer, segment->endNode, grid);
		tryGet(segmentJson, "startTangent", number_float, segment->startTan, grid);
		tryGet(segmentJson, "endTangent", number_float, segment->endTan, grid);
		tryGet(segmentJson, "headingMode", number_integer, segment->headingMode, grid);
	}
	return grid;
}

Action* parseAction(const nlohmann::json& node, const nlohmann::json& trajectoryJson)
{
	Action* action = new Action();
	tryGet(node, "type", number_integer, action->type, action);
	tryGetc(node, "parent", number_integer, action->parrent, action, (Action*)(long long));
	tryGetc(node, "next", number_integer, action->next, action, (Action*)(long long));
	tryGetc(node, "prev", number_integer, action->prev, action, (Action*)(long long));
	tryGetc(node, "actions", number_integer, action->actions, action, (Action*)(long long));

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
		typeCheck(node, "fields", object, action);
		for (CustomActionField& field : *data)
		{
			switch (field.type)
			{
			case FIELDTYPE_INT: {
				long long v;
				tryGet(node["fields"], field.name, number_integer, v, action);
				field.value = (void*)v;
				break;
			}
			case FIELDTYPE_DOUBLE: {
				double v;
				tryGet(node["fields"], field.name, number_float, v, action);
				field.value = *(void**)&v;
				break;
			}
			case FIELDTYPE_BOOL: {
				bool v;
				tryGet(node["fields"], field.name, boolean, v, action);
				field.value = (void*)v;
				break;
			}
			case FIELDTYPE_STRING: {
				std::string v;
				tryGet(node["fields"], field.name, string, v, action);
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
	reset();
	std::ifstream stream(filename);
	nlohmann::json json;
	stream >> json;

	typeCheck2(json, "actions", array);

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
