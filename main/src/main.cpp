#include <cstring>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>

#include "actions/Action.hpp"
#include "global.hpp"
#include "imgui/imgui.h"
#include "projectSettings.hpp"
#include "renderer/FrameBuffer.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Shader.hpp"
#include "settings.hpp"
#include "trajectories/Trajectory.hpp"
#include "ui/ui.hpp"
#include "preview.hpp"

#include <math.h>

Globals global;
Settings settings;
ProjectSettings projectSettings;

GLFWwindow* window;

GLFWwindow* getWindow()
{
	return window;
}

static bool close = false;

int mouseX = 0;
int mouseY = 0;

int mods = 0;

struct WindowData
{
	FrameBuffer* framebuffer;
	bool running;
};

WindowData windowData;

int main(int argc, char** argv)
{

	glfwInit();
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	int x;
	int y;
	int width;
	int height;
	glfwGetMonitorWorkarea(monitor, &x, &y, &width, &height);
	int winSize = 800;

	if (width > 3000)
	{
		global.uiScale = 2.0;
		winSize = 1700;
	}
	else
	{
		global.uiScale = 1.0;
	}

	if (argc > 2)
	{
		if (strcmp(argv[2], "4k") == 0)
		{
			winSize = 1700;
		}
	}

	loadSettings();

	generateActionNames();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Without these 2 hints, nothing above OpenGL 2.1 is supported
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(winSize * 2, winSize, "FTC Roadrunner Visual Trajectory Builder", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (status == GL_FALSE)
	{
		std::cout << "failed to init glad\n";
		return -1;
	}

	windowData.running = true;

	FrameBuffer framebuffer({});
	windowData.framebuffer = &framebuffer;

	glfwSetWindowUserPointer(window, &windowData);
	glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_TRUE);

	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
		WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
		data->running = false;
	});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int btn, int action, int _mods) {
		if (btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
			if (mouseX <= data->framebuffer->spec.width && mouseY <= data->framebuffer->spec.width)
			{
				if (global.onViewport && global.currentAction)
				{
					if (global.currentAction->type == ACTION_TRAJECTORY)
						global.currentAction->traj->mouseClick(mouseX, mouseY, data->framebuffer->spec.width, mods);
				}
			}
		}
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
		mouseX = floor(x) - global.mouseOffsetX;
		mouseY = floor(y) - global.mouseOffsetY;
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
	Texture nodeTex("node.png");
	Texture segTex("seg.png");
	Texture ctrlPtTex("ctrlPt.png");
	Texture robotTex("robot.png");
	renderer.shader = &shader;
	renderer.nodeTex = &nodeTex;
	renderer.segmentTex = &segTex;
	renderer.ctrlPointTex = &ctrlPtTex;
	renderer.robotTex = &robotTex;

	initUi();

	ImGuiStyle& style = ImGui::GetStyle();

	style.TabBarOverlineSize = 0;
	if (width > 3000)
	{
		style.WindowRounding = 12;
		style.FrameRounding = 12;
		style.PopupRounding = 12;
		style.GrabRounding = 12;
		style.TabRounding = 12;
	}
	else
	{
		style.WindowRounding = 9;
		style.FrameRounding = 4;
		style.PopupRounding = 4;
		style.GrabRounding = 4;
		style.TabRounding = 9;
	}

	double lastFrameTime = 0;
	double lastUpdateTime = 0;
	double fpsLimit = 1.0 / 30.0;

	while (!close)
	{
		double now = glfwGetTime();
		double deltaTime = now - lastUpdateTime;
		if (now - lastFrameTime >= fpsLimit)
		{
			double dt = now - lastFrameTime;
			framebuffer.bind();
			glClearColor(0.1, 0.1, 0.1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			shader.use();
			renderer.draw(verts, &tex, &shader, glm::vec4(1, 1, 1, 1));

			for (Action* action : global.actions)
			{
				if (action == global.currentAction)
					continue;
				if (action->type == ACTION_TRAJECTORY && action->traj != nullptr)
				{
					if (action->traj->visible)
						action->traj->render(renderer, settings.trajectoryOpac, 0, false);
				}
			}

			if (global.currentAction != nullptr)
			{
				if (global.currentAction->type == ACTION_TRAJECTORY)
					global.currentAction->traj->update(renderer, mouseX, mouseY, framebuffer.spec.width, mods, dt);
			}
			drawPreview(renderer);
			framebuffer.unbind();

			renderUi(framebuffer, !windowData.running);
			windowData.running = true;

			glfwSwapBuffers(window);

			lastFrameTime = now;
		}
		lastUpdateTime = now;
		glfwPollEvents();
	}

	closeUi();

	shader.del();
	renderer.del();
	tex.del();
	nodeTex.del();
	segTex.del();
	ctrlPtTex.del();
	robotTex.del();
	framebuffer.del();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void quit()
{
	saveSettings();
	close = true;
}
