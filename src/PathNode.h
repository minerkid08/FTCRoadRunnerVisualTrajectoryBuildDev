#pragma once

#include <glm/glm.hpp>

struct PathNode{
	glm::vec2 pos;
	float rot = 0;
	bool turnAfterMove = false;
	bool marker = false;
	int layer;
};