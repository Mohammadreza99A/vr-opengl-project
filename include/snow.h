#ifndef SNOW_H
#define SNOW_H
#include "shader.h"
#include "object.h"
#include "stb_image.h"

class SnowManager
{

public:
   SnowManager(uint nb_particles);

   ~SnowManager()
   {
   }

   void set_emiter_boundary(GLfloat minx, GLfloat maxx, GLfloat miny, GLfloat maxy, GLfloat minz, GLfloat maxz);

   // will be rand between the two durations
   void set_life_duration_sec(float life_sec_min, float life_sec_max);

   // must be called every draw loop
   void set_time(float time);

   // variation to set init_vel_x (+- variation)
   void set_initial_velocity(float init_vel_x, float init_vel_y, float init_vel_z, float variation_x, float variation_y, float variation_z);

   void clean();

   void draw(glm::mat4x4 view_matrix, glm::mat4x4 projection_matrix, GLfloat camera_position[3], GLfloat light_position[3]);

protected:
   struct Particle
   {
      float *position;
      glm::vec3 speed;
      float *life_remaining;
      float *age;
   };

   std::vector<Particle> lst_particles;

   uint nb_particles;
   Shader *shader;

   float life_sec_min;
   float life_sec_max;
   float current_time;
   float prev_time;

   GLfloat *particles_positions;
   GLfloat *particles_life;

   GLuint _pid;
   GLuint vao_particles;

   GLuint _vbo;
   GLuint _vbo_tex;
   GLuint _vbo_points_pos;
   GLuint _vbo_points_life;

   GLfloat emiter_boudary_min_x;
   GLfloat emiter_boudary_max_x;
   GLfloat emiter_boudary_min_y;
   GLfloat emiter_boudary_max_y;
   GLfloat emiter_boudary_min_z;
   GLfloat emiter_boudary_max_z;

   // initial velocities
   glm::vec3 velocity;

   void wind_func(float pos[3], float ret[3], float time);

   void handle_particles();

   float mix(float min, float max, float ratio);

   // no wind
   static void default_wind(float[3], float ret[3], float);
};

#endif
