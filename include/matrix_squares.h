#ifndef MatrixSquares_H
#define MatrixSquares_H

#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "stb_image.h"

class MatrixSquares
{
public:


    MatrixSquares(unsigned int sub_x, unsigned int sub_y)
  :sub_x(sub_x),sub_y(sub_y), nb_vertices(sub_x * sub_y) {}

    void init();
    GLuint initTexture(char const *path);

protected:
    unsigned int sub_x;
    unsigned int sub_y;
    unsigned int nb_vertices;
    unsigned int nb_indices;
    GLfloat *vertices;
    GLfloat *normals;
    GLfloat *textures;
    GLuint *indices;
    
    GLuint _vao;
    GLuint _vbo;
    GLuint _vbo_normals;
    GLuint _vbo_textures;
    GLuint _vbo_idx;
    GLuint _pid;
    Shader *shader;
    GLuint texture_id;

    void fill_buffers();
    float function_flat(float x, float y)
   {
      return 0.0f;
   }
   float function_sin(float x, float y)
   {
      return sin(x * y * 10) / 10;
   }
   void set_indices();

   ~MatrixSquares();
   void normalize_vec3(float vec[3]);
   void cross_product(float vec_a[3], float vec_b[3], float vec_out[3]);
};

#endif
