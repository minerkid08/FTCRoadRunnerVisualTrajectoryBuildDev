#pragma once

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <array>

glm::vec3 getPosRR(glm::vec2 start, glm::vec2 end, float startTan, float endTan, float t);
glm::vec2 getPointRR(glm::vec2 start, glm::vec2 end, float startTan, float endTan, float t);
float getLenRR(glm::vec2 start, glm::vec2 end, float startTan, float endTan);

inline long long combinations(int n, int k)
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

inline double bernstein_polynomial(int n, int i, float t)
{
	return combinations(n, i) * pow(t, i) * pow(1 - t, n - i);
}

inline float nCr(int n, int r)
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

inline float bernstein(int n, int i, float t)
{
	return nCr(n, i) * pow(1 - t, n - i) * pow(t, i);
}

inline float bernstein_derivative(int n, int i, float t)
{
	if (n == 0)
		return 0;
	return n * (bernstein(n - 1, i - 1, t) - bernstein(n - 1, i, t));
}

template <int len> glm::vec2 bezierTangent(const std::array<glm::vec2, len>& controlPoints, int num, float t)
{
	glm::vec2 tangent = {0, 0};
	int n = num- 1;
	for (int i = 0; i <= n; ++i)
	{
		double derivative_val = bernstein_derivative(n, i, t);
		tangent.x += derivative_val * controlPoints[i].x;
		tangent.y += derivative_val * controlPoints[i].y;
	}
	return glm::normalize(tangent);
}

template <int len> glm::vec2 bezierPosition(const std::array<glm::vec2, len>& controlPoints, int num, float t)
{
	glm::vec2 pos = {0, 0};
	int n = num- 1;
	for (int i = 0; i <= n; ++i)
	{
		double weight = bernstein_polynomial(n, i, t);
		pos.x += weight * controlPoints[i].x;
		pos.y += weight * controlPoints[i].y;
	}
	return pos;
}

#define PointCount 5
template <int len> float bezierLength(const std::array<glm::vec2, len>& controlPoints, int num)
{
	float dist = 0.0f;
	float dl = 1.0f / PointCount;
	for (int i = 0; i < PointCount; i++)
	{
		float t = (float)i / (float)PointCount;
		float t2 = t + dl;
		glm::vec2 p1 = bezierPosition<len>(controlPoints, num, t);
		glm::vec2 p2 = bezierPosition<len>(controlPoints, num, t2);
		dist += glm::distance(p1, p2);
	}
	return dist;
}
