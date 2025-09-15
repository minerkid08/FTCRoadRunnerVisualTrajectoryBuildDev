#pragma once

#include "renderer/Renderer.hpp"

#define Trajectory_RR 1

class Trajectory
{
  public:
	int type = 0;
	bool visible = false;
	bool canDelete = false;

	virtual ~Trajectory()
	{
	}
	virtual void update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int mods)
	{
	}
	virtual void mouseClick(int mouseX, int mouseY, int windowSize, int mods)
	{
	}

	virtual void render(Renderer& renderer, float transparency, float z, bool showSelected)
	{
	}

	virtual void flipVert()
	{
	}
	virtual void flipHoriz()
	{
	}

	virtual void reset()
	{
	}
};
