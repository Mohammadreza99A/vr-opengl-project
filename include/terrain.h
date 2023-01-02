#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "stb_image.h"

class Terrain
{
public:
   Terrain();

   // sub_x and sub_y define subdivision of the terrain
   // it defines the definition of the terrain
   //  a 4x3 terrain will contain 12 quads
   void init(unsigned int sub_x, unsigned int sub_y);
   void draw(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection,
             glm::vec3 light_position, glm::vec3 camera_position);
   void cleanup();
   float get_height(float pos_x, float pos_y);
   void initTexture(std::string path);
   void bindAllTexture();

private:
   GLuint _vao;
   GLuint _vbo;
   GLuint _vbo_normals;
   GLuint _vbo_textures;
   GLuint _vbo_idx;
   GLuint _pid;
   Shader *shader;

   GLuint terrain_texture_id;

   unsigned int sub_x;
   unsigned int sub_y;

   GLfloat *vertices = NULL;
   GLuint *indices = NULL;
   GLfloat *normals = NULL;
   GLfloat *textures = NULL;

   unsigned int nb_vertices;
   unsigned int nb_indices;

   std::vector<std::vector<float>> heightmap;
   std::vector<std::vector<std::vector<float>>> heightmap_normals;

   float function_flat(float x, float y)
   {
      return 0.0f;
   }

   float function_sin(float x, float y)
   {
      return sin(x * y * 10) / 10;
   }
   
   void generate_terrain();
   void set_indices();
   void generate_heightmap(float (Terrain::*func)(float, float),
                           unsigned int grid_x, unsigned int grid_y);
   void normalize_vec3(float vec[3]);
   void cross_product(float vec_a[3], float vec_b[3], float vec_out[3]);
};

#endif
