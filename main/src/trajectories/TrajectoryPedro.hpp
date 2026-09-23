#pragma once

#include "List.hpp"
#include "glm/ext/vector_float2.hpp"
#include "renderer/Renderer.hpp"
#include "trajectories/Trajectory.hpp"
#include <array>

#define Trajectory_Pedro 0

#define maxNodes 32
#define maxSegs 32
#define maxCtrlPts 8

#define TypeNode 1
#define TypeSegment 2

#define HeadingModeConstant 2
#define HeadingModeLinear 1
#define HeadingModeTangent 0

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
  int controlPointsCount = 0;
  std::array<glm::vec2, maxCtrlPts> controlPoints;
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
	void update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int mods, float dt);
	void mouseClick(int mouseX, int mouseY, int windowSize, int mods);

	void render(Renderer& renderer, float transparency, float z, bool showSelected);

	void resetNode(int i);
	void resetSegment(int i);

	void flipVert();
	void flipHoriz();

	void reset();
};
} // namespace PedroPathing
