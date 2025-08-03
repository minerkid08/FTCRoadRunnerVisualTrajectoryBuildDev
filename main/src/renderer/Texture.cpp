#include "Texture.hpp"
#include <iostream>

#include <filesystem>
#include <stb/stbImage.h>

Texture::Texture(const std::string& path)
{
	stbi_set_flip_vertically_on_load(1);
	int channels;
	if (!std::filesystem::exists(path))
	{
		std::cout << "Texture does not exist: " << path << "\n";
	}
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (data == nullptr)
	{
		std::cout << "Failed to load image: " << path << "\n";
	}
	format = (channels == 4 ? GL_RGBA : GL_RGB);
	intFormat = (channels == 4 ? GL_RGBA8 : GL_RGBA8);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	//glTexStorage2D(id, 1, (channels == 4 ? GL_RGBA8 : GL_RGB8), width, height);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTextureSubImage2D(id, 0, 0, 0, width, height, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
  int fmt = (channels == 4 ? GL_RGBA : GL_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
}

Texture::~Texture()
{
  del();
}

void Texture::del()
{
	if (width != 0)
	{
		glDeleteTextures(1, &id);
		width = 0;
	}
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
}
