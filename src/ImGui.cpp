#include <ImGui.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <Save.h>

ImGuiClass::ImGuiClass(){
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
	io.FontGlobalScale = 1.5f;//2 for 4k
	style.ScaleAllSizes(1.5); //2 for 4k
	ImGui_ImplGlfw_InitForOpenGL(getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	path = new char[256];
	memset(path, 0, 256);
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
	}
	if(ImGui::MenuItem("save")){
		Save::save(grid);
	}
	if(ImGui::MenuItem("save as")){
		Save::saveAs(grid, path);
	}
	if(ImGui::MenuItem("load")){
		Save::load(grid, path);
	}
	if(ImGui::MenuItem("export")){
		Save::exp(grid);
	}
	ImGui::EndMenuBar();
	ImGui::InputText("load location", path, 256);
	if(ImGui::Button("add")){
		grid->clickMode = 0;
	}
	ImGui::SameLine();
	if(ImGui::Button("select")){
		grid->clickMode = 1;
	}
	ImGui::SameLine();
	ImGui::Text(grid->clickMode == 0 ? "add" : "select");
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
}

void ImGuiClass::nodeProperties(NodeGrid* grid){
	ImGui::Begin("Properties");
	if(grid->selected > -1 && grid->selected < grid->nodeCount){
		PathNode* node = (grid->nodes + grid->selected);
		ImGui::Text((std::string("node: ") + std::to_string(grid->selected)).c_str());
		ImGui::InputFloat2("pos", glm::value_ptr(node->pos));
		ImGui::InputFloat("rot", &(node->rot));
		ImGui::Checkbox("turn after move", &(node->turnAfterMove));
		ImGui::Checkbox("marker", &(node->marker));
		if(ImGui::Button("remove")){
			grid->removeNode(grid->selected);
		}
	}
	ImGui::End();
}