#include <ImGui.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <Save.h>

static void removePart(PathNode* node, int ind){
	delete node->parts[ind];
	for(int i = ind; i < node->parts.size() - 1; i++){
		node->parts[i] = node->parts[i + 1];
	}
	node->parts.resize(node->parts.size() - 1);
}

static void moveUp(PathNode* node, int ind){
	if(ind + 1 < node->parts.size()){
		NodePart* nodePart = node->parts[ind + 1];
		node->parts[ind + 1] = node->parts[ind];
		node->parts[ind] = nodePart;
	}
}

static void moveDown(PathNode* node, int ind){
	if(ind - 1 > -1){
		NodePart* nodePart = node->parts[ind - 1];
		node->parts[ind - 1] = node->parts[ind];
		node->parts[ind] = nodePart;
	}
}

static void nodePartButtons(PathNode* node, int i){
	if(ImGui::Button("x")){
		removePart(node, i);
	}
	ImGui::SameLine();
	if(ImGui::Button("^")){
		moveDown(node, i);
	}
	ImGui::SameLine();
	if(ImGui::Button("v")){
		moveUp(node, i);
	}
}

ImGuiClass::ImGuiClass(int windowSize){
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
	style.Colors[ImGuiCol_WindowBg].x -1.0f;
	io.FontGlobalScale = windowSize == 1700 ? 2 : 1.5f;//2 for 4k
	style.ScaleAllSizes(windowSize == 1700 ? 2 : 1.5); //2 for 4k
	ImGui_ImplGlfw_InitForOpenGL(getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	path = new char[256];
	memset(path, 0, 256);
	explorer.setMainPath("save");
}

ImGuiClass::~ImGuiClass(){
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiClass::begin(){
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void ImGuiClass::end(){
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
		GLFWwindow* ctx = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(ctx);
	}
}

void ImGuiClass::nodeList(NodeGrid* grid){
	ImGui::Begin("Node List", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::BeginMenuBar();
	if(ImGui::MenuItem("new")){
		grid->nodeCount = 0;
		grid->selected = -1;
		Save::clearPath();
	}
	if(ImGui::MenuItem("save")){
		Save::save(grid);
	}
	if(ImGui::MenuItem("save as")){
		memset(path, 0, 256);
		explorerMode = 1;
		explorer.reset(FileExplorerFlags_MakeFile);
	}
	if(ImGui::MenuItem("load")){
		memset(path, 0, 256);
		explorerMode = 2;
		explorer.reset();
	}
	if(ImGui::MenuItem("export")){
		Save::exp(grid);
	}
	ImGui::EndMenuBar();
	ImGui::Text(("current path: " + Save::getPath()).c_str());
	ImGui::InputInt("layer", &(grid->layer), 1, 1, 0);
	if(ImGui::Button("flipHoriz")){
		grid->flipHoriz();
	}
	ImGui::SameLine();
	if(ImGui::Button("flipVert")){
		grid->flipVert();
	}
	if(ImGui::Button("^")){
		grid->moveUp(grid->selected);
	}
	ImGui::SameLine();
	if(ImGui::Button("v")){
		grid->moveDown(grid->selected);
	}
	ImGui::Text(("nodes: " + std::to_string(grid->nodeCount) + "/" + std::to_string(grid->maxNodes)).c_str());
	int j = 0;
	if(grid->nodeCount > 0){
		for(int i = 0; i < grid->nodeCount; i++){
			PathNode* node = (*grid)[i];

			ImVec4 tint(0.25f, 0.25f, 0.25f, 1);
			if(grid->selected == i){
				tint.x = 0.75f;
			}
			if(i == 0){
				tint.y = 0.75f;
			}
			//if(node->turnAfterMove){
			//	tint.z = 0.75f;
			//}

			ImGui::PushStyleColor(ImGuiCol_Button, tint);

			if(ImGui::Button(std::to_string(i).c_str())){
				grid->selected = i;
			}
			ImGui::PopStyleColor();
			if(j < 9){
				ImGui::SameLine();
				j++;
			}else{
				j = 0;
			}
		}
	}
	ImGui::End();
	if(explorerMode){
		if(int i = explorer.render(".path")){
			if(i == 1){
				strcpy(path, explorer.outPath.string().c_str());
				if(explorerMode == 1){
					Save::saveAs(grid, path);
				}
				if(explorerMode == 2){
					Save::load(grid, path);
					grid->selected = -1;
				}
				explorerMode = 0;
			}
			if(i == 2){
				explorerMode = 0;
			}
		}
	}
}

void ImGuiClass::nodeProperties(NodeGrid* grid){
	ImGui::Begin("Properties");
	if(grid->selected > -1 && grid->selected < grid->nodeCount){
		PathNode* node = (*grid)[grid->selected];
		ImGui::Text((std::string("node: ") + std::to_string(grid->selected)).c_str());
		if(ImGui::Button("remove")){
			grid->removeNode(grid->selected);
		}
		ImGui::SameLine();
		if(ImGui::Button("+")){
			ImGui::OpenPopup("add");
		}
		if(ImGui::BeginPopup("add")){
			bool overides = false;
			bool marker = false;
			bool delay = false;
			bool turn = false;
			for(int i = 0; i < node->parts.size(); i++){
				NodePart* part = node->parts[i];
				switch(part->getId()){
					case 1:
					overides = true;
					break;
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
			if(!overides){
				if(ImGui::MenuItem("overides")){
					node->parts.push_back(new Overides());
				}
			}
			if(!marker){
				if(ImGui::MenuItem("marker")){
					node->parts.push_back(new Marker());
				}
			}
			if(!delay){
				if(ImGui::MenuItem("delay")){
					node->parts.push_back(new Delay());
				}
			}
			if(!turn){
				if(ImGui::MenuItem("turn")){
					node->parts.push_back(new Turn());
				}
			}
			ImGui::EndPopup();
		}

		if(ImGui::TreeNode("transform")){
			ImGui::InputFloat2("pos", glm::value_ptr(node->pos));
			ImGui::InputFloat("rot", &(node->rot));
			ImGui::InputInt("layer", &(node->layer), 1, 1, 0);
			ImGui::TreePop();
		}

		if(ImGui::TreeNode("path")){
			const char* headingModes[] = {"none", "linear", "constant", "spline"};

			if(ImGui::BeginCombo("heading mode", headingModes[node->headingMode])){
				for(int i = 0; i < 4; i++){
					if(ImGui::Selectable(headingModes[i])){
						node->headingMode = i;
					}
				}
				ImGui::EndCombo();
			}
			const char* lineMode[] = {"spline", "line"};

			if(ImGui::BeginCombo("line", lineMode[node->line])){
				for(int i = 0; i < 2; i++){
					if(ImGui::Selectable(lineMode[i])){
						node->line = i;
					}
				}
				ImGui::EndCombo();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
		for(int i = 0; i < node->parts.size(); i++){
			switch(node->parts[i]->getId()){
				case 1:
					if(ImGui::TreeNode("speed overides")){
						Overides* overides = (Overides*)node->parts[i];
						nodePartButtons(node, i);
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
				case 2:
					if(ImGui::TreeNode("marker")){
						Marker* marker = (Marker*)node->parts[i];
						nodePartButtons(node, i);
						ImGui::InputText("label", marker->text, 255);
						ImGui::TreePop();
					}
					break;
				case 3:
					if(ImGui::TreeNode("delay")){
						Delay* delay = (Delay*)node->parts[i];
						nodePartButtons(node, i);
						ImGui::InputFloat("time", &(delay->time));
						ImGui::TreePop();
					}
					break;
				case 4:
					if(ImGui::TreeNode("turn")){
						Turn* turn = (Turn*)node->parts[i];
						nodePartButtons(node, i);
						ImGui::InputFloat("angle", &(turn->angle));
						ImGui::TreePop();
					}
			}
		}
	}
	ImGui::End();
}