#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "stb_image.h"

#include "tools.h"


class PointLight
{
private:
    glm::vec3 color;
    float ambient;
    float diffuse;
    float specular;
    
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;

public:
    PointLight(glm::vec3 color, float ambient, float diffuse, float specular, glm::vec3 position, float constant, float linear, float quadratic);

    void setLight(Shader *shader_light);
    void setColor(Shader *shader_light, glm::vec3 color);
};
#endif