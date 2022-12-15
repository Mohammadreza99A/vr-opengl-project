#ifndef STATUE_HORSE_H
#define STATUE_HORSE_H

#include "shader.h"
#include "object.h"
#include "stb_image.h"

class Horse
{

private:
    Object *horse;
    Object *base;
    Shader *shader_spec;
    Shader *shader_norm;
    GLuint horse_texture_id;

public:
    Horse();
    void cleanup();
    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos);

    void render();
    void initTexture(std::string path);

    void bindAllTexture();
};

#endif