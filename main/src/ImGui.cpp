#include "ImGui.hpp"
#include "Save.hpp"
#include "Upload.hpp"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <thread>

static void removePart(PathNode* node, int ind)
{
	delete node->parts[ind];
	for (int i = ind; i < node->parts.size() - 1; i++)
	{
		node->parts[i] = node->parts[i + 1];
	}
	node->parts.resize(node->parts.size() - 1);
}

static void moveUp(PathNode* node, int ind)
{
	if (ind + 1 < node->parts.size())
	{
		NodePart* nodePart = node->parts[ind + 1];
		node->parts[ind + 1] = node->parts[ind];
		node->parts[ind] = nodePart;
	}
}

static void moveDown(PathNode* node, int ind)
{
	if (ind - 1 > -1)
	{
		NodePart* nodePart = node->parts[ind - 1];
		node->parts[ind - 1] = node->parts[ind];
		node->parts[ind] = nodePart;
	}
}

static void nodePartButtons(PathNode* node, int i)
{
	if (ImGui::Button("x"))
	{
		removePart(node, i);
	}
	ImGui::SameLine();
	if (ImGui::Button("^"))
	{
		moveDown(node, i);
	}
	ImGui::SameLine();
	if (ImGui::Button("v"))
	{
		moveUp(node, i);
	}
}

static void removePart(PathSegment* seg, int ind)
{
	delete seg->parts[ind];
	for (int i = ind; i < seg->parts.size() - 1; i++)
	{
		seg->parts[i] = seg->parts[i + 1];
	}
	seg->parts.resize(seg->parts.size() - 1);
}

static void moveUp(PathSegment* seg, int ind)
{
	if (ind + 1 < seg->parts.size())
	{
		SegPart* nodePart = seg->parts[ind + 1];
		seg->parts[ind + 1] = seg->parts[ind];
		seg->parts[ind] = nodePart;
	}
}

static void moveDown(PathSegment* seg, int ind)
{
	if (ind - 1 > -1)
	{
		SegPart* nodePart = seg->parts[ind - 1];
		seg->parts[ind - 1] = seg->parts[ind];
		seg->parts[ind] = nodePart;
	}
}

static void nodePartButtons(PathSegment* seg, int i)
{
	if (ImGui::Button("x"))
	{
		removePart(seg, i);
	}
	ImGui::SameLine();
	if (ImGui::Button("^"))
	{
		moveDown(seg, i);
	}
	ImGui::SameLine();
	if (ImGui::Button("v"))
	{
		moveUp(seg, i);
	}
}

