#include <Save.hpp>
#include <fstream>
#include <json/json.hpp>
#include <sstream>

static std::string path;

template <typename T> T getField(nlohmann::json json, const char* name, std::string msg, NodeGrid* grid)
{
	if (json.contains(name))
	{
		try
		{
			return json[name];
		}
		catch (nlohmann::json::type_error e)
		{
			grid->err = "wrong type for field \'" + std::string(name) + "\' in " + msg;
			throw grid->err;
		}
	}
	grid->err = "cant find field \'" + std::string(name) + "\' in " + msg;
	throw grid->err;
}

template <typename T> T getField(nlohmann::json json, const char* name, std::string msg, NodeGrid* grid, T t)
{
	if (json.contains(name))
	{
		try
		{
			return json[name];
		}
		catch (nlohmann::json::type_error e)
		{
			grid->err = "wrong type for field \'" + std::string(name) + "\' in " + msg;
			throw grid->err;
		}
	}
	return t;
}

void Save::saveAs(NodeGrid* grid, const std::string& _path)
{
	path = _path;
	save(grid);
}
std::string& Save::getPath()
{
	return path;
}

void Save::clearPath()
{
	path = "";
}

void Save::save(NodeGrid* grid)
{
	grid->msg = "";
	grid->err = "";
	nlohmann::json json;
	json = {};
	for (int i = 0; i < grid->nodes.count; i++)
	{
		PathNode* node = grid->nodes.get(i);
		json[i] = {{"pos", {{"x", node->pos.x}, {"y", node->pos.y}}},
				   {"rot", node->rot},
				   {"heading", node->heading},
				   {"layer", node->layer}};
		int j = 0;
		for (NodePart* part : node->parts)
		{
			switch (part->getId())
			{
			case 2: {
				Marker* marker = (Marker*)part;
				json[i]["other"][j] = {{"text", marker->text}};
				break;
			}
			case 3: {
				Delay* delay = (Delay*)part;
				json[i]["other"][j] = {{"time", delay->time}};
				break;
			}
			case 4: {
				Turn* turn = (Turn*)part;
				json[i]["other"][j] = {{"angle", turn->angle}};
				break;
			}
			}
			j++;
		}
	}

	nlohmann::json jseg = {};
	for (int i = 0; i < grid->segs.count; i++)
	{
		PathSegment* seg = grid->segs.get(i);
		jseg[i] = {{"startNode", seg->startNode}, {"endNode", seg->endNode}, {"heading", seg->headingMode},
				   {"path", seg->pathType},		  {"layer", seg->layer},	 {"recognitionId", seg->recognitionId}};
		int j = 0;
		for (SegPart* part : seg->parts)
		{
			switch (part->getId())
			{
			case 1: {
				Overides* overides = (Overides*)part;
				jseg[i]["other"][j] = {{"vel", {overides->vel, overides->velV}},
									   {"accel", {overides->accel, overides->accelV}},
									   {"angVel", {overides->angVel, overides->angVelV}},
									   {"angAccel", {overides->angAccel, overides->angAccelV}}};
				break;
			}
			}
			j++;
		}
	}

	std::string _path = path;
	if (_path.find("\\") != std::string::npos)
	{
		_path = _path.substr(path.find("\\") + 1);
	}
	std::string folders = "save\\";
	while (_path.find("\\") != std::string::npos)
	{
		std::string folder = _path.substr(0, _path.find("\\") + 1);
		folders += folder;
		if (!std::filesystem::exists(folders))
		{
			std::filesystem::create_directory(folders);
		}
		_path = _path.substr(_path.find("\\") + 1);
	}
	grid->msg = "saved: " + path;
	std::ofstream fout(path);
	nlohmann::json j = {{"nodes", json}, {"segs", jseg}};
	fout << j.dump(4);
}

