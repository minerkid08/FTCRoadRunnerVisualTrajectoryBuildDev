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
	Renderer();
	~Renderer();

	void draw(glm::vec4 verts[4], Texture* tex, Shader* shader, glm::vec4 tint = {1, 1, 1, 1});

  private:
	glm::mat4 camMat;
	GLuint va;
	GLuint vb;
	GLuint ib;
};
