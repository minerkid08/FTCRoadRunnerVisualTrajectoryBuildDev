#pragma once

#include <PathNode.h>
#include <Renderer.h>
#include <Texture.h>

class NodeGrid{
	public:
	PathNode* nodes;
	int nodeCount;
	int selected = 0;
	int clickMode = 0;
	const int maxNodes = 32;
	int layer = -1;
	
	NodeGrid(Shader* shader);
	~NodeGrid();
	void update(Renderer& renderer);
	void mouseClick(int mouseX, int mouseY, int windowSize);

	void addNode(glm::vec2 pos);
	void removeNode(int ind);
	PathNode* getNode(int ind);
	
	void moveUp(int ind);
	void moveDown(int ind);

	private:
	Texture circleTex;
	Texture arrowTex;
	Shader* shader;
};