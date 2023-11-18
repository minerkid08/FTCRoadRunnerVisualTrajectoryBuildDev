#include <Renderer.h>

void glErrorCheck(){
	int err = glGetError();
	while(err != GL_NO_ERROR){
		std::string error;
		switch (err){
	        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
	        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
	        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
	        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
	        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
	        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
	        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << "\n";
		err = glGetError();
	}
}

static int inds[] = {
	0, 1, 2, 1, 2, 3
};

static glm::vec2 uv[4] = {
	{1,1},
	{1,0},
	{0,1},
	{0,0},
};

Renderer::Renderer(){
	glCreateVertexArrays(1, &va);
	glBindVertexArray(va);

	glCreateBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 7));
	glEnableVertexAttribArray(3);

	glCreateBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);
}

Renderer::~Renderer(){
	glDeleteBuffers(1, &vb);
	glDeleteBuffers(1, &ib);
	glDeleteVertexArrays(1, &va);
}

void Renderer::draw(glm::vec2 verts[4], Texture* tex, glm::vec4 tint){
	Vertex vertices[4];
	for(int i = 0; i < 4; i++){
		vertices[i].pos = verts[i];
		vertices[i].texInd = 0;
		vertices[i].tint = tint;
		vertices[i].texUV = uv[i];
	}

	tex->bind(0);
	glErrorCheck();

	glBindVertexArray(va);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, vertices);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}