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
			{"turnAfterMove", node->turnAfterMove},
			{"marker", node->marker},
			{"layer", node->layer}
		};
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
		node->turnAfterMove = jNode["turnAfterMove"];
		node->marker = jNode["marker"];
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
		if(i == 0){
			sstream << "drive.trajectorySequenceBuilder(new Pose2d(" << node->pos.x << ", " << node->pos.y << ", Math.toRadians(" << -(node->rot - 90) << ")))\n";
		}else if(node->turnAfterMove){
			if(node->pos != prevPos){
				sstream << "	.splineTo(new Vector2d(" << node->pos.x << ", " << node->pos.y << "), Math.toRadians(" << -(prevHeading - 90) << "))\n";
			}
			float angle = -(node->rot - prevHeading);
			if(abs(angle) > 180){
				angle -= 360 * (angle > 0 ? 1 : -1);
			}
			sstream << "	.turn(Math.toRadians(" << angle << "))\n"; 
		}else{
			sstream << "	.splineTo(new Vector2d(" << node->pos.x << ", " << node->pos.y << "), Math.toRadians(" << -(node->rot - 90) << "))\n";
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
	std::ofstream fout("export/" + path + ".java");
	fout << sstream.str();
}