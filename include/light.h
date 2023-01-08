#ifndef LIGHT_H
#define LIGHT_H

#include "shader.h"
#include "object.h"
#include "stb_image.h"

class Light
{

private:
    Shader *shader_light;
    float shininess;
    float ambient;
    float diffuse;
    float specular;
    glm::vec3 materialColour;

public:
    Light(float shininess, float ambient, float diffuse, float specular, glm::vec3 materialColour);
    void setLight(Shader *shader_light);

    void cleanup();
};
#endif