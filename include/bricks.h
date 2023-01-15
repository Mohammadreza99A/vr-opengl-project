#ifndef BRICKS_H
#define BRICKS_H

#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "stb_image.h"
#include "matrix_squares.h"

class Bricks : public MatrixSquares
{
public:
    Bricks(unsigned int sub_x, unsigned int sub_y);

    void transform(glm::vec3 translate, float radians, glm::vec3 rotate, glm::vec3 scale);

    ~Bricks();

    // sub_x and sub_y define subdivision of the terrain
    // it defines the definition of the terrain
    //  a 4x3 terrain will contain 12 quads
    void init();
    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, float heightScale, const glm::vec3 &light_pos);
    GLuint initTexture(char const *path);
    void cleanup();
    void bindAllTexture();
    void compute_tangents();

private:
    
    void fill_buffers();
    const int SIZE = 5;
    glm::mat4 model = glm::mat4(1.0);

    GLuint diffuseMap;
    GLuint normalMap;
    GLuint heightMap;

    void generate_bricks();
    GLfloat *tangent;

    GLuint _vbo_tg;
    GLuint _vbo_btg;
};

#endif
