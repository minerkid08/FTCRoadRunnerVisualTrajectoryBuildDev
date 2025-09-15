#pragma once

#include "List.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

glm::vec3 getPosRR(glm::vec2 start, glm::vec2 end, float startTan, float endTan, float t);

glm::vec2 bezierTangent(const List<glm::vec2>* controlPoints, float t);
glm::vec2 bezierPosition(const List<glm::vec2>* controlPoints, float t);
