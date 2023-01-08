#ifndef WINDMILL_H
#define WINDMILL_H

#include "shader.h"
#include "object.h"
#include "stb_image.h"

class Windmill
{

private:
    Object *mill;
    Object *turbines;
    Shader *shader;
    GLuint windmill_texture_id;

public:
    Windmill();
    Shader *getShader();
    void cleanup();

    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos, float degree);

    void initTexture(std::string path);
    void turn(float degree);

    void bindAllTexture();
};
#endif