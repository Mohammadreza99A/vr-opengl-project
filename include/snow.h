#ifndef SNOW_H
#define SNOW_H
#include "shader.h"
#include "object.h"
#include "stb_image.h"

class Snow
{

public:
   Snow(uint nbOfParticles);

   void setEmiterBoundary(glm::vec3 minCoords, glm::vec3 maxCoords);

   // will be rand between the two durations
   void setLifeDurationSec(float lifeSecMin, float lifeSecMax);

   // must be called every draw loop
   void setTime(float time);

   // variation to set init_vel_x (+- variation)
   void setInitialVelocity(glm::vec3 initialVelocity , glm::vec3 variation);

   void clean();

   void draw(glm::mat4x4 viewMatrix, glm::mat4x4 projectionMatrix, glm::vec3 &cameraPosition, glm::vec3 &lightPosition);

protected:
   struct Particle
   {
      float *position;
      glm::vec3 speed;
      float *lifeRemaining;
      float *age;
   };

   std::vector<Particle> particlesVector;

   uint nbOfParticles;
   Shader *shader;

   float lifeSecMin;
   float lifeSecMax;
   float currentTime;
   float prevTime;

   GLfloat *particlesPositions;
   GLfloat *particlesLife;

   GLuint _pid;
   GLuint vaoParticles;

   GLuint _vbo;
   GLuint _vboTex;
   GLuint _vboPointsPos;
   GLuint _vboPointsLife;

   glm::vec3 emiterBoudaryMin;
   glm::vec3 emiterBoudaryMax;

   // initial velocities
   glm::vec3 velocity;

   void windFunc(float pos[3], float ret[3], float time);

   void handleParticles();

   float mix(float min, float max, float ratio);

   // no wind
   static void defaultWind(float[3], float ret[3], float);
};

#endif