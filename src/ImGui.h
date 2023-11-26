#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <NodeGrid.h>
#include <string>
GLFWwindow* getWindow();

class ImGuiClass{
	public:
	ImGuiClass(int windowSize);
	~ImGuiClass();
	void begin();
	void end();
	void nodeList(NodeGrid* grid);
	void nodeProperties(NodeGrid* grid);
	private:
	char* path;
};