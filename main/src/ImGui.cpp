#include "ImGui.hpp"
#include "Action.hpp"
#include "FrameBuffer.hpp"
#include "Save.hpp"
#include "imgui/imgui.h"
#include <execution>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

ImGuiClass::ImGuiClass(int _windowSize)
{
	windowSize = _windowSize;
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
	io.FontGlobalScale = windowSize == 1700 ? 2 : 1.5f; // 2 for 4k
	style.ScaleAllSizes(windowSize == 1700 ? 2 : 1.5);	// 2 for 4k
	ImGui_ImplGlfw_InitForOpenGL(getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	path = new char[256];
	memset(path, 0, 256);
	explorer.setMainPath("save");
}

ImGuiClass::~ImGuiClass()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiClass::begin()
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void ImGuiClass::end()
{
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

static bool open = true;
static bool* p_open = &open;

static std::vector<Action> actions;
static Action* rootAction = nullptr;
static Action* currentAction = nullptr;

static const char* actionTypes[] = {"sequentional", "parallel", "trajectory"};

static int actionTypeCount = 3;

void drawAction(Action* action);
void addAction(Action* parent);

void ImGuiClass::nodeList(NodeGrid* grid, FrameBuffer& framebuffer)
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
						ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace Demo", p_open, window_flags);

	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
			ImGui::MenuItem("Padding", NULL, &opt_padding);
			ImGui::Separator();

			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
			}
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
			}
			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "",
								(dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
			}
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
			}
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "",
								(dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
				if (p_open != NULL)
					*p_open = false;
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Begin("viewport");
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	glm::vec2 glmSize = {viewportPanelSize.x, viewportPanelSize.y};
	int framebufferSize = glmSize.x > glmSize.y ? glmSize.y : glmSize.x;
	if (framebuffer.spec.width != framebufferSize)
	{
		framebuffer.resize(framebufferSize, framebufferSize);
	}
	ImVec2 screenPos = ImGui::GetCursorScreenPos();
	ImGui::Image((void*)framebuffer.getColor(), ImVec2{(float)framebufferSize, (float)framebufferSize}, ImVec2{0, 1},
				 ImVec2{1, 0});
	ImGui::End();

	ImGui::Begin("actionList");
	if (rootAction == nullptr)
	{
		actions.push_back({});
		rootAction = &actions[0];
	}
	drawAction(rootAction);

	ImGui::End();

	ImGui::Begin("actionEditor");
	if (currentAction != nullptr)
	{
		ImGui::Combo("type", &currentAction->type, "sequental\0parallel\0trajectory\0");
		if (currentAction->type == 2)
		{
		}
		else
		{
			if (ImGui::Button("addAction"))
			{
				addAction(currentAction);
			}
		}
	}
	ImGui::End();

	// End the parent window that contains the Dockspace:
	ImGui::End();
}

void drawAction(Action* action)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
							   (action == currentAction ? ImGuiTreeNodeFlags_Selected : 0);
	bool opened = ImGui::TreeNodeEx((void*)action->id, flags, "%s", actionTypes[action->type]);
	if (ImGui::IsItemClicked())
	{
		currentAction = action;
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("action");
		if (payload)
		{
			Action* a = *(Action**)payload->Data;
			if (a->prev)
			{
				a->prev->next = a->next;
			}
			else
			{
				a->parrent->actions = a->next;
			}
			if (a->next)
			{
				a->next->prev = a->prev;
			}

			a->next = nullptr;
			a->parrent = a;
			if (action->actions == nullptr)
			{
				action->actions = a;
				a->prev = nullptr;
			}
			else
			{
				Action* action2 = action->actions;
				while (true)
				{
					if (action2->next == nullptr)
						break;
					action2 = action2->next;
				}
				action2->next = a;
				a->prev = action2;
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (action != rootAction)
	{
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("action", (void*)&action, sizeof(void*));
			ImGui::EndDragDropSource();
		}
	}

	if (opened)
	{
		if (action->actions)
		{
			Action* action2 = action->actions;
			while (true)
			{
				drawAction(action2);
				if (action2->next == nullptr)
					break;
				action2 = action2->next;
			}
		}
		ImGui::TreePop();
	}
}

void addAction(Action* parent)
{
	int actionInd = actions.size();
	actions.emplace_back();

	Action* action = &actions[actionInd];

	action->id = actionInd;
	action->parrent = parent;

	if (parent->actions)
	{
		Action* a = parent->actions;
		while (true)
		{
			if (a->next)
				a = a->next;
			else
				break;
		}
		a->next = action;
    action->prev = a;
	}
	else
	{
		parent->actions = action;
	}
}
