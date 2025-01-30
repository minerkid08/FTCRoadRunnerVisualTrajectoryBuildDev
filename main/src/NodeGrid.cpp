#include "NodeGrid.hpp"
#include "global.hpp"

#define max(a, b) ((a) > (b) ? (a) : (b))

NodeGrid::NodeGrid() : nodes(maxNodes), segs(maxSegs)
{
}

NodeGrid::~NodeGrid()
{
}

void NodeGrid::update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int mods)
{
	if (selected.ind >= (selected.type == TypeNode ? nodes.count : segs.count))
	{
		selected.ind = nodes.count - 1;
	}
	if (nodes.count > 1)
	{
		for (int i = 0; i < segs.count; i++)
		{
			PathSegment* seg = segs.get(i);
			PathNode* node1 = nodes.get(seg->startNode);
			PathNode* node2 = nodes.get(seg->endNode);
			bool s = (selected.type == TypeSegment && selected.ind == i);
			renderer.drawSegment(node1->pos, node2->pos, s ? 1 : 0, seg->startTan, seg->endTan,
								 s ? glm::vec4(1.0, 0.0, 0.0, 1.0) : glm::vec4(0.5, 0.5, 0.5, 1.0));
		}
	}
	for (int i = 0; i < nodes.count; i++)
	{
		PathNode* node = nodes.get(i);
		bool s = (selected.type == TypeNode && selected.ind == i);
		renderer.drawNode({node->pos.x, node->pos.y, s ? 1 : 0}, node->heading,
						  s ? glm::vec4(1.0, 0.0, 0.0, 1.0) : glm::vec4(1.0, 1.0, 1.0, 1.0));
	}
	if (global.onViewport)
	{
		if (mods == 1)
		{
			float x = (mouseX - (float)(windowSize / 2)) / (windowSize / 2) * 72;
			float y = (mouseY - (float)(windowSize / 2)) / (windowSize / 2) * 72;

			if (gridSnap)
			{
				x = round(x / 6.0f) * 6;
				y = round(y / 6.0f) * -6;
			}
			else
			{
				y *= -1;
			}

			renderer.drawNode({x, y, 2}, 0, {0.5, 0.5, 0.5, 0.5});
		}
	}
}

void NodeGrid::mouseClick(int mouseX, int mouseY, int windowSize, int mods)
{
	float x = (mouseX - (float)(windowSize / 2)) / (windowSize / 2) * 72;
	float y = (mouseY - (float)(windowSize / 2)) / (windowSize / 2) * 72;
	if (gridSnap)
	{
		x = round(x / 6.0f) * 6;
		y = round(y / 6.0f) * -6;
	}
	else
	{
		y *= -1;
	}
	if (mods == 1)
	{
		PathNode* node = nodes.add();
		if (node == nullptr)
		{
			return;
		}
		node->pos = {x, y};
		resetNode(nodes.count - 1);
	}
	else
	{
		float closestDist = 100.0f;
		int closestInd = -1;
		int closestType = -1;
		for (int i = 0; i < nodes.count; i++)
		{
			PathNode* node = nodes.get(i);
			float dist = glm::distance(glm::vec2(x, y), glm::vec2(node->pos.x, node->pos.y));
			if (dist < closestDist)
			{
				closestDist = dist;
				closestInd = i;
				closestType = TypeNode;
			}
		}
		for (int i = 0; i < segs.count; i++)
		{
			PathSegment* seg = segs.get(i);
			PathNode* start = nodes.get(seg->startNode);
			PathNode* end = nodes.get(seg->endNode);
			glm::vec2 pos = {(start->pos.x + end->pos.x) / 2.0f, (start->pos.y + end->pos.y) / 2.0f};
			float dist = glm::distance(glm::vec2(x, y), glm::vec2(pos.x, pos.y));
			if (dist < closestDist)
			{
				closestDist = dist;
				closestInd = i;
				closestType = TypeSegment;
			}
		}
		if (closestDist < 12)
		{
			if (mods == 0)
			{
				selected.ind = closestInd;
				selected.type = closestType;
			}
			else if (selected.ind >= 0 && selected.type == TypeNode && closestType == TypeNode)
			{
				PathSegment* seg = segs.add();
				if (seg == nullptr)
				{
					return;
				}
				seg->startNode = selected.ind;
				seg->endNode = closestInd;
				seg->headingMode = 0;
				seg->pathType = 0;
				seg->startTan = 0;
				seg->endTan = 0;
				selected.ind = closestInd;
				selected.type = TypeNode;
			}
		}
	}
}

void NodeGrid::resetNode(int ind)
{
	PathNode* node = nodes.get(ind);
	node->heading = 0;
}

void NodeGrid::flipVert()
{
	nodes.foreach ([](int ind, PathNode* node) {
		node->pos.y *= -1;
		node->heading += 90;
		node->heading *= -1;
		node->heading -= 90;
	});
}

void NodeGrid::flipHoriz()
{
	nodes.foreach ([](int ind, PathNode* node) {
		node->pos.x *= -1;
		node->heading *= -1;
	});
}

void NodeGrid::reset()
{
	nodes.count = 0;
	segs.count = 0;
}
