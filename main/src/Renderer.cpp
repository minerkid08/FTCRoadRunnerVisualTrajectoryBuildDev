#include "Renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>

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
	glCreateVertexArrays(1, &va);
	glBindVertexArray(va);

	// vb
	glCreateBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 2));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 4));

	// ib
	glCreateBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);

	camMat = glm::ortho(-2.0f, 2.0f, -1.0f, 1.0f);
}

Renderer::~Renderer()
{
	glDeleteBuffers(1, &vb);
	glDeleteBuffers(1, &ib);
	glDeleteVertexArrays(1, &va);
}

void Renderer::draw(glm::vec4 verts[4], Texture* tex, Shader* shader, glm::vec4 tint)
{
	Vertex vertices[4];
	for (int i = 0; i < 4; i++)
	{
		glm::vec4 vert = verts[i] * camMat;
		vertices[i].pos = {vert.x - 0.5f, vert.y};
		vertices[i].texUV = uv[i];
		vertices[i].tint = tint;
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, vertices);

	tex->bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
