#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "shader.h"

class SkyBox
{
private:
    Shader *shader;
    std::vector<std::string> paths;

public:
    SkyBox();
    ~SkyBox();
    void load();
    void draw(const glm::mat4 &view, const glm::mat4 &projection,
              const glm::vec3 &camera_position, const glm::vec3 &light_pos);

    unsigned int textureID;
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
};

#endif