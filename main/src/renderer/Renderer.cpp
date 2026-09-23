#include "Renderer.hpp"
#include "curve.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <array>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

static float lerp(float a, float b, float t)
{
	return a + ((b - a) * t);
}

static int inds[] = {0, 1, 2, 2, 1, 3};

static glm::vec2 uv[4] = {
	{1, 1},
	{1, 0},
	{0, 1},
	{0, 0},
};

Renderer::Renderer()
{
	// va
	glGenVertexArrays(1, &va);
	glBindVertexArray(va);

	// vb
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 2));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 4));

	// ib
	glGenBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);

	camMat = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
}

Renderer::~Renderer()
{
	del();
}

void Renderer::del()
{
	if (shader == nullptr)
	{
		glDeleteBuffers(1, &vb);
		glDeleteBuffers(1, &ib);
		glDeleteVertexArrays(1, &va);
		shader = nullptr;
	}
}

void Renderer::draw(glm::vec4 verts[4], Texture* tex, Shader* shader, glm::vec4 tint)
{
	Vertex vertices[4];
	for (int i = 0; i < 4; i++)
	{
		glm::vec4 vert = verts[i];
		vertices[i].pos = vert;
		vertices[i].texUV = uv[i];
		vertices[i].tint = tint;
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, vertices);

	tex->bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::drawNode(glm::vec3 pos, float heading, glm::vec4 color)
{
	glm::mat4 mat = glm::rotate(glm::mat4(1), glm::radians(heading), glm::vec3(0, 0, 1));
	glm::vec4 verts[4] = {
		glm::vec4(+0.04, +0.04, 0, 1) * mat,
		glm::vec4(+0.04, -0.04, 0, 1) * mat,
		glm::vec4(-0.04, +0.04, 0, 1) * mat,
		glm::vec4(-0.04, -0.04, 0, 1) * mat,
	};
	pos.y /= 72;
	pos.x /= 72;
	for (int j = 0; j < 4; j++)
	{
		verts[j] = {pos.x + verts[j].x, pos.y + verts[j].y, pos.z, 1};
	}
	draw(verts, nodeTex, shader, color);
}

void Renderer::drawSegment(glm::vec2 start, glm::vec2 end, float z, float startTan, float endTan, glm::vec4 color)
{
	static float lineWidth = 0.02;

	glm::vec4 verts[6];

	verts[0] = {start.x / 72, start.y / 72, z, 1};
	verts[5] = {end.x / 72, end.y / 72, z, 1};

	float startTanRad = glm::radians(startTan);
	float endTanRad = glm::radians(endTan + 180.0f);
	verts[0].z = cos(startTanRad) * lineWidth;
	verts[0].w = sin(startTanRad) * lineWidth;
	verts[5].z = -cos(endTanRad) * lineWidth;
	verts[5].w = -sin(endTanRad) * lineWidth;

	for (int i = 0; i < 4; i++)
	{
		float t = i * 0.2 + 0.2;
		glm::vec3 pos = getPosRR(start, end, startTan, endTan, t);

		verts[i + 1].x = pos.x / 72;
		verts[i + 1].y = pos.y / 72;
		verts[i + 1].z = cos(pos.z) * lineWidth;
		verts[i + 1].w = sin(pos.z) * lineWidth;
	}

	for (int i = 0; i < 5; i++)
	{
		glm::vec4 start = verts[i];
		glm::vec4 end = verts[i + 1];
		glm::vec4 verts2[4];
		verts2[0] = {-start.z + start.x, start.w + start.y, z, 1};
		verts2[1] = {start.z + start.x, -start.w + start.y, z, 1};
		verts2[2] = {-end.z + end.x, end.w + end.y, z, 1};
		verts2[3] = {end.z + end.x, -end.w + end.y, z, 1};
		draw(verts2, segmentTex, shader, color);
	}
}

#define nodeCount 10
void Renderer::drawSegment(float z, const std::array<glm::vec2, maxCtrlPts>& controlPoints, int count, glm::vec4 color)
{
	static float lineWidth = 0.02;
	glm::vec4 verts[nodeCount];

	int l = count - 1;

	verts[0] = {controlPoints[0].x / 72, controlPoints[0].y / 72, z, 1};
	verts[nodeCount - 1] = {controlPoints[l].x / 72, controlPoints[l].y / 72, z, 1};

	glm::vec2 tangent = bezierTangent<maxCtrlPts>(controlPoints, count, 0.001f);
	verts[0].z = tangent.y * lineWidth;
	verts[0].w = tangent.x * lineWidth;

	tangent = bezierTangent<maxCtrlPts>(controlPoints, count, 0.999f);
	verts[nodeCount - 1].z = tangent.y * lineWidth;
	verts[nodeCount - 1].w = tangent.x * lineWidth;

	for (int i = 0; i < nodeCount - 2; i++)
	{
		float t = i * (1.0f / (nodeCount - 1.0f)) + (1.0f / (nodeCount - 1.0f));
		glm::vec2 p = bezierPosition<maxCtrlPts>(controlPoints, count, t);

		glm::vec2 tangent = bezierTangent<maxCtrlPts>(controlPoints, count, t);
		verts[i + 1].x = p.x / 72;
		verts[i + 1].y = p.y / 72;
		verts[i + 1].z = tangent.y * lineWidth;
		verts[i + 1].w = tangent.x * lineWidth;
	}

	for (int i = 0; i < nodeCount - 1; i++)
	{
		glm::vec4 start = verts[i];
		glm::vec4 end = verts[i + 1];
		glm::vec4 verts2[4];
		verts2[0] = {-start.z + start.x, start.w + start.y, z, 1};
		verts2[1] = {start.z + start.x, -start.w + start.y, z, 1};
		verts2[2] = {-end.z + end.x, end.w + end.y, z, 1};
		verts2[3] = {end.z + end.x, -end.w + end.y, z, 1};
		draw(verts2, segmentTex, shader, color);
	}
}

void Renderer::drawControlPoints(const std::array<glm::vec2, maxCtrlPts>& controlPoints, int count, float z)
{
	for (int i = 1; i < count - 1; i++)
	{
		glm::vec2 pos = controlPoints[i];
		glm::vec4 verts[4] = {
			glm::vec4(+0.03, +0.03, 0, 1),
			glm::vec4(+0.03, -0.03, 0, 1),
			glm::vec4(-0.03, +0.03, 0, 1),
			glm::vec4(-0.03, -0.03, 0, 1),
		};

		pos.y /= 72;
		pos.x /= 72;
		for (int j = 0; j < 4; j++)
			verts[j] = {pos.x + verts[j].x, pos.y + verts[j].y, z, 1};
		draw(verts, ctrlPointTex, shader, {1.0f, 1.0f, 1.0f, 1.0f});
	}
}

void Renderer::drawRobot(glm::vec3 curPos, float sizeX, float sizeY)
{
	sizeX = (sizeX / 72.0f) / 2.0f;
	sizeY = (sizeY / 72.0f) / 2.0f;
	glm::mat4 mat = glm::rotate(glm::mat4(1), curPos.z, glm::vec3(0, 0, 1));
	glm::vec4 verts[4] = {
		glm::vec4(+sizeX, +sizeY, 0, 1) * mat,
		glm::vec4(+sizeX, -sizeY, 0, 1) * mat,
		glm::vec4(-sizeX, +sizeY, 0, 1) * mat,
		glm::vec4(-sizeX, -sizeY, 0, 1) * mat,
	};
	curPos.y /= 72;
	curPos.x /= 72;
	for (int j = 0; j < 4; j++)
	{
		verts[j] = {curPos.x + verts[j].x, curPos.y + verts[j].y, 1.0f, 1};
	}
	draw(verts, robotTex, shader, {1.0f, 1.0f, 1.0f, 1.0f});
}
