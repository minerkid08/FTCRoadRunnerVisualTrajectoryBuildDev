#pragma once

#include <glm/glm.hpp>
#include <cstring>
#include <vector>

#define NodePartOveride 1
#define NodePartMarker 2
#define NodePartDelay 3
#define NodePartTurn 4

struct NodePart{
	virtual void reset(){}
	virtual int getId(){return 0;}
};

struct Overides : public NodePart{
	int getId(){return 1;}
	bool vel = false;
	float velV = 0.0f;
	bool accel = false;
	float accelV = 0.0f;
	bool angVel = false;
	float angVelV = 0.0f;
	bool angAccel = false;
	float angAccelV = 0.0f;
	void reset(){
		vel = false;
		velV = 0.0f;
		accel = false;
		accelV = 0.0f;
		angVel = false;
		angVelV = 0.0f;
		angAccel = false;
		angAccelV = 0.0f;
	}
};

struct Marker : public NodePart{
	int getId(){return 2;}
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
	int getId(){return 3;}
	float time = 0;
	void reset(){
		time = 0;
	}
};

struct Turn : public NodePart{
	int getId(){return 4;}
	float angle;
	void reset(){
		angle = 0;
	}
};

struct PathNode{
	glm::vec2 pos;
	float rot = 0;
	int headingMode = 0;
	bool line = false;
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