#include <ImGui.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <Save.h>

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
	if(ImGui::Button("add")){
		grid->clickMode = 0;
	}
	ImGui::SameLine();
	if(ImGui::Button("select")){
		grid->clickMode = 1;
	}
	ImGui::SameLine();
	ImGui::Text(grid->clickMode == 0 ? "add" : "select");
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
			if(ImGui::Button(std::to_string(i).c_str())){
				grid->selected = i;
			}
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
		}
	}
}

void ImGuiClass::nodeProperties(NodeGrid* grid){
	ImGui::Begin("Properties");
	if(grid->selected > -1 && grid->selected < grid->nodeCount){
		PathNode* node = (grid->nodes + grid->selected);
		ImGui::Text((std::string("node: ") + std::to_string(grid->selected)).c_str());
		if(ImGui::Button("remove")){
			grid->removeNode(grid->selected);
		}
		ImGui::SameLine();
		if(ImGui::Button("+")){
			ImGui::OpenPopup("add");
		}
		if(ImGui::BeginPopup("add")){
			if(!node->marker.hasMarker){
				if(ImGui::MenuItem("marker")){
					node->marker.reset();
					node->marker.hasMarker = true;
				}
			}
			if(!node->delay.hasDelay){
				if(ImGui::MenuItem("delay")){
					node->delay.reset();
					node->delay.hasDelay = true;
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
			ImGui::Checkbox("turn after move", &(node->turnAfterMove));
			ImGui::Checkbox("line", &(node->line));
			ImGui::TreePop();
		}

		if(ImGui::TreeNode("speed overides")){
			ImGui::PushID(1);
			ImGui::Checkbox("", &(node->overides.vel));
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::InputFloat("vel", &(node->overides.velV));

			ImGui::PushID(2);
			ImGui::Checkbox("", &(node->overides.accel));
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::InputFloat("accel", &(node->overides.accelV));

			ImGui::PushID(3);
			ImGui::Checkbox("", &(node->overides.angVel));
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::InputFloat("ang vel", &(node->overides.angVelV));

			ImGui::PushID(4);
			ImGui::Checkbox("", &(node->overides.angAccel));
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::InputFloat("ang accel", &(node->overides.angAccelV));
			ImGui::TreePop();
		}

		if(node->marker.hasMarker){
			if(ImGui::TreeNode("marker")){
				if(ImGui::Button("-")){
					node->marker.hasMarker = false;
				}
				ImGui::InputText("label", node->marker.text, 255);
				ImGui::TreePop();
			}
		}
		if(node->delay.hasDelay){
			if(ImGui::TreeNode("delay")){
				if(ImGui::Button("-")){
					node->marker.hasMarker = false;
				}
				ImGui::InputFloat("time", &(node->delay.time));
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}