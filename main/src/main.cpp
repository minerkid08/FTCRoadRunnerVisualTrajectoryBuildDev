#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>

#include "ImGui.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"

#include "NodeGrid.hpp"
#include "Save.hpp"
#include "Upload.hpp"

#include <Windows.h>
#include <math.h>

#include <filesystem>

GLFWwindow* window;
NodeGrid* grid;

GLFWwindow* getWindow()
{
	return window;
}

int windowSize = 800;

int mouseX = 0;
int mouseY = 0;

int mods = 0;

int main(int argc, char** argv)
{

	if (!std::filesystem::exists("save"))
	{
		std::filesystem::create_directory("save");
	}

	if (!std::filesystem::exists("export"))
	{
		std::filesystem::create_directory("export");
	}

	glfwInit();
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	int x;
	int y;
	int width;
	int height;
	glfwGetMonitorWorkarea(monitor, &x, &y, &width, &height);
	if (width > 3000)
	{
		windowSize = 1700;
	}

	if (argc > 2)
	{
		if (strcmp(argv[2], "4k") == 0)
		{
			windowSize = 1700;
		}
	}

	bool running = true;

	window = glfwCreateWindow(windowSize * 2, windowSize, "FTC Roadrunner Visual Trajectory Builder", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &running);
	glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (status == GL_FALSE)
	{
		std::cout << "failed to init glad\n";
		return -1;
	}

	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
		bool* running = (bool*)glfwGetWindowUserPointer(window);
		*running = false;
    Upload::close();
	});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int btn, int action, int _mods) {
		if (btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			if (mouseX <= windowSize && mouseY <= windowSize)
			{
				ImGuiIO& io = ImGui::GetIO();
				if (!io.WantCaptureMouse)
				{
					grid->mouseClick(mouseX, mouseY, windowSize, mods);
				}
			}
		}
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
		mouseX = floor(x);
		mouseY = floor(y);
	});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int _mods) {
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)
		{
			if (action == GLFW_PRESS)
			{
				mods = 1;
			}
			if (action == GLFW_RELEASE)
			{
				mods = 0;
			}
		}
		if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
		{
			if (action == GLFW_PRESS)
			{
				mods = 2;
			}
			if (action == GLFW_RELEASE)
			{
				mods = 0;
			}
		}
	});

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader(R"(
		#version 330 core
		layout(location = 0) in vec2 pos;
		layout(location = 1) in vec2 itexUV;
		layout(location = 2) in vec4 itint;
		out vec2 texUV;
		out vec4 tint;
		void main(){
			texUV = itexUV;
			tint = itint;
			gl_Position = vec4(pos.x, pos.y, 0, 1);
		}
	)",
				  R"(
		#version 330 core
		out vec4 color;
		in vec2 texUV;
		in vec4 tint;
		uniform sampler2D tex;
		void main(){
			color = texture(tex, texUV) * tint;
		}
	)");

	Renderer renderer;

	glm::vec4 verts[]{{1, 1, 0, 1}, {1, -1, 0, 1}, {-1, 1, 0, 1}, {-1, -1, 0, 1}};

	Texture tex("field.png");

	ImGuiClass imGui(windowSize);

	grid = new NodeGrid(&shader);

	if (argc > 1)
	{
		if (strcmp(argv[1], "new"))
		{
			int len = strlen(argv[1]);
			char* str = new char[len];
			memcpy(str, argv[1], len);
			for (int i = 0; i < len; i++)
			{
				if (str[i] == '/')
				{
					str[i] = '\\';
				}
			}
			Save::load(grid, str);
		}
	}

	if (argc > 2)
	{
		if (strcmp(argv[2], "export") == 0)
		{
			Save::exp(grid);
			if (grid->err != "")
			{
				if (grid->err[7] == 'w')
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
				}
				else
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				}
				std::cout << grid->err << "\n";
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}
			if (grid->msg != "")
			{
				std::cout << grid->msg << "\n";
			}
			running = false;
		}
	}

  Upload::init(grid);

	while (running)
	{
		glClearColor(0.1, 0.1, 0.1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();

		renderer.draw(verts, &tex, &shader, glm::vec4(1, 1, 1, 1));

		grid->update(renderer, mouseX, mouseY, windowSize, mods);

		imGui.begin();
		imGui.nodeList(grid);
		imGui.end();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
