#include <NodeGrid.h>

NodeGrid::NodeGrid() : circleTex("circle.png"), colorTex("color.png"){
	nodes = new PathNode[maxNodes]();
	nodeCount = 0;
}

NodeGrid::~NodeGrid(){
	delete nodes;
}

void NodeGrid::update(Renderer renderer){
	for(int i = 0; i < nodeCount; i++){
		PathNode* node = (nodes + i);
		renderer.draw(node->pos, 0.1f, &circleTex, {1,1,1,1});
	}
}

void NodeGrid::addNode(glm::vec2 pos){
	(nodes + nodeCount + 1)->pos = pos;
	nodeCount++;
}

void NodeGrid::removeNode(int ind){

}

void NodeGrid::moveUp(int ind){

}

void NodeGrid::moveDown(int ind){

}