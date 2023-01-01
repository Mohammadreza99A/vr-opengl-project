#ifndef BARREL_H
#define BARREL_H

#include "shader.h"
#include "object.h"
#include "stb_image.h"
#include "tools.h"

class Barrel
{

private:
    Object *barrel;
    Shader *shader;
    GLuint diffuseMap;
    GLuint normalMap;

public:
    Barrel();
    void cleanup();
    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos);

    void bindAllTexture();
};

#endif