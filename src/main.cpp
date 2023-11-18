#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>

#include <Shader.h>
#include <Renderer.h>

struct WindowData{
	bool running = true;
};


int main(){
	WindowData windowData;
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(2000, 2000, "window", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &windowData);

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if(status == GL_FALSE){
		std::cout << "failed to init glad\n";
		return -1;
	}

	glfwSetWindowCloseCallback(window, [](GLFWwindow* window){
		WindowData* windowData = (WindowData*)glfwGetWindowUserPointer(window);
		windowData->running = false;
	});

	Shader shader(R"(
		#version 330 core
		layout(location = 0) in vec2 pos;
		layout(location = 1) in int itexInd;
		layout(location = 2) in vec4 itint;
		layout(location = 3) in vec2 itexUV;
		out int texInd;
		out vec4 tint;
		out vec2 texUV;
		void main(){
			texInd = itexInd;
			tint = itint;
			texUV = itexUV;
			gl_Position = vec4(pos.x, pos.y, 0, 1);
		}
	)",
	R"(
		#version 330 core
		out vec4 color;
		in int texInd;
		in vec4 tint;
		in vec2 texUV;
		uniform sampler2D tex;
		void main(){
			vec4 texColor = texture(tex, texUV);
			color = texColor * tint;
		}
	)");

	Renderer renderer;

	//float verts[8] = {
	//	0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f
	//};
//
	//GLuint va;
	//glCreateVertexArrays(1, &va);
	//glBindVertexArray(va);
//
	//GLuint vb;
	//glCreateBuffers(1, &vb);
	//glBindBuffer(GL_ARRAY_BUFFER, vb);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
//
	//int inds[6] = {
	//	0, 1, 2, 1, 2, 3
	//};
//
	//GLuint ib;
	//glCreateBuffers(1, &ib);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);


	glm::vec2 verts[]{
		{1, 1},
		{1, -1},
		{-1, 1},
		{-1, -1}
	};

	Texture tex("field-2023-juice-dark.png");

	while(windowData.running){
		glClearColor(0.1, 0.1, 0.1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		shader.use();
		renderer.draw(verts, &tex, glm::vec4(1, 1, 1, 1));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	return 0;
}