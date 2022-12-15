#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <iostream>

class SkyBox
{
private:
    std::vector<std::string> paths;

public:
    SkyBox(std::vector<std::string> paths);
    ~SkyBox();
    void load();
    void draw();

    unsigned int textureID;
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
};

#endif