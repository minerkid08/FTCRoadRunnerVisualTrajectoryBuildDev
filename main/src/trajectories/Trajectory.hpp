#pragma once

#include "List.hpp"
#include "PathNode.hpp"
#include "PathSegment.hpp"
#include "renderer/Renderer.hpp"

#define maxNodes 32
#define maxSegs 32

#define TypeNode 1
#define TypeSegment 2

struct Selected
{
	int ind = -1;
	int type = 0;
};

class Trajectory
{
  public:
	List<PathNode> nodes;
	List<PathSegment> segs;
	Selected selected;
	int mods = 0;
	bool gridSnap = true;
	bool visible = false;

	Trajectory();
	~Trajectory();
	void update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int mods);
	void mouseClick(int mouseX, int mouseY, int windowSize, int mods);

	void render(Renderer& renderer, float transparency, float z, bool showSelected);

	void resetNode(int i);
	void resetSegment(int i);

	void flipVert();
	void flipHoriz();

	void reset();
};
