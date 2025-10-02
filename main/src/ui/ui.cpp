#include "ui.hpp"

#include "FileExplorer.hpp"
#include "Save.hpp"
#include "global.hpp"
#include "main.hpp"
#include "renderer/FrameBuffer.hpp"
#include "settings.hpp"

#include "glfw/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ui/popup.hpp"
#include <filesystem>

GLFWwindow* getWindow();

static int level;
static std::string msg;

static bool logOpen = false;

void setNotif(const std::string& str)
{
	addLog(LEVEL_INFO, str);
	if (level < LEVEL_INFO)
	{
		level = LEVEL_INFO;
		msg = str;
	}
}

void setWarn(const std::string& str)
{
	addLog(LEVEL_WARN, str);
	if (level < LEVEL_WARN)
	{
		level = LEVEL_WARN;
		msg = str;
	}
}

void setErr(const std::string& str)
{
	addLog(LEVEL_ERROR, str);
	if (level < LEVEL_ERROR)
	{
		level = LEVEL_ERROR;
		msg = str;
	}
}

void clearMsg()
{
	level = LEVEL_NONE;
	msg = "";
}

void initUi()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.FontDefault = io.Fonts->AddFontFromFileTTF("OpenSans-Bold.ttf", 20.0f);
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	io.FontGlobalScale = global.uiScale; // 2 for 4k, 1.5 for 1080
	style.ScaleAllSizes(global.uiScale); // 2 for 4k, 1.5 for 1080
	ImGui_ImplGlfw_InitForOpenGL(getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void closeUi()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

static bool open = true;

void beginDockspace()
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	window_flags |=
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::Begin("dockspace", &open, window_flags);

	ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("dockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
}

void endDockspace()
{
	ImGui::End();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* ctx = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(ctx);
	}
}

void drawMenuBar(bool shouldClose);
void drawViewport(FrameBuffer& framebuffer);

void renderUi(FrameBuffer& framebuffer, bool shouldClose)
{
	beginDockspace();

	drawMenuBar(shouldClose);
	drawViewport(framebuffer);
	drawActionEditor();
	drawActionList();
	drawSettingsMenu();
	drawAboutWindow();
	drawLog(&logOpen);

	if (global.explorerMode)
	{
		if (int res = explorerUpdate(".path"))
		{
			if (res == FileExplorerUpdate_PathSelected)
			{
				global.filename = explorerGetPath();
				if (global.explorerMode == 1)
					save(global.filename);
				else
					load(global.filename);
			}
			global.explorerMode = 0;
		}
	}

	endDockspace();
}

void exportProject(const Action* action);

void drawMenuBar(bool shouldClose)
{
	ImGui::BeginMenuBar();
	if (shouldClose)
		openQuitPopup();
	if (ImGui::MenuItem("quit"))
		openQuitPopup();
	if (ImGui::MenuItem("new"))
		reset();
	if (ImGui::MenuItem("save"))
		save(global.filename);
	if (ImGui::MenuItem("save as"))
	{
		if (!std::filesystem::exists(settings.savePath))
			std::filesystem::create_directories(settings.savePath);
		global.explorerMode = 1;
		explorerSetPath(settings.savePath);
		explorerReset(FileExplorerFlags_MakeFile);
	}
	if (ImGui::MenuItem("load"))
	{
		if (!std::filesystem::exists(settings.savePath))
			std::filesystem::create_directories(settings.savePath);
		explorerSetPath(settings.savePath);
		explorerReset();
		global.explorerMode = 2;
	}
	if (ImGui::MenuItem("export"))
		exportProject(global.rootAction);
	if (ImGui::MenuItem("settings"))
		openSettings();
	if (ImGui::MenuItem("logs"))
		logOpen = true;
	if (ImGui::MenuItem("about"))
		openAboutWindow();

	ImGui::Separator();

	if (level == LEVEL_ERROR)
		ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.0f, 0.0f, 1.0f});
	if (level == LEVEL_WARN)
		ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 1.0f, 0.0f, 1.0f});
	ImGui::Text("%s", msg.c_str());
	if (level == LEVEL_WARN || level == LEVEL_ERROR)
		ImGui::PopStyleColor();

	drawQuitPopup();

	ImGui::EndMenuBar();
}

void drawViewport(FrameBuffer& framebuffer)
{
#ifdef DEBUG
	ImGui::Begin("style editor");
	ImGui::ShowStyleEditor();
	ImGui::End();
#endif
	ImGui::Begin("viewport");
	global.onViewport = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	glm::vec2 glmSize = {viewportPanelSize.x, viewportPanelSize.y};
	int framebufferSize = glmSize.x > glmSize.y ? glmSize.y : glmSize.x;
	if (framebuffer.spec.width != framebufferSize)
	{
		framebuffer.resize(framebufferSize, framebufferSize);
	}
	ImVec2 screenPos = ImGui::GetCursorScreenPos();
	global.mouseOffsetX = screenPos.x;
	global.mouseOffsetY = screenPos.y;

	ImGui::Image((void*)(intptr_t)framebuffer.getColor(), ImVec2{(float)framebufferSize, (float)framebufferSize},
				 ImVec2{0, 1}, ImVec2{1, 0});
	ImGui::End();
}
