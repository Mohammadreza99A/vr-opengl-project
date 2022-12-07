#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <IL/il.h>

#include <iostream>
#include "stb_image.h"

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
    Texture(const std::string &path, const bool vflip = true);
    ~Texture();
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;
};

#endif