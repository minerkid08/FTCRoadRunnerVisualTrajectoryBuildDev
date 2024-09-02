#pragma once
#include "NodeGrid.hpp"

class Save
{
  public:
	static void save(NodeGrid* grid);
	static void saveAs(NodeGrid* grid, const std::string& _path);
	static bool load(NodeGrid* grid, const std::string& _path);
	static void exp(NodeGrid* grid);
	static void clearPath();
	static std::string& getPath();
};
