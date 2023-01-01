#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "stb_image.h"
#include "matrix_squares.h"

class Terrain: public MatrixSquares
{
public:
   Terrain(unsigned int sub_x, unsigned int sub_y);

   // sub_x and sub_y define subdivision of the terrain
   // it defines the definition of the terrain
   //  a 4x3 terrain will contain 12 quads
   void init();
   void draw(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection,
             glm::vec3 light_position, glm::vec3 camera_position);
   void cleanup();
   float get_height(float pos_x, float pos_y);
   void bindAllTexture();

private:

   const int SIZE=1000;
   std::vector<std::vector<float>> heightmap;
   std::vector<std::vector<std::vector<float>>> heightmap_normals;
   void generate_terrain();
   void generate_heightmap(float (Terrain::*func)(float, float),
                           unsigned int grid_x, unsigned int grid_y);
};

#endif
