#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class Texture {

	public:
		Texture(const std::string& filePath, bool hasAlpha = true);
		~Texture();

		void bind(unsigned int slot = 0);
		void unbind() const;
		
		inline void free(); // Clear out local buffer
		inline unsigned int getWidth() const { return width_; }
		inline unsigned int getHeight() const { return height_; }
		inline unsigned int getChannels() const { return channels_; }

		inline int getSlot() const { return slot_; }
		// Slot is set through bind()
		
		inline const std::string& getFilePath() const { return filePath_; }
		inline bool hasAlpha() const { return hasAlpha_; }
		inline GLuint getID() const { return texID_; }

	private:
		GLuint texID_;
		int width_, height_, channels_;
		int slot_;	// Every texture must be bound to a slot before being used for drawing
		std::string filePath_;
		unsigned char* localBuffer_;
		bool hasAlpha_;
};