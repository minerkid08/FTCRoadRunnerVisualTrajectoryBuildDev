#include <Renderer.h>
void errorCheck(int num){
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
		std::cout << num << " error " << err << ": " << error << "\n";
		err = glGetError();
	}
}

static int inds[] = {
	0, 1, 2, 2, 1, 3
};

static glm::vec2 uv[4] = {
	{1,1},
	{1,0},
	{0,1},
	{0,0},
};

Renderer::Renderer(){
	//va
	glCreateVertexArrays(1, &va);
	glBindVertexArray(va);

	//vb
	glCreateBuffers(1, &vb);
	glErrorCheck();
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glErrorCheck();
	
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	
	glErrorCheck();
	glEnableVertexAttribArray(0);
	glErrorCheck();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glErrorCheck();
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 2));
	glErrorCheck();
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 4));
	glErrorCheck();

	//ib
	glCreateBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);
	glErrorCheck();
}

Renderer::~Renderer(){
	glDeleteBuffers(1, &vb);
	glDeleteBuffers(1, &ib);
	glDeleteVertexArrays(1, &va);
}

void Renderer::draw(glm::vec4 verts[4], Texture* tex, Shader* shader, glm::vec4 tint){
	Vertex vertices[4];
	for(int i = 0; i < 4; i++){
		vertices[i].pos = {verts[i].x, verts[i].y};
		vertices[i].texUV = uv[i];
		vertices[i].tint = tint;
	}
	
	tex->bind();
	glErrorCheck();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glErrorCheck();

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glErrorCheck();

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glErrorCheck();
}