#pragma once

#include <glm/glm.hpp>
#include <cstring>
#include <vector>

#define SegPartOveride 1

struct SegPart{
	virtual void reset(){}
	virtual int getId(){return 0;}
};

struct Overides : public SegPart{
	int getId(){return SegPartOveride;}
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

struct PathSegment{
	int startNode;
	int endNode;
	int headingMode;
	int pathType;
	int layer;
	std::vector<SegPart*> parts;
	bool hasPart(int id){
		for(SegPart* part : parts){
			if(part->getId() == id){
				return true;
			}
		}
		return false;
	}
};