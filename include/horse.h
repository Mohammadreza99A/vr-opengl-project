#ifndef STATUE_HORSE_H
#define STATUE_HORSE_H

#include "shader.h"
#include "object.h"
#include "stb_image.h"
#include <vector>

class Horse
{

private:
    Object *horse;
    Object *base;
    Shader *shaderHorse;
    Shader *shaderBase;
    GLuint horse_texture_id;
    GLuint base_id;
    std::vector<Shader *>shaders;

public:
    Horse(GLuint skyboxID);
    void cleanup();
    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos, const glm::vec3 &light_color);

    void render();
    void initTexture(std::string path);
    std::vector<Shader *>getShader();

    void bindAllTexture();
};

#endif