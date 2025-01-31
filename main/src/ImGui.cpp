#include "ImGui.hpp"
#include "Action.hpp"
#include "FileExplorer.hpp"
#include "FrameBuffer.hpp"
#include "NodeGrid.hpp"
#include "Save.hpp"
#include "global.hpp"

#include <cstdint>
#include <imgui/imgui.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void trajectoryUi(NodeGrid* grid)
{
	ImGui::Separator();
	if (ImGui::Button("flipHoriz"))
	{
		grid->flipHoriz();
	}
	ImGui::SameLine();
	if (ImGui::Button("flipVert"))
	{
		grid->flipVert();
	}
	ImGui::Checkbox("grid snap", &(grid->gridSnap));
	if (ImGui::Button("^"))
	{
		if (grid->selected.type == TypeNode)
		{
			if (grid->nodes.moveUp(grid->selected.ind))
			{
				grid->selected.ind++;
			}
		}
		else
		{
			if (grid->segs.moveUp(grid->selected.ind))
			{
				grid->selected.ind++;
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("v"))
	{
		if (grid->selected.type == TypeNode)
		{
			if (grid->nodes.moveDown(grid->selected.ind))
			{
				grid->selected.ind--;
			}
		}
		else
		{
			if (grid->segs.moveDown(grid->selected.ind))
			{
				grid->selected.ind--;
			}
		}
	}
	int id = 0;
	ImGui::Text("nodes: %d/%d", grid->nodes.count, maxNodes);
	int j = 0;
	if (grid->nodes.count > 0)
	{
		for (int i = 0; i < grid->nodes.count; i++)
		{
			PathNode* node = grid->nodes.get(i);

			ImVec4 tint(0.25f, 0.25f, 0.25f, 1);
			if (grid->selected.ind == i && grid->selected.type == TypeNode)
			{
				tint.x = 0.75f;
			}

			ImGui::PushStyleColor(ImGuiCol_Button, tint);
			ImGui::PushID(id++);
			if (ImGui::Button(std::to_string(i).c_str()))
			{
				grid->selected.ind = i;
				grid->selected.type = TypeNode;
			}
			ImGui::PopID();
			ImGui::PopStyleColor();
			if (j < 16 && i < grid->nodes.count - 1)
			{
				ImGui::SameLine();
				j++;
			}
			else
			{
				j = 0;
			}
		}
	}

	ImGui::Text("segments: %d/%d", grid->segs.count, maxSegs);
	j = 0;
	if (grid->segs.count > 0)
	{
		for (int i = 0; i < grid->segs.count; i++)
		{
			PathSegment* seg = grid->segs.get(i);

			ImVec4 tint(0.25f, 0.25f, 0.25f, 1);
			if (grid->selected.ind == i && grid->selected.type == TypeSegment)
			{
				tint.x = 0.75f;
			}

			ImGui::PushStyleColor(ImGuiCol_Button, tint);
			ImGui::PushID(id++);
			if (ImGui::Button(std::to_string(i).c_str()))
			{
				grid->selected.ind = i;
				grid->selected.type = TypeSegment;
			}
			ImGui::PopID();
			ImGui::PopStyleColor();
			if (j < 16 && i < grid->segs.count - 1)
			{
				ImGui::SameLine();
				j++;
			}
			else
			{
				j = 0;
			}
		}
		ImGui::Text(" ");
	}

	ImGui::Separator();

	if (grid->selected.type == TypeNode)
	{
		if (grid->selected.ind > -1 && grid->selected.ind < grid->nodes.count)
		{
			PathNode* node = grid->nodes.get(grid->selected.ind);
			ImGui::Text("node: %d", grid->selected.ind);
			if (ImGui::Button("remove"))
			{
				grid->nodes.remove(grid->selected.ind);
				std::vector<int> toRemove;
				for (int i = 0; i < grid->segs.count; i++)
				{
					PathSegment* seg = grid->segs.get(i);
					if (seg->startNode == grid->selected.ind || seg->endNode == grid->selected.ind)
					{
						toRemove.push_back(i);
					}
					if (seg->startNode > grid->selected.ind)
					{
						seg->startNode--;
					}
					if (seg->endNode > grid->selected.ind)
					{
						seg->endNode--;
					}
				}
				int j = 0;
				for (int i : toRemove)
				{
					grid->segs.remove(i - j++);
				}
				return;
			}
			ImGui::InputFloat2("pos", glm::value_ptr(node->pos));
			ImGui::InputFloat("heading", &node->heading);
		}
	}
	else
	{
		if (grid->selected.ind > -1 && grid->selected.ind < grid->segs.count)
		{
			PathSegment* seg = grid->segs.get(grid->selected.ind);
			ImGui::Text("segment: %d", grid->selected.ind);
			if (ImGui::Button("remove"))
			{
				grid->segs.remove(grid->selected.ind);
			}
			static const char* headingModes[] = {"none", "linear", "constant", "spline"};

			if (ImGui::BeginCombo("heading mode", headingModes[seg->headingMode]))
			{
				for (int i = 0; i < 4; i++)
				{
					if (ImGui::Selectable(headingModes[i]))
					{
						seg->headingMode = i;
					}
				}
				ImGui::EndCombo();
			}
			ImGui::InputFloat("start tangent", &seg->startTan);
			ImGui::InputFloat("end tangent", &seg->endTan);
		}
	}
}

void initUI()
{
  explorerSetPath("save");

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

void closeUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void beginDockspace()
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void endDockspace()
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

void drawAction(Action* action);

void renderUI(FrameBuffer& framebuffer)
{
	beginDockspace();
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
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

	ImGui::BeginMenuBar();
	if (ImGui::MenuItem("new"))
		reset();
	if (ImGui::MenuItem("save"))
		save(global.filename);
	if (ImGui::MenuItem("save as"))
	{
		global.explorerMode = 1;
		explorerReset(FileExplorerFlags_MakeFile);
	}
	if (ImGui::MenuItem("load"))
	{
		explorerReset();
		global.explorerMode = 2;
	}
  if(ImGui::MenuItem("help"))
    std::cout << "get gud\n";
	ImGui::EndMenuBar();

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

	ImGui::Image((void*)(intptr_t)framebuffer.getColor(), ImVec2{(float)framebufferSize, (float)framebufferSize}, ImVec2{0, 1},
				 ImVec2{1, 0});
	ImGui::End();

	ImGui::Begin("actionList");
	if (global.rootAction == nullptr)
	{
		global.rootAction = new Action();
		global.actions.push_back(global.rootAction);
	}
	drawAction(global.rootAction);

	ImGui::End();

	ImGui::Begin("actionEditor");
	if (global.currentAction != nullptr)
	{
		if (ImGui::Combo("type", &global.currentAction->type, "sequental\0parallel\0trajectory\0"))
		{
			if (global.currentAction->type == ACTION_TRAJECTORY)
			{
				if (global.currentAction->data == nullptr)
					global.currentAction->data = new NodeGrid();
			}
		}
		if (global.currentAction->type != ACTION_TRAJECTORY)
		{
			if (ImGui::Button("addAction"))
			{
				addAction(global.currentAction);
			}
		}
		if (ImGui::Button("removeAction"))
		{
			if (!global.currentAction->actions)
			{
			}
		}
		if (global.currentAction->type == ACTION_TRAJECTORY)
			trajectoryUi(global.currentAction->data);
	}
	ImGui::End();

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

	// End the parent window that contains the Dockspace:
	ImGui::End();

	endDockspace();
}

void drawAction(Action* action)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
							   (action == global.currentAction ? ImGuiTreeNodeFlags_Selected : 0);
	bool opened = ImGui::TreeNodeEx((void*)action->id, flags, "%s", global.actionTypes[action->type]);
	ImVec2 size = ImGui::GetItemRectSize();
	if (ImGui::IsItemClicked())
	{
		global.currentAction = action;
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("action");
		if (payload)
		{
			Action* a = *(Action**)payload->Data;
			moveAction(a, action);
		}
		ImGui::EndDragDropTarget();
	}

	if (action != global.rootAction)
	{
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("action", (void*)&action, sizeof(void*));
			ImGui::EndDragDropSource();
		}
		if (action == global.currentAction)
		{
			ImGui::SameLine();
			if (ImGui::Button("^", ImVec2(0, size.y)))
				moveActionUp(action);
			ImGui::SameLine();
			if (ImGui::Button("v", ImVec2(0, size.y)))
				moveActionDown(action);
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