bool Save::load(NodeGrid* grid, const std::string& _path)
{
	try
	{
		grid->msg = "";
		grid->err = "";
		path = _path;
		std::ifstream stream(path);
		if (!stream.good())
		{
			grid->err = "file " + path + " doesnt exist";
			return false;
		}
		std::stringstream sstream;
		sstream << stream.rdbuf();
		nlohmann::json json = nlohmann::json::parse(sstream);

		int i = 0;
		std::vector<PathNode> nodes;
		for (auto jNode : json["nodes"])
		{
			nodes.push_back({});
			PathNode* node = &nodes[nodes.size() - 1];
			nlohmann::json pos = getField<nlohmann::json>(jNode, "pos", "node " + std::to_string(i), grid);
			node->pos.x = getField<float>(pos, "x", "node " + std::to_string(i) + " position", grid);
			node->pos.y = getField<float>(pos, "y", "node " + std::to_string(i) + " position", grid);
			node->layer = getField<int>(jNode, "layer", "node " + std::to_string(i), grid);
			node->rot = getField<float>(jNode, "rot", "node " + std::to_string(i), grid);
			node->heading = getField<float>(jNode, "heading", "node " + std::to_string(i), grid);

			if (jNode.contains("other"))
			{
				nlohmann::json parts = getField<nlohmann::json>(jNode, "other", "node " + std::to_string(i), grid);
				int i2 = 0;
				for (nlohmann::json jPart : parts)
				{
					if (jPart.contains("text"))
					{
						Marker* marker = new Marker();
						std::string text = getField<std::string>(
							jPart, "text", "node " + std::to_string(i) + " other: " + std::to_string(i2), grid);
						strcpy(marker->text, text.c_str());
						node->parts.push_back(marker);
					}
					if (jPart.contains("time"))
					{
						Delay* delay = new Delay();
						delay->time = getField<float>(
							jPart, "time", "node " + std::to_string(i) + " other: " + std::to_string(i2), grid);
						node->parts.push_back(delay);
					}
					if (jPart.contains("angle"))
					{
						Turn* turn = new Turn();
						turn->angle = jPart["angle"];
						turn->angle = getField<float>(
							jPart, "angle", "node " + std::to_string(i) + " other: " + std::to_string(i2), grid);
						node->parts.push_back(turn);
					}
					i2++;
				}
				i++;
			}
		}
		i = 0;
		std::vector<PathSegment> segs;
		for (auto jNode : json["segs"])
		{
			segs.push_back({});
			PathSegment* seg = &segs[segs.size() - 1];
			seg->startNode = getField<int>(jNode, "startNode", "segment " + std::to_string(i), grid);
			seg->endNode = getField<int>(jNode, "endNode", "segment " + std::to_string(i), grid);
			seg->layer = getField<int>(jNode, "layer", "segment " + std::to_string(i), grid);
			seg->headingMode = getField<int>(jNode, "heading", "segment " + std::to_string(i), grid);
			seg->pathType = getField<int>(jNode, "path", "segment " + std::to_string(i), grid);
			seg->recognitionId = getField<int>(jNode, "recognitionId", "segment " + std::to_string(i), grid, -1);

			for (auto jPart : jNode["other"])
			{
				if (jPart.contains("vel"))
				{
					Overides* overides = new Overides();
					overides->vel = jPart["vel"][0];
					overides->velV = jPart["vel"][1];
					overides->accel = jPart["accel"][0];
					overides->accelV = jPart["accel"][1];
					overides->angVel = jPart["angVel"][0];
					overides->angVelV = jPart["angVel"][1];
					overides->angAccel = jPart["angAccel"][0];
					overides->angAccelV = jPart["angAccel"][1];
					seg->parts.push_back(overides);
				}
			}
			i++;
		}

		grid->reset();
		for (PathNode& node : nodes)
		{
			PathNode* node2 = grid->nodes.add();

			node2->pos = node.pos;
			node2->rot = node.rot;
			node2->layer = node.layer;
			node2->heading = node.heading;
			for (NodePart* part : node.parts)
			{
				node2->parts.push_back(part);
			}
		}

		for (PathSegment& seg : segs)
		{
			PathSegment* seg2 = grid->segs.add();

			seg2->startNode = seg.startNode;
			seg2->endNode = seg.endNode;
			seg2->layer = seg.layer;
			seg2->pathType = seg.pathType;
			seg2->headingMode = seg.headingMode;
			seg2->recognitionId = seg.recognitionId;

			for (SegPart* part : seg.parts)
			{
				seg2->parts.push_back(part);
			}
		}

		grid->msg = "loaded: " + path;
		return true;
	}
	catch (std::string e)
	{
		return false;
	}
}

