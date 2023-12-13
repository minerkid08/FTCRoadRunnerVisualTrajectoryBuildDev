#pragma once

#include <PathNode.h>
#include <Renderer.h>
#include <Texture.h>

class NodeGrid{
	public:
	PathNode* nodes;
	int nodeCount;
	int selected = -1;
	const int maxNodes = 32;
	int layer = -1;
	
	NodeGrid(Shader* shader);
	~NodeGrid();
	void update(Renderer& renderer, int mouseX, int mouseY, int windowSize, bool shiftDown);
	void mouseClick(int mouseX, int mouseY, int windowSize, bool shiftDown);

	void addNode(glm::vec2 pos);
	void removeNode(int ind);
	PathNode* getNode(int ind);
	
	void moveUp(int ind);
	void moveDown(int ind);

	void flipVert();
	void flipHoriz();

	void reset();

	PathNode* operator[](int i){
		return nodes + i;
	}

	private:
	Texture circleTex;
	Texture arrowTex;
	Texture arrowSquareTex;
	Shader* shader;
};