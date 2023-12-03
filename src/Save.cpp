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
			{"layer", node->layer},
			{"line", node->line}
		};
		if(node->overides.hasOverides){
			json[i]["overides"] = {
				{"vel", {node->overides.vel, node->overides.velV}},
				{"accel", {node->overides.accel, node->overides.accelV}},
				{"angVel", {node->overides.angVel, node->overides.angVelV}},
				{"angAccel", {node->overides.angAccel, node->overides.angAccelV}},
			};
		}
		if(node->marker.hasMarker){
			json[i]["marker"] = node->marker.text;
		}
		if(node->delay.hasDelay){
			json[i]["delay"] = node->delay.time;
		}
	}
	std::string _path = path;
	if(_path.find("\\") != std::string::npos){
		_path = _path.substr(path.find("\\") + 1);
	}
	if(_path.find("\\") != std::string::npos){
		std::string folder = _path.substr(0, _path.find("\\") + 1);
		if(!std::filesystem::exists("save\\" + folder)){
			std::filesystem::create_directory("save\\" + folder);
		}
	}
	std::cout << "saved: " << path << "\n";
	std::ofstream fout(path);
	fout << json.dump(4);
}

void Save::load(NodeGrid* grid, const std::string& _path){
	path = _path;
	std::ifstream stream(path);
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
		if(jNode.contains("marker")){
			node->marker.hasMarker = true;
			std::string text = jNode["marker"];
			strcpy(node->marker.text, text.c_str());
		}
		if(jNode.contains("delay")){
			node->delay.hasDelay = true;
			node->delay.time = jNode["delay"];
		}
		node->line = jNode["line"];
		if(jNode.contains("overides")){
			nlohmann::json overides = jNode["overides"];
			node->overides.hasOverides = true;
			node->overides.vel = overides["vel"][0];
			node->overides.velV = overides["vel"][1];
			node->overides.accel = overides["accel"][0];
			node->overides.accelV = overides["accel"][1];
			node->overides.angVel = overides["angVel"][0];
			node->overides.angVelV = overides["angVel"][1];
			node->overides.angAccel = overides["angAccel"][0];
			node->overides.angAccelV = overides["angAccel"][1];
		}
		i++;
	}
	grid->nodeCount = i;
	std::cout << "loaded: " << path << "\n";
}

void Save::exp(NodeGrid* grid){
	std::stringstream sstream;
	float prevHeading;
	glm::vec2 prevPos;
	for(int i = 0; i < grid->nodeCount; i++){
		PathNode* node = (grid->nodes + i);
		std::stringstream func;
		func << "	.";
		if(!node->line){
			func << "sp";
		}
		bool ins = false;
		std::stringstream pose;
		pose << "(" << node->pos.x << ", " << node->pos.y << ", Math.toRadians(" << -((node->turnAfterMove ? prevHeading : node->rot) - 90) << "))";
		std::stringstream vec;
		vec << "(" << node->pos.x << ", " << node->pos.y << ")";
		std::stringstream ang;
		ang << ", Math.toRadians(" << -((node->turnAfterMove ? prevHeading : node->rot) - 90) << ")";
		switch(node->headingMode){
			case 0:
				func << "lineTo(new Vector2d" << vec.str();
				if(!node->line){
					func << ang.str();
				}
				func << ")\n";
				break;
			case 1:
				func << "lineToLinearHeading(new Pose2d" << pose.str() << ")\n";
				break;
			case 2:
				func << "lineToConstantHeading(new Vector2d" << vec.str();
				if(!node->line){
					func << ang.str();
				}
				func << ")\n";
				break;
			case 3:
				func << "lineToSplineHeading(new Pose2d" << vec.str() << ")\n";
				break;
		}
		if(i == 0){
			sstream << "drive.trajectorySequenceBuilder(new Pose2d(" << node->pos.x << ", " << node->pos.y << ", Math.toRadians(" << -(node->rot - 90) << ")))\n";
		}else if(node->turnAfterMove){
			if(node->pos != prevPos){
				sstream << func.str();
			}
			float angle = -(node->rot - prevHeading);
			if(abs(angle) > 180){
				angle -= 360 * (angle > 0 ? 1 : -1);
			}
			sstream << "	.turn(Math.toRadians(" << angle << "))\n"; 
		}else{
			sstream << func.str();
		}
		prevPos = node->pos;
		prevHeading = node->rot;
		if(node->marker.hasMarker){
			sstream << "	.addDisplacementMarker(() -> {\n";
			sstream << "		System.out.println(" << node->marker.text <<");\n";
			sstream << "	})\n";
		}
		if(node->delay.hasDelay){
			sstream << "	.waitSeconds(" << node->delay.time << ")\n";
		}
	}
	sstream << "	.build();";
	std::string _path = path;
	if(_path.find("\\") != std::string::npos){
		_path = _path.substr(path.find("\\") + 1);
	}
	if(_path.find("\\") != std::string::npos){
		std::string folder = _path.substr(0, _path.find("\\") + 1);
		if(!std::filesystem::exists("export/" + folder)){
			std::filesystem::create_directory("export/" + folder);
		}
	}
	if(_path.find(".") != std::string::npos){
		_path = _path.substr(0, _path.find("."));
	}
	std::cout << "exported: export\\" << _path << ".java\n";
	std::ofstream fout("export\\" + _path + ".java");
	fout << sstream.str();
}