#pragma once
#include <glm/glm.hpp>
#include <Texture.h>
#include <Shader.h>
#include <array>

struct Vertex{
	glm::vec2 pos;
	glm::vec2 texUV;
	glm::vec4 tint;
};

class Renderer{
	public:
	Renderer();
	~Renderer();

	void draw(glm::vec4 verts[4], Texture* tex, Shader* shader, glm::vec4 tint);

	private:
	GLuint va;
	GLuint vb;
	GLuint ib;
};