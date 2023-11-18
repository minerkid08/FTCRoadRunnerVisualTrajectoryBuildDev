#pragma once
#include <glm/glm.hpp>
#include <Texture.h>
#include <array>

struct Vertex{
	glm::vec2 pos;
	int texInd;
	glm::vec4 tint;
	glm::vec2 texUV;
};

class Renderer{
	public:
	Renderer();
	~Renderer();

	//void draw(glm::vec2 pos, Texture* tex, glm::vec4 tint);
	void draw(glm::vec2 verts[4], Texture* tex, glm::vec4 tint);

	private:
	GLuint va;
	GLuint vb;
	GLuint ib;
};