#pragma once

#include <glm/glm.hpp>
#include <cstring>
#include <vector>

#define NodePartMarker 2
#define NodePartDelay 3
#define NodePartTurn 4

struct NodePart{
	virtual void reset(){}
	virtual int getId(){return 0;}
};

struct Marker : public NodePart{
	int getId(){return NodePartMarker;}
	char* text;
	Marker(){
		text = new char[255];
		reset();
	}
	~Marker(){
		delete text;
	}
	void reset(){
		memset(text, 0, 255);
	}
};

struct Delay : public NodePart{
	int getId(){return NodePartDelay;}
	float time = 0;
	void reset(){
		time = 0;
	}
};

struct Turn : public NodePart{
	int getId(){return NodePartTurn;}
	float angle;
	void reset(){
		angle = 0;
	}
};

struct PathNode{
	glm::vec2 pos;
	float rot = 0;
	float heading = 0;
	int layer;
	std::vector<NodePart*> parts;
	bool hasPart(int id){
		for(NodePart* part : parts){
			if(part->getId() == id){
				return true;
			}
		}
		return false;
	}
};