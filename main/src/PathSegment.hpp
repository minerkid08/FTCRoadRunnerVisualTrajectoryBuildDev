#pragma once

#include <glm/glm.hpp>

struct PathSegment
{
	int startNode;
	int endNode;
	int headingMode;
	int pathType;
  float startTan;
  float endTan;
};
