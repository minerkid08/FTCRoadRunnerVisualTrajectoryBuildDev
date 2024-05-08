#pragma once

class NodeGrid;

class Upload
{
  public:
	Upload(NodeGrid* nodeGrid);
  void upload();

  private:
	NodeGrid* grid;
  bool opened = false;
};
