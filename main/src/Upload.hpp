#pragma once

class NodeGrid;

class Upload
{
  public:
	static void init(NodeGrid* nodeGrid);
  static void closeSock();
	static void upload(bool current);
  static void pull();
  static void remove();
};
