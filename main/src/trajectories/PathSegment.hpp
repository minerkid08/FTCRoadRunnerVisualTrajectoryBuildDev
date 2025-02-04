#pragma once

#include <glm/glm.hpp>

struct PathSegment
{
	int startNode;
	int endNode;
	int headingMode;
  float startTan;
  float endTan;
};
