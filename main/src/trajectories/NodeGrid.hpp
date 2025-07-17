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

class NodeGrid
{
  public:
	List<PathNode> nodes;
	List<PathSegment> segs;
	Selected selected;
	int mods = 0;
	bool gridSnap = true;

	NodeGrid();
	~NodeGrid();
	void update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int mods);
	void mouseClick(int mouseX, int mouseY, int windowSize, int mods);

	void resetNode(int i);
	void resetSegment(int i);

	void flipVert();
	void flipHoriz();

	void reset();
};
