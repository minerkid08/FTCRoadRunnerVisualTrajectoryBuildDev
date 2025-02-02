#pragma once
#include "renderer/FrameBuffer.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

GLFWwindow* getWindow();

void initUI();
void closeUI();
void renderUI(FrameBuffer& framebuffer);