ImGuiClass::ImGuiClass(int _windowSize)
{
	windowSize = _windowSize;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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

void ImGuiClass::nodeList(NodeGrid* grid)
{
	// node list

	ImGui::Begin("Node List", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::BeginMenuBar();
	if (ImGui::MenuItem("new"))
	{
		grid->reset();
		grid->selected.ind = -1;
		grid->selected.type = TypeNode;
		Save::clearPath();
	}
	if (ImGui::MenuItem("save"))
	{
		Save::save(grid);
	}
	if (ImGui::MenuItem("save as"))
	{
		memset(path, 0, 256);
		explorerMode = 1;
		explorer.reset(FileExplorerFlags_MakeFile);
	}
	if (ImGui::MenuItem("load"))
	{
		memset(path, 0, 256);
		explorerMode = 2;
		explorer.reset();
	}
	if (ImGui::MenuItem("export"))
	{
		Save::exp(grid);
	}
	if (ImGui::BeginMenu("robot"))
	{
		if (ImGui::MenuItem("upload current"))
		{
			std::thread t([grid]() { Upload::upload(true); });
			t.detach();
		}
		if (ImGui::MenuItem("upload all"))
		{
			std::thread t([grid]() { Upload::upload(false); });
			t.detach();
		}
		if (ImGui::MenuItem("pull from robot"))
		{
			std::thread t([grid]() { Upload::pull(); });
			t.detach();
		}
		if (ImGui::MenuItem("remove from robot"))
		{
			std::thread t([grid]() {
				// upload.remove();
			});
			t.detach();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
	if (grid->err != "")
	{
		ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.0f, 0.0f, 1.0f});
		ImGui::Text("%s", grid->err.c_str());
		ImGui::PopStyleColor();
	}
	if (grid->msg != "")
		ImGui::Text("%s", grid->msg.c_str());
	ImGui::Text("%s", ("current path: " + Save::getPath()).c_str());
	if (grid->mods == 1)
	{
		ImGui::Text("add node");
	}
	else if (grid->mods == 2)
	{
		ImGui::Text("add segment");
	}
	else
	{
		ImGui::Text("select");
	}
	ImGui::InputInt("layer", &(grid->layer), 1, 1, 0);
	ImGui::DragFloat("other layer transparency", &grid->otherLayerA, 0.1f, 0.0f, 1.0f);
	ImGui::InputInt("recognitionId", &(grid->recognitionId));
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
	ImGui::Text("%s", ("nodes: " + std::to_string(grid->nodes.count) + "/" + std::to_string(maxNodes)).c_str());
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

	ImGui::Text("%s", ("segments: " + std::to_string(grid->segs.count) + "/" + std::to_string(maxSegs)).c_str());
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

	// node properties
	ImGui::Separator();
	if (grid->selected.type == TypeNode)
	{
		if (ImGui::TreeNodeEx("node properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			nodeUi(grid);
			ImGui::TreePop();
		}
	}
	else
	{
		if (ImGui::TreeNodeEx("segment properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			segUi(grid);
			ImGui::TreePop();
		}
	}
	ImGui::End();

	// file explorer
	if (explorerMode)
	{
		if (int i = explorer.render(".path"))
		{
			if (i == FileExplorerUpdate_PathSelected)
			{
				strcpy(path, explorer.outPath.string().c_str());
				if (explorerMode == 1)
				{
					Save::saveAs(grid, path);
				}
				if (explorerMode == 2)
				{
					Save::load(grid, path);
					grid->selected.ind = -1;
					grid->selected.type = TypeNode;
				}
				explorerMode = 0;
			}
			if (i == FileExplorerUpdate_Close)
			{
				explorerMode = 0;
			}
		}
	}
}

void ImGuiClass::nodeUi(NodeGrid* grid)
{
	if (grid->selected.ind > -1 && grid->selected.ind < grid->nodes.count)
	{
		PathNode* node = grid->nodes.get(grid->selected.ind);
		ImGui::Text("%s", (std::string("node: ") + std::to_string(grid->selected.ind)).c_str());
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
		}
		if (grid->selected.ind > 0)
		{
			ImGui::SameLine();
			if (ImGui::Button("+"))
			{
				ImGui::OpenPopup("add");
			}
			if (ImGui::BeginPopup("add"))
			{
				bool marker = false;
				bool delay = false;
				bool turn = false;
				for (int i = 0; i < node->parts.size(); i++)
				{
					NodePart* part = node->parts[i];
					switch (part->getId())
					{
					case 2:
						marker = true;
						break;
					case 3:
						delay = true;
						break;
					case 4:
						turn = true;
						break;
					}
				}
				if (!marker)
				{
					if (ImGui::MenuItem("marker"))
					{
						node->parts.push_back(new Marker());
					}
				}
				if (!delay)
				{
					if (ImGui::MenuItem("delay"))
					{
						node->parts.push_back(new Delay());
					}
				}
				if (!turn)
				{
					if (ImGui::MenuItem("turn"))
					{
						node->parts.push_back(new Turn());
					}
				}
				ImGui::EndPopup();
			}
		}

		ImGui::InputFloat2("pos", glm::value_ptr(node->pos));
		ImGui::InputFloat("angle", &(node->rot));
		ImGui::InputFloat("heading", &(node->heading));
		ImGui::InputInt("layer", &(node->layer), 1, 1, 0);
		ImGui::Separator();
		for (int i = 0; i < node->parts.size(); i++)
		{
			switch (node->parts[i]->getId())
			{
			case 2:
				if (ImGui::TreeNode("marker"))
				{
					Marker* marker = (Marker*)node->parts[i];
					nodePartButtons(node, i);
					ImGui::InputText("label", marker->text, 255);
					ImGui::TreePop();
				}
				break;
			case 3:
				if (ImGui::TreeNode("delay"))
				{
					Delay* delay = (Delay*)node->parts[i];
					nodePartButtons(node, i);
					ImGui::InputFloat("time", &(delay->time));
					ImGui::TreePop();
				}
				break;
			case 4:
				if (ImGui::TreeNode("turn"))
				{
					Turn* turn = (Turn*)node->parts[i];
					nodePartButtons(node, i);
					ImGui::InputFloat("angle", &(turn->angle));
					ImGui::TreePop();
				}
			}
		}
	}
}

void ImGuiClass::segUi(NodeGrid* grid)
{
	if (grid->selected.ind > -1 && grid->selected.ind < grid->segs.count)
	{
		PathSegment* seg = grid->segs.get(grid->selected.ind);
		ImGui::Text("%s", (std::string("segment: ") + std::to_string(grid->selected.ind)).c_str());
		if (ImGui::Button("remove"))
		{
			grid->segs.remove(grid->selected.ind);
		}
		ImGui::SameLine();
		if (ImGui::Button("+"))
		{
			ImGui::OpenPopup("add");
		}
		if (ImGui::BeginPopup("add"))
		{
			bool overides = false;
			for (int i = 0; i < seg->parts.size(); i++)
			{
				SegPart* part = seg->parts[i];
				switch (part->getId())
				{
				case 1:
					// overides = true;
					break;
				}
			}
			if (!overides)
			{
				if (ImGui::MenuItem("overides"))
				{
					seg->parts.push_back(new Overides());
				}
			}
			ImGui::EndPopup();
		}
		ImGui::InputInt("layer", &(seg->layer), 1, 1, 0);
		const char* headingModes[] = {"none", "linear", "constant", "spline"};

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
		const char* lineMode[] = {"spline", "line"};
		if (ImGui::BeginCombo("path type", lineMode[seg->pathType]))
		{
			for (int i = 0; i < 2; i++)
			{
				if (ImGui::Selectable(lineMode[i]))
				{
					seg->pathType = i;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::InputInt("recognition id", &(seg->recognitionId));
		ImGui::Separator();
		for (int i = 0; i < seg->parts.size(); i++)
		{
			switch (seg->parts[i]->getId())
			{
			case 1:
				if (ImGui::TreeNode("speed overides"))
				{
					Overides* overides = (Overides*)seg->parts[i];
					nodePartButtons(seg, i);
					ImGui::PushID(1);
					ImGui::Checkbox("", &(overides->vel));
					ImGui::PopID();
					ImGui::SameLine();
					ImGui::InputFloat("vel", &(overides->velV));

					ImGui::PushID(2);
					ImGui::Checkbox("", &(overides->accel));
					ImGui::PopID();
					ImGui::SameLine();
					ImGui::InputFloat("accel", &(overides->accelV));

					ImGui::PushID(3);
					ImGui::Checkbox("", &(overides->angVel));
					ImGui::PopID();
					ImGui::SameLine();
					ImGui::InputFloat("ang vel", &(overides->angVelV));

					ImGui::PushID(4);
					ImGui::Checkbox("", &(overides->angAccel));
					ImGui::PopID();
					ImGui::SameLine();
					ImGui::InputFloat("ang accel", &(overides->angAccelV));
					ImGui::TreePop();
				}
				break;
			}
		}
	}
}
