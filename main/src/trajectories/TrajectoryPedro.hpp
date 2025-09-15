#pragma once

#include "List.hpp"
#include "glm/ext/vector_float2.hpp"
#include "renderer/Renderer.hpp"
#include "trajectories/Trajectory.hpp"

#define Trajectory_Pedro 0

#define maxNodes 32
#define maxSegs 32

#define TypeNode 1
#define TypeSegment 2

#define HeadingModeConstant 2
#define HeadingModeLinear 2
#define HeadingModeTangent 2

namespace PedroPathing
{

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
	List<glm::vec2> controlPoints;
  PathSegment();
};

struct Selected
{
	int ind = -1;
	int type = 0;
};

class TrajectoryPedro : public Trajectory
{
  public:
	List<PathNode> nodes;
	List<PathSegment> segs;
	Selected selected;
	int mods = 0;
	bool gridSnap = true;

	TrajectoryPedro();
	~TrajectoryPedro();
	void update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int mods);
	void mouseClick(int mouseX, int mouseY, int windowSize, int mods);

	void render(Renderer& renderer, float transparency, float z, bool showSelected);

	void resetNode(int i);
	void resetSegment(int i);

	void flipVert();
	void flipHoriz();

	void reset();
};
} // namespace PedroPathing
