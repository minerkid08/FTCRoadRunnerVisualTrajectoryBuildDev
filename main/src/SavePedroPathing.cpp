#include "Save.hpp"
#include "glm/ext/vector_float2.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "json/json.hpp"
#include <iostream>

namespace PedroPathing
{
void saveTrajectory(const TrajectoryPedro* trajectory, nlohmann::json& j)
{
  j["globalVisibility"] = trajectory->visible;
	j["nodes"] = {};
	j["segments"] = {};
	for (int i2 = 0; i2 < trajectory->nodes.count; i2++)
	{
		const PathNode* node = trajectory->nodes.get(i2);
		nlohmann::json& nodeJson = j["nodes"][i2];
		nodeJson["x"] = node->pos.x;
		nodeJson["y"] = node->pos.y;
		nodeJson["h"] = node->heading;
	}
	for (int i2 = 0; i2 < trajectory->segs.count; i2++)
	{
		const PathSegment* segment = trajectory->segs.get(i2);
		nlohmann::json& segmentJson = j["segments"][i2];
		segmentJson["startNode"] = segment->startNode;
		segmentJson["endNode"] = segment->endNode;
		segmentJson["headingMode"] = segment->headingMode;
		segmentJson["ctrlPoints"] = {};
		for (int i3 = 0; i3 < segment->controlPointsCount; i3++)
		{
			glm::vec2 pos = segment->controlPoints[i3];
			segmentJson["ctrlPoints"][i3] = {};
			segmentJson["ctrlPoints"][i3]["x"] = pos.x;
			segmentJson["ctrlPoints"][i3]["y"] = pos.y;
		}
	}
}

TrajectoryPedro* parseTrajectory(const nlohmann::json& json, int ind)
{
	TrajectoryPedro* grid = new TrajectoryPedro();
	const nlohmann::json& traj = json[ind];
  if(traj.contains("globalVisibility"))
    grid->visible = traj["globalVisibility"];
	if (traj.contains("nodes"))
	{
		if (traj["nodes"].is_null())
			goto end;
		typeCheck(traj, "nodes", is_array);
		typeCheck(traj, "segments", is_array);
		for (const nlohmann::json& jnode : traj["nodes"])
		{
			PathNode* node = grid->nodes.add();
			tryGet(jnode, "x", is_number, node->pos.x);
			tryGet(jnode, "y", is_number, node->pos.y);
			tryGet(jnode, "h", is_number, node->heading);
		}

		for (const nlohmann::json& segmentJson : traj["segments"])
		{
			PathSegment* segment = grid->segs.add();
			tryGet(segmentJson, "startNode", is_number, segment->startNode);
			tryGet(segmentJson, "endNode", is_number, segment->endNode);
			tryGet(segmentJson, "headingMode", is_number, segment->headingMode);
			typeCheck(segmentJson, "ctrlPoints", is_array);
			for (const nlohmann::json& ctrlPoint : segmentJson["ctrlPoints"])
			{
				if (!ctrlPoint.is_object())
				{
					std::cerr << "ctrlPoint is not object\n";
					goto err;
				}
				glm::vec2* pos = &segment->controlPoints[segment->controlPointsCount++];
				tryGet(ctrlPoint, "x", is_number, pos->x);
				tryGet(ctrlPoint, "y", is_number, pos->y);
			}
		}
	}
  grid->canDelete = (grid->nodes.count > 0);
end:
	return grid;
err:
	delete grid;
	return 0;
}
} // namespace PedroPathing
