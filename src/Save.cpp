#include <Save.h>
#include <json/json.hpp>
#include <fstream>
#include <sstream>

static std::string path;

void Save::saveAs(NodeGrid* grid, const std::string& _path){
	path = _path;
	save(grid);
}
std::string& Save::getPath(){
	return path;
}

void Save::clearPath(){
	path = "";
}

void Save::save(NodeGrid* grid){
	nlohmann::json json;
	json = {};
	for(int i = 0; i < grid->nodeCount; i++){
		PathNode* node = (grid->nodes + i);
		json[i] = {
			{"pos", {
				{"x", node->pos.x}, 
				{"y", node->pos.y}
				}},
			{"rot", node->rot},
			{"headingMode", node->headingMode},
			{"turnAfterMove", node->turnAfterMove},
			{"marker", node->marker},
			{"layer", node->layer},
			{"line", node->line},
			{"overides", {
				{"vel", {node->overides.vel, node->overides.velV}},
				{"accel", {node->overides.accel, node->overides.accelV}},
				{"angVel", {node->overides.angVel, node->overides.angVelV}},
				{"angAccel", {node->overides.angAccel, node->overides.angAccelV}},
			}}
		};
	}
	if(path.find("/") != std::string::npos){
		std::string folder = path.substr(0, path.find("/"));
		if(!std::filesystem::exists("save/" + folder)){
			std::filesystem::create_directory("save/" + folder);
		}
	}

	std::ofstream fout("save/" + path + ".path");
	fout << json.dump(4);
}

void Save::load(NodeGrid* grid, const std::string& _path){
	path = _path;
	std::ifstream stream("save/" + path + ".path");
	if(!stream.good()){
		std::cout << "file " << path << " doesnt exist\n";
		return;
	}
	std::stringstream sstream;
	sstream << stream.rdbuf();
	nlohmann::json json = nlohmann::json::parse(sstream);
	
	int i = 0;
	for(auto jNode : json){	
		PathNode* node = (grid->nodes + i);
		node->pos = {
			jNode["pos"]["x"],
			jNode["pos"]["y"]
		};
		node->layer = jNode["layer"];
		node->rot = jNode["rot"];
		node->headingMode = jNode["headingMode"];
		node->turnAfterMove = jNode["turnAfterMove"];
		node->marker = jNode["marker"];
		node->line = jNode["line"];
		nlohmann::json overides = jNode["overides"];
		node->overides.vel = overides["vel"][0];
		node->overides.velV = overides["vel"][1];
		node->overides.accel = overides["accel"][0];
		node->overides.accelV = overides["accel"][1];
		node->overides.angVel = overides["angVel"][0];
		node->overides.angVelV = overides["angVel"][1];
		node->overides.angAccel = overides["angAccel"][0];
		node->overides.angAccelV = overides["angAccel"][1];
		i++;
	}
	grid->nodeCount = i;
}

void Save::exp(NodeGrid* grid){
	std::stringstream sstream;
	float prevHeading;
	glm::vec2 prevPos;
	for(int i = 0; i < grid->nodeCount; i++){
		PathNode* node = (grid->nodes + i);
		std::string func;
		bool ins = false;
		switch(node->headingMode){
			case 0:
				func = "lineTo(new Vector2d";
				ins = true;
				break;
			case 1:
				func = "lineToLinearHeading(new Pose2d";
				break;
			case 2:
				func = "lineToConstantHeading(new Vector2d";
				ins = true;
				break;
			case 3:
				func = "lineToSplineHeading(new Pose2d";
				break;
		}
		if(!node->line){
			func = "sp" + func;
			ins = true;
		}
		if(i == 0){
			sstream << "drive.trajectorySequenceBuilder(new Pose2d(" << node->pos.x << ", " << node->pos.y << ", Math.toRadians(" << -(node->rot - 90) << ")))\n";
		}else if(node->turnAfterMove){
			if(node->pos != prevPos){
				if(ins && node->line){
					sstream << "	." << func << "(" << node->pos.x << ", " << node->pos.y << "))\n";
				}else{
					sstream << "	." << func << "(" << node->pos.x << ", " << node->pos.y << (ins ? "), Math.toRadians(" : ", ") << -(prevHeading - 90) << "))\n";
				}
			}
			float angle = -(node->rot - prevHeading);
			if(abs(angle) > 180){
				angle -= 360 * (angle > 0 ? 1 : -1);
			}
			sstream << "	.turn(Math.toRadians(" << angle << "))\n"; 
		}else{
			if(ins && node->line){
				sstream << "	." << func << "(" << node->pos.x << ", " << node->pos.y << "))\n";
			}else{
				sstream << "	." << func << "(" << node->pos.x << ", " << node->pos.y << (ins ? "), Math.toRadians(" : ", ") << -(node->rot - 90) << "))\n";
			}
		}
		prevPos = node->pos;
		prevHeading = node->rot;
		if(node->marker){
			sstream << "	.addDisplacementMarker(() -> {\n";
			sstream << "		System.out.println(" << i <<");\n";
			sstream << "	})\n";
		}
	}
	sstream << "	.build();";
	if(path.find("/") != std::string::npos){
		std::string folder = path.substr(0, path.find("/"));
		if(!std::filesystem::exists("export/" + folder)){
			std::filesystem::create_directory("export/" + folder);
		}
	}
	std::ofstream fout("export/" + path + ".java");
	fout << sstream.str();
}