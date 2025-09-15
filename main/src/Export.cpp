#include "Export.hpp"
#include "actions/Action.hpp"
#include "actions/CustomAction.hpp"
#include "global.hpp"
#include "settings.hpp"
#include "trajectories/TrajectoryRR.hpp"
#include "ui/ui.hpp"
#include "utils.hpp"
#include "json/json.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define rot(a) (-(a) - 90)

static bool exportTrajectoryRR(RoadRunner::TrajectoryRR* grid, std::string* string, int level, nlohmann::json& lang)
{
	uint8_t* segUsage = new uint8_t[grid->nodes.count];
	memset(segUsage, 0, grid->nodes.count);
	for (int i = 0; i < grid->segs.count; i++)
	{
    RoadRunner::PathSegment* s = grid->segs.get(i);
		segUsage[s->startNode] |= 1;
		segUsage[s->endNode] |= 2;
	}
	int startInd = -1;
	bool emptyNodes = false;
	for (int j = 0; j < grid->nodes.count; j++)
	{
		if (segUsage[j] == 1)
		{
			if (startInd == -1)
			{
				startInd = j;
			}
			else
			{
				setErr("export failed: path has multiple start nodes");
				return false;
			}
		}
		if (segUsage[j] == 0)
		{
			emptyNodes = true;
		}
	}
	if (emptyNodes)
	{
		setWarn("warning: path has unused nodes");
	}
	if (startInd == -1)
	{
		setErr("export failed: no start node found");
		return false;
	}

	std::vector<int> segments;
	int targetInd = startInd;
	bool foundNode = true;
	while (foundNode)
	{
		foundNode = false;
		int foundInd = 0;
		for (int i = 0; i < grid->segs.count; i++)
		{
      RoadRunner::PathSegment* seg = grid->segs.get(i);
			if (seg->startNode == targetInd)
			{
				if (foundNode)
				{
					setErr("export failed: fork found at node " + std::to_string((int)seg->startNode));
					return true;
				}
				foundNode = true;
				foundInd = seg->endNode;
				segments.push_back(i);
			}
		}
		targetInd = foundInd;
	}

  RoadRunner::PathNode* node = grid->nodes.get(startInd);

	nlohmann::json& actionLang = lang["trajectory"];

	if (actionLang.contains("indent"))
	{
		if (actionLang["indent"] == false)
			level--;
	}

	*string += fmt(actionLang["start"], node->pos.x, node->pos.y, rot(node->heading));
	*string += '\n';
	for (int i = 0; i < segments.size(); i++)
	{
    RoadRunner::PathSegment* seg = grid->segs.get(segments[i]);
		node = grid->nodes.get(seg->endNode);
		for (int j = 0; j < level; j++)
			*string += '\t';
		*string += fmt(actionLang["setTangent"], rot(seg->startTan));
		*string += '\n';
		for (int j = 0; j < level; j++)
			*string += '\t';

		if (seg->headingMode == 0)
		{
			*string += fmt(actionLang["splineTo"], node->pos.x, node->pos.y, rot(seg->endTan + 180));
			*string += '\n';
		}
		if (seg->headingMode == 1)
		{
			*string += fmt(actionLang["splineToLinearHeading"], node->pos.x, node->pos.y, rot(node->heading),
							  rot(seg->endTan + 180));
			*string += '\n';
		}
		if (seg->headingMode == 2)
		{
			*string += fmt(actionLang["splineToConstantHeading"], node->pos.x, node->pos.y, rot(seg->endTan + 180));
			*string += '\n';
		}
	}
	for (int i = 0; i < level; i++)
		*string += '\t';
	*string += actionLang["build"];
	return false;
}

static void intExportAction(const Action* action, std::string* string, int level, nlohmann::json& lang)
{
	const char* containerName = global.actionTypes[action->type];
	nlohmann::json& containerLang = lang[containerName];
	const Action* a = action->actions;
	if (a == nullptr)
		return;
	while (true)
	{
		if (containerLang.contains("add"))
		{
			for (int i = 0; i < level - 1; i++)
				*string += '\t';
			*string += containerLang["add"];
			*string += '\n';
		}

		for (int i = 0; i < level; i++)
			*string += '\t';

		if (a->type == ACTION_TRAJECTORY)
		{
			if (exportTrajectoryRR((RoadRunner::TrajectoryRR*)a->data, string, level + 1, lang))
				return;
		}
		else if (a->type > 2)
		{
			std::vector<CustomActionField>* data = (std::vector<CustomActionField>*)a->data;
			std::string args;
			int l = 0;
			nlohmann::json& actionLang = lang["custom-action"];
			for (CustomActionField& f : *data)
			{
				switch (f.type)
				{
				case FIELDTYPE_INT:
					args += fmt(actionLang["int"], f.value.i);
					break;
				case FIELDTYPE_DOUBLE:
					args += fmt(actionLang["double"], f.value.f);
					break;
				case FIELDTYPE_BOOL:
					args += fmt(actionLang["bool"], f.value.b ? "true" : "false");
					break;
				case FIELDTYPE_STRING:
					args += fmt(actionLang["string"], f.value.s);
					break;
				}
				if (l < data->size() - 1)
					args += ", ";
				l++;
			}
			*string += fmt(lang["custom-action"]["format"], global.actionTypes[a->type], args.c_str());
		}
		else
		{
			const char* actionName = global.actionTypes[a->type];
			nlohmann::json& actionLang = lang[actionName];

			*string += actionLang["start"];
			*string += '\n';

			intExportAction(a, string, level + 1, lang);

			if (actionLang.contains("build"))
			{
				for (int i = 0; i < level; i++)
					*string += '\t';
				*string += actionLang["build"];
				*string += '\n';
			}
			for (int i = 0; i < level; i++)
				*string += '\t';
			*string += actionLang["end"];
			*string += '\n';
		}

		if (containerLang.contains("addEnd"))
		{
			*string += '\n';
			for (int i = 0; i < level - 1; i++)
				*string += '\t';
			*string += containerLang["addEnd"];
			*string += '\n';
		}
		else
		{
			*string += ",\n";
		}

		a = a->next;
		if (a == nullptr)
			return;
	}
}

void exportAction(const Action* action)
{
  clearMsg();
	std::string out;

	std::ifstream langIfStream(fmt("lang/%s.json", global.languages[settings.language]));
	std::stringstream langSstream;
	langSstream << langIfStream.rdbuf();
	nlohmann::json lang = nlohmann::json::parse(langSstream.str());

	const char* actionName = global.actionTypes[action->type];
	nlohmann::json& actionLang = lang[actionName];
	out += actionLang["start"];
	out += '\n';
	intExportAction(action, &out, 1, lang);
	if (actionLang.contains("build"))
	{
		out += actionLang["build"];
		out += '\n';
	}
	if (actionLang.contains("end"))
	{
		out += lang[actionName]["end"];
		out += '\n';
	}

	std::string path = global.filename;

	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == '\\')
			path[i] = '/';
	}

	if (path.find("/") != std::string::npos)
	{
		path = path.substr(strlen(settings.savePath) + 1);
	}
	std::string prefix = settings.exportPath;
	path = prefix + '/' + path;
	std::string s = path.substr(0, path.find_last_of('/'));
	if (!std::filesystem::exists(s))
		std::filesystem::create_directories(s);
	std::ofstream stream(path);
	stream << out;
	stream.close();

  setNotif("exported to " + path);
}
