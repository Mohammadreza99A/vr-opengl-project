#ifndef HOUSE_H
#define HOUSE_H

#include "shader.h"
#include "object.h"
#include "stb_image.h"

class House
{

private:
    Object *house;
    Shader *shader;
    GLuint house_texture_id;

public:
    House();
    void cleanup();
    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos);

    void initTexture(std::string path);

    void bindAllTexture();
};

#endif