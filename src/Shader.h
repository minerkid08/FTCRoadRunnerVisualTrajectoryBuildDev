#pragma once
#include <glad/glad.h>
#include <iostream>

class Shader{
	public:
	Shader(const std::string& vert, const std::string& frag);
	~Shader();
	void use();
	void setUniform(const std::string& name, int val);
	void setUniform(const std::string& name, float val);
	private:
	int prgmId;
	int makeShader(GLenum type, const std::string& src);
};