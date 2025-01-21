#pragma once
#include <cstdint>
struct FrameBufferSpec
{
	int width = 1280;
	int height = 720;
	int samples = 1;
	bool swapChainTarget = false;
};
class FrameBuffer
{
  public:
	FrameBufferSpec spec;
	FrameBuffer(const FrameBufferSpec& _spec);
	~FrameBuffer();
	void regen(const FrameBufferSpec& _spec);
	void resize(int width, int height);
	void bind();
	void unbind();
	uint32_t getColor()
	{
		return color;
	}

  private:
	uint32_t id = 0;
	uint32_t color = 0;
	uint32_t depth = 0;
};