void Save::exp(NodeGrid* grid)
{
	grid->msg = "";
	grid->err = "";
	uint8_t* segUsage = new uint8_t[grid->nodes.count];
	memset(segUsage, 0, grid->nodes.count);
	for (int i = 0; i < grid->segs.count; i++)
	{
		PathSegment* s = grid->segs.get(i);
		if (s->recognitionId == grid->recognitionId || s->recognitionId == -1)
		{
			segUsage[s->startNode] |= 1;
			segUsage[s->endNode] |= 2;
		}
	}
	int startInd = -1;
	bool emptyNodes = false;
	for (int j = 0; j < grid->nodes.count; j++)
	{
		if (segUsage[j] == 1)
		{
			if (startInd == -1)
			{
				startInd = j;
			}
			else
			{
				grid->err = "export error: path has multiple start nodes";
				return;
			}
		}
		if (segUsage[j] == 0)
		{
			emptyNodes = true;
		}
	}
	if (startInd == -1)
	{
		grid->err = "export error: cant find start node";
		return;
	}
	std::vector<int> segments;
	int targetInd = startInd;
	bool foundNode = true;
	while (foundNode)
	{
		foundNode = false;
		int foundInd = 0;
		for (int i = 0; i < grid->segs.count; i++)
		{
			PathSegment* seg = grid->segs.get(i);
			if (seg->recognitionId == grid->recognitionId || seg->recognitionId == -1)
			{
				if (seg->startNode == targetInd)
				{
					if (foundNode)
					{
						grid->err = "export error: fork found at node " + std::to_string((int)seg->startNode);
						return;
					}
					foundNode = true;
					foundInd = seg->endNode;
					for (int s : segments)
					{
						if (i == s)
						{
							grid->err = "export error: loop found at segment " + std::to_string(s);
							return;
						}
					}
					segments.push_back(i);
				}
			}
		}
		targetInd = foundInd;
	}

	std::stringstream sstream;
	float prevHeading;
	glm::vec2 prevPos;

	PathNode* startNode = grid->nodes.get(startInd);
	prevPos = startNode->pos;
	sstream << "drive.trajectorySequenceBuilder(new Pose2d(" << startNode->pos.x << ", " << startNode->pos.y
			<< ", Math.toRadians(" << -((startNode->rot) - 90) << ")" << "))\n";

	for (int i = 0; i < segments.size(); i++)
	{
		PathSegment* seg = grid->segs.get(segments[i]);
		PathNode* node = grid->nodes.get(seg->endNode);

		std::stringstream func;
		func << "	.";
		if (seg->pathType == 0)
		{
			func << "sp";
		}
		bool ins = false;
		std::stringstream pose;
		pose << "(" << node->pos.x << ", " << node->pos.y << ", Math.toRadians(" << -((node->heading) - 90) << "))";
		std::stringstream vec;
		vec << "(" << node->pos.x << ", " << node->pos.y << ")";
		std::stringstream ang;
		ang << ", Math.toRadians(" << -((node->rot) - 90) << ")";
		bool constantHeading = false;
		switch (seg->headingMode)
		{
		case 0:
			func << "lineTo(new Vector2d" << vec.str();
			if (seg->pathType == 0)
			{
				func << ang.str();
			}
			func << ")\n";
			break;
		case 1:
			func << "lineToLinearHeading(new Pose2d" << pose.str();
			if (seg->headingMode == 1 && seg->pathType == 0)
			{
				func << ang.str();
			}
			func << ")\n";
			break;
		case 2:
			func << "lineToConstantHeading(new Vector2d" << vec.str();
			constantHeading = true;
			if (seg->pathType == 0)
			{
				func << ang.str();
			}
			func << ")\n";
			break;
		case 3:
			func << "lineToSplineHeading(new Pose2d" << pose.str() << ")\n";
			break;
		}
		if (node->pos != prevPos)
		{
			sstream << func.str();
			if (!constantHeading)
			{
				prevHeading = node->rot;
			}
			for (NodePart* part : node->parts)
			{
				switch (part->getId())
				{
				case NodePartTurn: {
					Turn* turn = (Turn*)part;
					float angle = -(node->heading - prevHeading);
					if (abs(angle) > 180)
					{
						angle -= 360 * (angle > 0 ? 1 : -1);
					}
					sstream << "	.turn(Math.toRadians(" << angle << "))\n";
					break;
				}
				case NodePartDelay: {
					Delay* delay = (Delay*)part;
					sstream << "	.waitSeconds(" << delay->time << ")\n";
					break;
				}
				case NodePartMarker: {
					Marker* marker = (Marker*)part;
					sstream << "	.addDisplacementMarker(() -> {\n";
					sstream << "		System.out.println(" << marker->text << ");\n";
					sstream << "	})\n";
					break;
				}
				}
			}
		}
		prevPos = node->pos;
		prevHeading = node->rot;
	}
	sstream << "	.build();";

	std::string _path = path;
	if (_path.find("\\") != std::string::npos)
	{
		_path = _path.substr(path.find("\\") + 1);
	}
	std::string folders = "";
	while (_path.find("\\") != std::string::npos)
	{
		std::string folder = _path.substr(0, _path.find("\\") + 1);
		folders += folder;
		if (!std::filesystem::exists("export\\" + folders))
		{
			std::filesystem::create_directory("export\\" + folders);
		}
		_path = _path.substr(_path.find("\\") + 1);
	}
	if (_path.find(".") != std::string::npos)
	{
		_path = _path.substr(0, _path.find("."));
	}
	std::ofstream fout("export\\" + folders + _path + ".java");
	fout << sstream.str();
	grid->msg = "exported: export\\" + folders + _path + ".java";
}
