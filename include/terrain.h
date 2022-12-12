#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class Terrain
{
private:
   GLuint _vao;
   GLuint _vbo;
   GLuint _vbo_normals;
   GLuint _vbo_idx;
   GLuint _pid;
   Shader *shader;

   unsigned int sub_x;
   unsigned int sub_y;

   GLfloat *vertices = NULL;
   GLuint *indices = NULL;
   GLfloat *normals = NULL;

   unsigned int nb_vertices;
   unsigned int nb_indices;

   std::vector<std::vector<float>> heightmap;
   std::vector<std::vector<std::vector<float>>> heightmap_normals;

   void generate_terrain();
   void set_indices();

public:
   Terrain();

   // sub_x and sub_y define subdivision of the terrain
   // it defines the definition of the terrain
   //  a 4x3 terrain will contain 12 quads
   void init(unsigned int sub_x, unsigned int sub_y);

   void draw(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection,
             glm::vec3 light_position, glm::vec3 camera_position);
   // draw with transform matrix in memory
   void draw(glm::mat4x4 view, glm::mat4x4 projection,
             glm::vec3 light_position, glm::vec3 camera_position);

   void cleanup();
};

#endif
