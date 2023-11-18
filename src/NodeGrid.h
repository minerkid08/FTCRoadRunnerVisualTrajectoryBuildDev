#pragma once

#include <PathNode.h>
#include <Renderer.h>
#include <Texture.h>

class NodeGrid{
	public:
	NodeGrid();
	~NodeGrid();
	void update(Renderer renderer);

	void addNode(glm::vec2 pos);
	void removeNode(int ind);
	PathNode* getNode(int ind);
	
	void moveUp(int ind);
	void moveDown(int ind);

	private:
	PathNode* nodes;
	int nodeCount;
	const int maxNodes = 20;
	Texture circleTex;
	Texture colorTex;
};