#include <Shader.h>

Shader::Shader(const std::string& vertSrc, const std::string& fragSrc){
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

int Shader::makeShader(GLenum type, const std::string& src){
	int id = glCreateShader(type);
	const GLchar* glSrc = src.c_str();
	glShaderSource(id, 1, &glSrc, 0);
	glCompileShader(id);
	int state;
	glGetShaderiv(id, GL_COMPILE_STATUS, &state);
	if(state == GL_FALSE){
		int len = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		char err[len];
		glGetShaderInfoLog(id, len, &len, err);
		std::cout << "shader error: " << err << "\n";
		glDeleteShader(id);
		return -1;
	}
	return id;
}