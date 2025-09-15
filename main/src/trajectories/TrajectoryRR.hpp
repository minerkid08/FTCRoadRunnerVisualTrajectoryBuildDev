#pragma once

#include "List.hpp"
#include "renderer/Renderer.hpp"
#include "trajectories/Trajectory.hpp"
#include <glm/glm.hpp>

#define Trajectory_RR 1

#define maxNodes 32
#define maxSegs 32

#define TypeNode 1
#define TypeSegment 2

namespace RoadRunner
{
struct Selected
{
	int ind = -1;
	int type = 0;
};

struct PathNode
{
	glm::vec2 pos;
	float heading = 0;
};

struct PathSegment
{
	int startNode;
	int endNode;
	int headingMode;
  float startTan;
  float endTan;
};

class TrajectoryRR : public Trajectory
{
  public:
	List<PathNode> nodes;
	List<PathSegment> segs;
	Selected selected;
	int mods = 0;
	bool gridSnap = true;

	TrajectoryRR();
	~TrajectoryRR();
	void update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int mods);
	void mouseClick(int mouseX, int mouseY, int windowSize, int mods);

	void render(Renderer& renderer, float transparency, float z, bool showSelected);

	void resetNode(int i);
	void resetSegment(int i);

	void flipVert();
	void flipHoriz();

	void reset();
};
} // namespace RoadRunner
