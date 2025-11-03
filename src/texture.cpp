#include <iostream>
#include "texture.hpp"
#include "vendor/stb_image.h" // Include stb_image implementation

Texture::Texture(const std::string& filePath, bool hasAlpha)
	: texID_(0), width_(0), height_(0), channels_(0),
	  filePath_(filePath), localBuffer_(nullptr), hasAlpha_(hasAlpha) {

	stbi_set_flip_vertically_on_load(1); // OpenGL expects 0,0 at bottom left
	localBuffer_ = stbi_load(filePath.c_str(), &width_, &height_, &channels_, 4);
	if (!localBuffer_) {
		std::cerr << "Failed to load texture: " << filePath << std::endl;
		return;
	}

	glGenTextures(1, &texID_);
	glBindTexture(GL_TEXTURE_2D, texID_);

	
	// Upload texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer_);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Tex params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind, done

	// Don't free in constructor, free in separate function AND destructor
}

Texture::~Texture() {
	if (localBuffer_) {
		stbi_image_free(localBuffer_);
		localBuffer_ = nullptr;
	}
	glDeleteTextures(1, &texID_);
}

void Texture::bind(unsigned int slot) {

	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texID_);
	slot_ = slot;

}
void Texture::unbind() const{

	glBindTexture(GL_TEXTURE_2D, 0);

}

inline void Texture::free() {
	if (localBuffer_) {
		stbi_image_free(localBuffer_);
		localBuffer_ = nullptr;
	}
}