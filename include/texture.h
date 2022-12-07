#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <IL/il.h>

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

#include <iostream>

// Inspired by the provided code in the practicals of the course
class Texture
{
private:
    unsigned int textID;
    unsigned char *dataBuffer;
    std::string filePath;
    int width, height, nrChannels;
    bool flip;

public:
    Texture(const std::string &path, const bool vflip);
    ~Texture();
    void bind(unsigned int slot = 0) const;
    void unbind() const;
};

#endif