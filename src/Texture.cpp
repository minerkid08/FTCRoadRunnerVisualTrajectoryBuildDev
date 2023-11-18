#include <Texture.h>

#include <stb/stbImage.h>

Texture::Texture(const std::string& path){
	stbi_set_flip_vertically_on_load(1);
	int channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if(data == nullptr){
		std::cout << "Failed to load image: " << path << "\n";
	}
	format = (channels == 4 ? GL_RGBA : GL_RGB);
	intFormat = (channels == 4 ? GL_RGBA8 : GL_RGBA8);
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTextureStorage2D(id, 1, (channels == 4 ? GL_RGBA8 : GL_RGB8), width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureSubImage2D(id, 0, 0, 0, width, height, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
}

Texture::~Texture(){
	glDeleteTextures(1, &id);
}

void Texture::bind(int i){
	glBindTextureUnit(i, id);
}