#include "preview.hpp"
#include "curve.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/trigonometric.hpp"
#include "projectSettings.hpp"
#include "renderer/Renderer.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"
#include "ui/ui.hpp"
#include <cstdio>

#define lerp(a, b, t) ((a) + (((b) - (a)) * (t)))

RobotPreview preview;

void generatePathPedro(PedroPathing::TrajectoryPedro* trajectory)
{
	uint8_t* segUsage = new uint8_t[trajectory->nodes.count];
	memset(segUsage, 0, trajectory->nodes.count);
	for (int i = 0; i < trajectory->segs.count; i++)
	{
		PedroPathing::PathSegment* s = trajectory->segs.get(i);
		segUsage[s->startNode] |= 1;
		segUsage[s->endNode] |= 2;
	}
	int startInd = -1;
	bool emptyNodes = false;
	for (int j = 0; j < trajectory->nodes.count; j++)
	{
		if (segUsage[j] == 1)
		{
			if (startInd == -1)
			{
				startInd = j;
			}
			else
			{
				setErr("path generation failed: path has multiple start nodes");
				return;
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
		setErr("path generation failed: no start node found");
		return;
	}

	std::vector<int> segments;
	int targetInd = startInd;
	bool foundNode = true;
	while (foundNode)
	{
		foundNode = false;
		int foundInd = 0;
		for (int i = 0; i < trajectory->segs.count; i++)
		{
			PedroPathing::PathSegment* seg = trajectory->segs.get(i);
			if (seg->startNode == targetInd)
			{
				if (foundNode)
				{
					setErr("path generation error: fork found at node " + std::to_string((int)seg->startNode));
					return;
				}
				foundNode = true;
				foundInd = seg->endNode;
				segments.push_back(i);
				printf("found seg %d\n", i);
			}
		}
		targetInd = foundInd;
	}

	preview.segments.resize(segments.size());

	int i2 = 0;
	float len = 0.0f;
	for (int i : segments)
	{
		PreviewPathSegment* seg = &preview.segments[i2++];
		seg->segment.pedro = trajectory->segs.get(i);
		seg->length =
			bezierLength<maxCtrlPts>(seg->segment.pedro->controlPoints, seg->segment.pedro->controlPointsCount);
		len += seg->length;
	}

	float i = 0.0f;
	for (PreviewPathSegment& seg : preview.segments)
	{
		seg.tStart = i / len;
		i += seg.length;
		seg.tEnd = i / len;
		seg.tMul = len / seg.length;
	}
	preview.trajectory.pedro = trajectory;
}

void generatePathRR(RoadRunner::TrajectoryRR* trajectory)
{
	uint8_t* segUsage = new uint8_t[trajectory->nodes.count];
	memset(segUsage, 0, trajectory->nodes.count);
	for (int i = 0; i < trajectory->segs.count; i++)
	{
		RoadRunner::PathSegment* s = trajectory->segs.get(i);
		segUsage[s->startNode] |= 1;
		segUsage[s->endNode] |= 2;
	}
	int startInd = -1;
	bool emptyNodes = false;
	for (int j = 0; j < trajectory->nodes.count; j++)
	{
		if (segUsage[j] == 1)
		{
			if (startInd == -1)
			{
				startInd = j;
			}
			else
			{
				setErr("path generation failed: path has multiple start nodes");
				return;
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
		setErr("path generation failed: no start node found");
		return;
	}

	std::vector<int> segments;
	int targetInd = startInd;
	bool foundNode = true;
	while (foundNode)
	{
		foundNode = false;
		int foundInd = 0;
		for (int i = 0; i < trajectory->segs.count; i++)
		{
			RoadRunner::PathSegment* seg = trajectory->segs.get(i);
			if (seg->startNode == targetInd)
			{
				if (foundNode)
				{
					setErr("path generation error: fork found at node " + std::to_string((int)seg->startNode));
					return;
				}
				foundNode = true;
				foundInd = seg->endNode;
				segments.push_back(i);
			}
		}
		targetInd = foundInd;
	}

	preview.segments.resize(segments.size());

	float len = 0.0f;
	for (int i : segments)
	{
		PreviewPathSegment* seg = &preview.segments[i];
		seg->segment.rr = trajectory->segs.get(i);
		RoadRunner::PathSegment* segment = seg->segment.rr;
		RoadRunner::PathNode* start = trajectory->nodes.get(segment->startNode);
		RoadRunner::PathNode* end = trajectory->nodes.get(segment->endNode);
		seg->length = getLenRR(start->pos, end->pos, segment->startTan, segment->endNode);
		len += seg->length;
	}

	float i = 0.0f;
	for (PreviewPathSegment& seg : preview.segments)
	{
		seg.tStart = i / len;
		i += seg.length;
		seg.tEnd = i / len;
		seg.tMul = len / seg.length;
		printf("seg: len %.2f, tStart %.2f, tEnd %.2f, tMul %.2f\n", seg.length, seg.tStart, seg.tEnd, seg.tMul);
	}
	preview.trajectory.rr = trajectory;
}

void updatePedro(PedroPathing::PathSegment* seg, float t)
{
	glm::vec2 pos = bezierPosition<maxCtrlPts>(seg->controlPoints, seg->controlPointsCount, t);
	float heading;
	if (seg->headingMode == HeadingModeTangent)
	{
		glm::vec2 tangent = bezierTangent<maxCtrlPts>(seg->controlPoints, seg->controlPointsCount, t);
		heading = -atan2(tangent.y, tangent.x) + (3.14159265f / 2.0f);
	}
	else
	{
		PedroPathing::PathNode* node = preview.trajectory.pedro->nodes.get(seg->startNode);
		if (seg->headingMode == HeadingModeConstant)
			heading = glm::radians(node->heading);
		if (seg->headingMode == HeadingModeLinear)
		{
			PedroPathing::PathNode* endNode = preview.trajectory.pedro->nodes.get(seg->endNode);
			heading = glm::radians(lerp(node->heading, endNode->heading, t));
		}
	}
	preview.curPos = {pos.x, pos.y, heading};
}

void updateRR(RoadRunner::PathSegment* seg, float t)
{
	RoadRunner::PathNode* start = preview.trajectory.rr->nodes.get(seg->startNode);
	RoadRunner::PathNode* end = preview.trajectory.rr->nodes.get(seg->endNode);
	preview.curPos = getPosRR(start->pos, end->pos, seg->startTan, seg->endTan, t);
	if (seg->headingMode == HeadingModeConstant)
		preview.curPos.z = glm::radians(start->heading);
	if (seg->headingMode == HeadingModeLinear)
		preview.curPos.z = glm::radians(lerp(start->heading, end->heading, t));
}

void updatePreview()
{
	if (preview.playing)
	{
		if (preview.playbackLength == 0)
			preview.playing = false;
		preview.t += preview.dt / preview.playbackLength;
		if (preview.t > 1)
			preview.t = 0;
	}
	if (preview.useSingleSegment)
	{
		float t = preview.t;
		if (projectSettings.pathType == Trajectory_Pedro)
		{
			PedroPathing::PathSegment* seg = preview.trajectory.pedro->segs.get(preview.singleSegmentId);
			updatePedro(seg, t);
		}
		if (projectSettings.pathType == Trajectory_RR)
		{
			RoadRunner::PathSegment* seg = preview.trajectory.rr->segs.get(preview.singleSegmentId);
			updateRR(seg, t);
		}
	}
	else
	{
		for (PreviewPathSegment& segment : preview.segments)
		{
			if (segment.tStart <= preview.t && preview.t <= segment.tEnd)
			{
				float t = (preview.t - segment.tStart) * segment.tMul;
				if (projectSettings.pathType == Trajectory_Pedro)
				{
					PedroPathing::PathSegment* seg = segment.segment.pedro;
					updatePedro(seg, t);
				}
				if (projectSettings.pathType == Trajectory_RR)
				{
					RoadRunner::PathSegment* seg = segment.segment.rr;
					updateRR(seg, t);
				}
				break;
			}
		}
	}
}

void drawPreview(Renderer& renderer)
{
	renderer.drawRobot(preview.curPos, preview.sizeX, preview.sizeY);
}
