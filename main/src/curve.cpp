#include "curve.hpp"
#include "List.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <cmath>

#define lerp(a, b, t) ((a) + (((b) - (a)) * (t)))

long long combinations(int n, int k)
{
	if (k < 0 || k > n)
		return 0;
	if (k == 0 || k == n)
		return 1;
	if (k > n / 2)
		k = n - k;
	long long res = 1;
	for (int i = 1; i <= k; ++i)
		res = res * (n - i + 1) / i;
	return res;
}

double bernstein_polynomial(int n, int i, float t)
{
	return combinations(n, i) * pow(t, i) * pow(1 - t, n - i);
}

float nCr(int n, int r)
{
	if (r < 0 || r > n)
		return 0;
	if (r == 0 || r == n)
		return 1;
	if (r > n / 2)
		r = n - r;
	float res = 1;
	for (int i = 1; i <= r; ++i)
		res = res * (n - i + 1) / i;
	return res;
}

float bernstein(int n, int i, float t)
{
	return nCr(n, i) * pow(1 - t, n - i) * pow(t, i);
}

float bernstein_derivative(int n, int i, float t)
{
	if (n == 0)
		return 0;
	return n * (bernstein(n - 1, i - 1, t) - bernstein(n - 1, i, t));
}

glm::vec2 bezierTangent(const List<glm::vec2>* controlPoints, float t)
{
	glm::vec2 tangent = {0, 0};
	int n = controlPoints->count - 1;
	for (int i = 0; i <= n; ++i)
	{
		double derivative_val = bernstein_derivative(n, i, t);
		tangent.x += derivative_val * controlPoints->get(i)->x;
		tangent.y += derivative_val * controlPoints->get(i)->y;
	}
	return glm::normalize(tangent);
}

glm::vec2 bezierPosition(const List<glm::vec2>* controlPoints, float t)
{
	glm::vec2 pos = {0, 0};
	int n = controlPoints->count - 1;
	for (int i = 0; i <= n; ++i)
	{
		double weight = bernstein_polynomial(n, i, t);
		pos.x += weight * controlPoints->get(i)->x;
		pos.y += weight * controlPoints->get(i)->y;
	}
	return pos;
}

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
