#include <Shader.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertSrc, const char* fragSrc){
	int vert = makeShader(GL_VERTEX_SHADER, vertSrc);
	int frag = makeShader(GL_FRAGMENT_SHADER, fragSrc);

	prgmId = glCreateProgram();
	glAttachShader(prgmId, vert);
	glAttachShader(prgmId, frag);
	glLinkProgram(prgmId);
	
	int status;
	glGetProgramiv(prgmId, GL_LINK_STATUS, &status);
	if(status == GL_FALSE){
		std::cout << "shader linking failed\n";
		glDeleteShader(vert);
		glDeleteShader(frag);
		return;
	}
	glDeleteShader(vert);
	glDeleteShader(frag);
}

Shader::~Shader(){
	glDeleteProgram(prgmId);
}

void Shader::use(){
	glUseProgram(prgmId);
}

void Shader::setUniform(const std::string& name, int value){
	int loc = glGetUniformLocation(prgmId, name.c_str());
	glUniform1i(loc, value);
}

void Shader::setUniform(const std::string& name, float value){
	int loc = glGetUniformLocation(prgmId, name.c_str());
	glUniform1f(loc, value);
}

void Shader::setUniform(const std::string& name, glm::vec2 value){
	int loc = glGetUniformLocation(prgmId, name.c_str());
	glUniform2f(loc, value.x, value.y);
}

void Shader::setUniform(const std::string& name, glm::vec4 value){
	int loc = glGetUniformLocation(prgmId, name.c_str());
	glUniform4f(loc, value.x, value.y, value.z, value.w);
}

int Shader::makeShader(GLenum type, const char* src)
{
	int id = glCreateShader(type);
	const GLchar* glSrc = src;
	glShaderSource(id, 1, &glSrc, 0);
	glCompileShader(id);
	int state;
	glGetShaderiv(id, GL_COMPILE_STATUS, &state);
	glErrorCheck();
	if(state == GL_FALSE)
	{
		int len = 256;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		char* err;
		err = new char[len];
		glGetShaderInfoLog(id, len, &len, err);
		std::cout << "shader error: " << err << "\n";
		glDeleteShader(id);
		return -1;
	}
	return id;
}