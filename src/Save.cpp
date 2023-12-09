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
			{"layer", node->layer},
			{"line", node->line}
		};
		int j = 0;
		for(NodePart* part : node->parts){
			switch(part->getId()){
				case 1:{
					Overides* overides = (Overides*)part;
					json[i]["other"][j] = {
						{"vel", {overides->vel, overides->velV}},
						{"accel", {overides->accel, overides->accelV}},
						{"angVel", {overides->angVel, overides->angVelV}},
						{"angAccel", {overides->angAccel, overides->angAccelV}}
					};
					break;
				}
				case 2:{
					Marker* marker = (Marker*)part;
					json[i]["other"][j] = {
						{"text", marker->text}
					};
					break;
				}
				case 3:{
					Delay* delay = (Delay*)part;
					json[i]["other"][j] = {
						{"time", delay->time}
					};
					break;
				}
				case 4:{
					Turn* turn = (Turn*)part;
					json[i]["other"][j] = {
						{"angle", turn->angle}
					};
					break;
				}
			}
			j++;
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
		for(auto jPart : jNode["other"]){
			if(jPart.contains("vel")){;
				Overides* overides = new Overides();
				overides->vel = jPart["vel"][0];
				overides->velV = jPart["vel"][1];
				overides->accel = jPart["accel"][0];
				overides->accelV = jPart["accel"][1];
				overides->angVel = jPart["angVel"][0];
				overides->angVelV = jPart["angVel"][1];
				overides->angAccel = jPart["angAccel"][0];
				overides->angAccelV = jPart["angAccel"][1];
				node->parts.push_back(overides);
			}
			if(jPart.contains("text")){
				Marker* marker = new Marker();
				std::string text = jPart["text"];
				strcpy(marker->text, text.c_str());
				node->parts.push_back(marker);
			}
			if(jPart.contains("time")){
				Delay* delay = new Delay();
				delay->time = jPart["time"];
				node->parts.push_back(delay);
			}
			if(jPart.contains("angle")){
				Turn* turn = new Turn();
				turn->angle = jPart["angle"];
				node->parts.push_back(turn);
			}
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
		pose << "(" << node->pos.x << ", " << node->pos.y << ", Math.toRadians(" << -((node->rot) - 90) << "))";
		std::stringstream vec;
		vec << "(" << node->pos.x << ", " << node->pos.y << ")";
		std::stringstream ang;
		ang << ", Math.toRadians(" << -((node->rot) - 90) << ")";
		bool constantHeading = false;
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
				constantHeading = true;
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
			sstream << "drive.trajectorySequenceBuilder(new Pose2d(" << node->pos.x << ", " << node->pos.y << ang.str() << ")))\n";
		}else{
			if(node->pos != prevPos){
				sstream << func.str();
				if(!constantHeading){
					prevHeading = node->rot;
				}
				for(NodePart* part : node->parts){
					switch(part->getId()){
						case NodePartTurn:{
							Turn* turn = (Turn*)part;
							float angle = -(node->rot - prevHeading);
							if(abs(angle) > 180){
								angle -= 360 * (angle > 0 ? 1 : -1);
							}
							sstream << "	.turn(Math.toRadians(" << angle << "))\n";
							break;
						}
						case NodePartDelay:{
							Delay* delay = (Delay*)part;
							sstream << "	.waitSeconds(" << delay->time << ")\n";
							break;
						}
						case NodePartMarker:{
							Marker* marker = (Marker*)part;
							sstream << "	.addDisplacementMarker(() -> {\n";
							sstream << "		System.out.println(" << marker->text <<");\n";
							sstream << "	})\n";
							break;
						}
					}
				}
			}
		}
		prevPos = node->pos;
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