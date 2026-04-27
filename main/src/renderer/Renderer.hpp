#pragma once
#include "Shader.hpp"
#include "Texture.hpp"
#include "glm/ext/vector_float2.hpp"
#include <array>
#include <glm/glm.hpp>

#define maxCtrlPts 8

struct Vertex
{
	glm::vec2 pos;
	glm::vec2 texUV;
	glm::vec4 tint;
};

class Renderer
{
  public:
  Texture* nodeTex;
  Texture* segmentTex;
  Texture* ctrlPointTex;
  Texture* robotTex;
  Shader* shader;

	Renderer();
	~Renderer();

	void draw(glm::vec4 verts[4], Texture* tex, Shader* shader, glm::vec4 tint = {1, 1, 1, 1});
  void drawNode(glm::vec3 pos, float heading, glm::vec4 color);
  void drawSegment(glm::vec2 start, glm::vec2 end, float z, float startTan, float endTan, glm::vec4 color);
  void drawSegment(float z, const std::array<glm::vec2, maxCtrlPts>& controlPoints, int count, glm::vec4 color);
  void drawControlPoints(const std::array<glm::vec2, maxCtrlPts>& controlPoints, int count, float z);
  void drawRobot(glm::vec3 curPos, float sizeX, float sizeY);

  void del();

  private:
	glm::mat4 camMat;
	GLuint va;
	GLuint vb;
	GLuint ib;
};
