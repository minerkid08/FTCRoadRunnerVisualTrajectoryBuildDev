#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
class Shader
{
  public:
	Shader(const char* vert, const char* frag);
	~Shader();
	void use();
	void setUniform(const std::string& name, int val);
	void setUniform(const std::string& name, float val);
	void setUniform(const std::string& name, glm::vec2 val);
	void setUniform(const std::string& name, glm::vec4 val);

  private:
	int prgmId;
	int makeShader(GLenum type, const char* src);
};
