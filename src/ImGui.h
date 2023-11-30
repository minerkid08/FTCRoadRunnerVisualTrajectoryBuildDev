#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <NodeGrid.h>
#include <string>
#include <FileExplorer.h>
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
	FileExplorer explorer;
	int explorerMode = 0;
	char* path;
};