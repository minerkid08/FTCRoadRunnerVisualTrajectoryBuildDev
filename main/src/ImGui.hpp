#pragma once
#include "FileExplorer.hpp"
#include "NodeGrid.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
GLFWwindow* getWindow();

class ImGuiClass
{
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
	int windowSize = 0;
	char* path;
	void nodeUi(NodeGrid* grid);
	void segUi(NodeGrid* grid);
};
