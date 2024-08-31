#pragma once

#include "List.hpp"
#include "PathNode.hpp"
#include "PathSegment.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"

#include <string>

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
	std::string err = "";
	std::string msg = "";
	List<PathNode> nodes;
	List<PathSegment> segs;
	Selected selected;
	int mods = 0;
	int layer = -1;
  float otherLayerA = 0.5f;
  int recognitionId = 0;
	bool gridSnap = true;

	NodeGrid(Shader* shader);
	~NodeGrid();
	void update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int mods);
	void mouseClick(int mouseX, int mouseY, int windowSize, int mods);

	void resetNode(int i);
	void resetSegment(int i);

	void flipVert();
	void flipHoriz();

	void reset();

  private:
	Texture circleTex;
	Texture arrowTex;
	Texture arrowSquareTex;
	Shader* shader;

	void drawRotatedArrow(Renderer& renderer, glm::vec2 pos, float angle, glm::vec4 color = {1, 1, 1, 1});
};
