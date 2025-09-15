#include "TrajectoryPedro.hpp"
#include "global.hpp"
#include "preview.hpp"

#define max(a, b) ((a) > (b) ? (a) : (b))

#define zoff(s) (s ? 0.1 : 0)

namespace PedroPathing
{
PathSegment::PathSegment() : controlPoints(8)
{
}

TrajectoryPedro::TrajectoryPedro() : nodes(maxNodes), segs(maxSegs)
{
}

TrajectoryPedro::~TrajectoryPedro()
{
}

void TrajectoryPedro::render(Renderer& renderer, float a, float z, bool showSelected)
{
	if (nodes.count > 1)
	{
		for (int i = 0; i < segs.count; i++)
		{
			PathSegment* seg = segs.get(i);
			bool s = (selected.type == TypeSegment && selected.ind == i && showSelected);
			renderer.drawSegment(z + zoff(s), seg->controlPoints,
								 s ? glm::vec4(1.0, 0.0, 0.0, a) : glm::vec4(0.5, 0.5, 0.5, a));
      if(s)
        renderer.drawControlPoints(seg->controlPoints, zoff(s) + 0.1);
		}
	}

	for (int i = 0; i < nodes.count; i++)
	{
		PathNode* node = nodes.get(i);
		bool s = (selected.type == TypeNode && selected.ind == i && showSelected);
		renderer.drawNode({node->pos.x, node->pos.y, s ? 1 : 0}, node->heading,
						  s ? glm::vec4(1.0, 0.0, 0.0, a) : glm::vec4(1.0, 1.0, 1.0, a));
	}
  if(preview.active && preview.running)
    drawPreview(renderer);
}

void TrajectoryPedro::update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int mods)
{
  preview.running = (selected.type == TypeSegment);
	if(selected.type == TypeSegment && preview.active)
  {
    PathSegment* seg = segs.get(selected.ind);
    preview.ctrlPts = &seg->controlPoints;
    updatePreview();
  }
	if (selected.ind >= (selected.type == TypeNode ? nodes.count : segs.count))
		selected.ind = nodes.count - 1;
	render(renderer, 1.0f, 0.9f, true);
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

void TrajectoryPedro::mouseClick(int mouseX, int mouseY, int windowSize, int mods)
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
					return;
				seg->startNode = selected.ind;
				seg->endNode = closestInd;
				seg->headingMode = 0;

				PathNode* start = nodes.get(seg->startNode);
				PathNode* end = nodes.get(seg->endNode);
				*seg->controlPoints.add() = start->pos;
				*seg->controlPoints.add() = end->pos;

				selected.ind = closestInd;
				selected.type = TypeNode;
			}
		}
	}
}

void TrajectoryPedro::resetNode(int ind)
{
	PathNode* node = nodes.get(ind);
	node->heading = 0;
}

void TrajectoryPedro::flipVert()
{
	nodes.foreach ([](int ind, PathNode* node) {
		node->pos.y *= -1;
		node->heading += 90;
		node->heading *= -1;
		node->heading -= 90;
	});
}

void TrajectoryPedro::flipHoriz()
{
	nodes.foreach ([](int ind, PathNode* node) {
		node->pos.x *= -1;
		node->heading *= -1;
	});
}

void TrajectoryPedro::reset()
{
	nodes.count = 0;
	segs.count = 0;
}
} // namespace PedroPathing
