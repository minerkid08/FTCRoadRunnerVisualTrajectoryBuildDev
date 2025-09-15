#include "trajectories/TrajectoryRR.hpp"
#include "json/json.hpp"
#include <iostream>
#include "Save.hpp"
namespace RoadRunner
{
void saveTrajectory(const TrajectoryRR* trajectory, nlohmann::json& j)
{
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
		segmentJson["startTangent"] = segment->startTan;
		segmentJson["endTangent"] = segment->endTan;
		segmentJson["headingMode"] = segment->headingMode;
	}
}
TrajectoryRR* parseTrajectory(const nlohmann::json& json, int ind)
{
	TrajectoryRR* grid = new TrajectoryRR();
	const nlohmann::json& traj = json[ind];
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
		tryGet(segmentJson, "startTangent", is_number, segment->startTan);
		tryGet(segmentJson, "endTangent", is_number, segment->endTan);
		tryGet(segmentJson, "headingMode", is_number, segment->headingMode);
	}
	return grid;
err:
	delete grid;
	return 0;
}
} // namespace RoadRunner
