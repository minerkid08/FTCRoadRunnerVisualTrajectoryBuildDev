#pragma once
#include "Shader.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>

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
  Shader* shader;

	Renderer();
	~Renderer();

	void draw(glm::vec4 verts[4], Texture* tex, Shader* shader, glm::vec4 tint = {1, 1, 1, 1});
  void drawNode(glm::vec3 pos, float heading, glm::vec4 color);
  void drawSegment(glm::vec2 start, glm::vec2 end, float z, float startTan, float endTan, glm::vec4 color);

  private:
	glm::mat4 camMat;
	GLuint va;
	GLuint vb;
	GLuint ib;
};
