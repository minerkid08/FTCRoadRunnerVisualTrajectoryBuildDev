#pragma once

#include <glm/glm.hpp>

struct Overides{
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

struct PathNode{
	glm::vec2 pos;
	float rot = 0;
	int headingMode = 0;
	bool turnAfterMove = false;
	bool marker = false;
	bool line = false;
	int layer;
	Overides overides;
};