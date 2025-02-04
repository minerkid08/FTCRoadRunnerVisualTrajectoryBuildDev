#include "Export.hpp"
#include "actions/Action.hpp"
#include "global.hpp"
#include "trajectories/NodeGrid.hpp"
#include "trajectories/PathNode.hpp"
#include "trajectories/PathSegment.hpp"
#include "utils.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#define rot(a) (-(a) - 90)

static bool exportTrajectory(NodeGrid* grid, std::string* string, int level)
{
	uint8_t* segUsage = new uint8_t[grid->nodes.count];
	memset(segUsage, 0, grid->nodes.count);
	for (int i = 0; i < grid->segs.count; i++)
	{
		PathSegment* s = grid->segs.get(i);
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
				std::cout << "export error: path has multiple start nodes\n";
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
		std::cout << "export warning: path has unused nodes\n";
	}
	if (startInd == -1)
	{
		std::cout << "export error: cant find start node\n";
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
			PathSegment* seg = grid->segs.get(i);
			if (seg->startNode == targetInd)
			{
				if (foundNode)
				{
					std::cout << "export error: fork found at node " << std::to_string((int)seg->startNode) << '\n';
					return true;
				}
				foundNode = true;
				foundInd = seg->endNode;
				segments.push_back(i);
			}
		}
		targetInd = foundInd;
	}

	PathNode* node = grid->nodes.get(startInd);

	*string += format("drive.actionBuilder(Pose2d(%.2f, %.2f, Math.toRadians(%.2f)))\n", node->pos.x, node->pos.y,
					  rot(node->heading));
	for (int i = 0; i < segments.size(); i++)
	{
		PathSegment* seg = grid->segs.get(segments[i]);
		node = grid->nodes.get(seg->endNode);
		for (int j = 0; j < level; j++)
			*string += '\t';
		*string += format(".setTangent(Math.toRadians(%.2f))\n", rot(seg->startTan));
		for (int j = 0; j < level; j++)
			*string += '\t';

		if (seg->headingMode == 0)
			*string += format(".splineTo(Vector2d(%.2f, %.2f), Math.toRadians(%.2f)))\n", node->pos.x, node->pos.y,
							  rot(seg->endTan + 180));
		if (seg->headingMode == 1)
			*string +=
				format(".splineToLinearHeading(Pose2d(%.2f, %.2f, Math.toRadians(%.2f)), Math.toRadians(%.2f)))\n",
					   node->pos.x, node->pos.y, rot(node->heading), rot(seg->endTan + 180));
		if (seg->headingMode == 2)
			*string += format(".splineToConstantHeading(Vector2d(%.2f, %.2f), Math.toRadians(%.2f)))\n", node->pos.x,
							  node->pos.y, rot(seg->endTan + 180));
	}
	for (int i = 0; i < level; i++)
		*string += '\t';
	*string += ".build(),\n";
	return false;
}

static void intExportAction(const Action* action, std::string* string, int level)
{
	const Action* a = action->actions;
	if (a == nullptr)
		return;
	while (true)
	{
		for (int i = 0; i < level; i++)
			*string += '\t';
		if (a->type == ACTION_PARALLEL)
		{
			*string += "ParallelAction(\n";
			intExportAction(a, string, level + 1);
			for (int i = 0; i < level; i++)
				*string += '\t';
			*string += "),\n";
		}
		else if (a->type == ACTION_SEQUENTIONAL)
		{
			*string += "SequentionalAction(\n";
			intExportAction(a, string, level + 1);
			for (int i = 0; i < level; i++)
				*string += '\t';
			*string += "),\n";
		}
		else if (a->type == ACTION_TRAJECTORY)
		{
			if (exportTrajectory(a->data, string, level + 1))
				return;
		}
		a = a->next;
		if (a == nullptr)
			return;
	}
}

void exportAction(const Action* action)
{
	std::string out;

	if (action->type == ACTION_PARALLEL)
	{
		out += "ParallelAction(\n";
		intExportAction(action, &out, 1);
		out += ");\n";
	}
	else if (action->type == ACTION_SEQUENTIONAL)
	{
		out += "SequentionalAction(\n";
		intExportAction(action, &out, 1);
		out += ");\n";
	}
	else if (action->type == ACTION_TRAJECTORY)
		out += "drive.actionBuilder().build();\n";
	std::string path = global.filename;
	if (path.find("\\") != std::string::npos)
	{
		path = path.substr(path.find("\\") + 1);
	}
	path = "export\\" + path;
	std::cout << path << '\n';
#ifndef __WIN64
  for(int i = 0; i < path.size(); i++)
  {
    if(path[i] == '\\')
      path[i] = '/';
  }
#endif
	std::ofstream stream(path);
	stream << out;
	stream.close();
}
