#pragma once
#include <glad/glad.h>
#include <iostream>

class Texture{
	public:
	Texture(const std::string& path);
	~Texture();

	int getWidth(){return width;}
	int getHeight(){return height;}

	void bind();
	int getId(){return id;}

	bool operator==(const Texture& other){
		return other.id == id;
	}

	private:
	int width;
	int height;
	GLuint id;
	int format;
	int intFormat;
};