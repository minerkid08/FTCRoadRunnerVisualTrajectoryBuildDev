#pragma once
#include <NodeGrid.h>

class Save{
	public:
	static void save(NodeGrid* grid);
	static void saveAs(NodeGrid* grid, const std::string& _path);
	static void load(NodeGrid* grid, const std::string& _path);
	static void exp(NodeGrid* grid);
	static std::string& getPath();
};