#include "curve.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/trigonometric.hpp"
#include <cmath>

#define lerp(a, b, t) ((a) + (((b) - (a)) * (t)))


glm::vec3 getPosRR(glm::vec2 start, glm::vec2 end, float startTan, float endTan, float t)
{

	static float ctrlNodeDist = 12;
	endTan = endTan + 180.0f;

	startTan = glm::radians(startTan);
	endTan = glm::radians(endTan);
	glm::vec2 ctrl1 = {sin(startTan) * ctrlNodeDist + start.x, cos(startTan) * ctrlNodeDist + start.y};
	glm::vec2 ctrl2 = {sin(endTan) * ctrlNodeDist + end.x, cos(endTan) * ctrlNodeDist + end.y};

	float xa = lerp(start.x, ctrl1.x, t);
	float ya = lerp(start.y, ctrl1.y, t);
	float xb = lerp(ctrl1.x, ctrl2.x, t);
	float yb = lerp(ctrl1.y, ctrl2.y, t);
	float xc = lerp(ctrl2.x, end.x, t);
	float yc = lerp(ctrl2.y, end.y, t);

	// The Blue Line
	float xm = lerp(xa, xb, t);
	float ym = lerp(ya, yb, t);
	float xn = lerp(xb, xc, t);
	float yn = lerp(yb, yc, t);

	// The Black Dot
	glm::vec3 out;
	out.x = lerp(xm, xn, t);
	out.y = lerp(ym, yn, t);

	glm::vec2 tangentVec;
	tangentVec.x = lerp(xm, xn, t + 0.001) - out.x;
	tangentVec.y = lerp(ym, yn, t + 0.001) - out.y;
	out.z = atan2(tangentVec.x, tangentVec.y);
	return out;
}

glm::vec2 getPointRR(glm::vec2 start, glm::vec2 end, float startTan, float endTan, float t)
{
	static float ctrlNodeDist = 12;
	endTan = endTan + 180.0f;

	startTan = glm::radians(startTan);
	endTan = glm::radians(endTan);
	glm::vec2 ctrl1 = {sin(startTan) * ctrlNodeDist + start.x, cos(startTan) * ctrlNodeDist + start.y};
	glm::vec2 ctrl2 = {sin(endTan) * ctrlNodeDist + end.x, cos(endTan) * ctrlNodeDist + end.y};

	float xa = lerp(start.x, ctrl1.x, t);
	float ya = lerp(start.y, ctrl1.y, t);
	float xb = lerp(ctrl1.x, ctrl2.x, t);
	float yb = lerp(ctrl1.y, ctrl2.y, t);
	float xc = lerp(ctrl2.x, end.x, t);
	float yc = lerp(ctrl2.y, end.y, t);

	// The Blue Line
	float xm = lerp(xa, xb, t);
	float ym = lerp(ya, yb, t);
	float xn = lerp(xb, xc, t);
	float yn = lerp(yb, yc, t);

	// The Black Dot
	glm::vec2 out;
	out.x = lerp(xm, xn, t);
	out.y = lerp(ym, yn, t);
	return out;
}

float getLenRR(glm::vec2 start, glm::vec2 end, float startTan, float endTan)
{
	float dist = 0.0f;
	float dl = 1.0f / PointCount;
	for (int i = 0; i < PointCount; i++)
	{
		float t = (float)i / (float)PointCount;
		float t2 = t + dl;
		glm::vec2 p1 = getPointRR(start, end, startTan, endTan, t);
		glm::vec2 p2 = getPointRR(start, end, startTan, endTan, t2);
		dist += glm::distance(p1, p2);
	}
	return dist;
}
