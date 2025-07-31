#include "FrameBuffer.hpp"
#include <glad/glad.h>
#include <iostream>

FrameBuffer::FrameBuffer(const FrameBufferSpec& _spec)
{
	regen(_spec);
}
FrameBuffer::~FrameBuffer()
{
	del();
}

void FrameBuffer::del()
{
	if (id)
	{
		id = 0;
		glDeleteFramebuffers(1, &id);
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &depth);
	}
}

void FrameBuffer::regen(const FrameBufferSpec& _spec)
{
	spec = _spec;
	if (id)
	{
		glDeleteFramebuffers(1, &id);
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &depth);
	}
	glCreateFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glCreateTextures(GL_TEXTURE_2D, 1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec.width, spec.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glCreateTextures(GL_TEXTURE_2D, 1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, spec.width, spec.height, 0, GL_DEPTH_STENCIL,
				 GL_UNSIGNED_INT_24_8, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Frame buffer could not be created\n";
	}
	unbind();
}
void FrameBuffer::resize(int width, int height)
{
	spec.width = width;
	spec.height = height;
	regen(spec);
}
void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glViewport(0, 0, spec.width, spec.height);
}
void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
