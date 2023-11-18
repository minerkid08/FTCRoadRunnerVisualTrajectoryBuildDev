#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
GLFWwindow* getWindow();

class ImGuiClass{
	public:
	ImGuiClass();
	~ImGuiClass();
	void begin();
	void end();
};