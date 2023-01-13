#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "terrain.h"

class Camera
{
public:
    Camera();
    void init(float height, Terrain *terrain);
    void lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
    void getPosition(glm::vec3  position);
    void getDirection(glm::vec3 direction);
    void getCenter(float center[3]);
    glm::mat4x4 getMatrix();
    glm::mat4x4 getReflectionMatrix(float height);
    glm::mat4x4 getPerspectiveMat();
    void inputHandling(char key, float time_delta);
    void updatePos();
    void setWindowSize(unsigned int win_width, unsigned int win_height);
    void setSpeed(float speed);
    void setViewSpeed(float speed);
    void setHeight(float height);
    void setTerrain(Terrain *terrain);

protected:
    // for the look at function
    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;

    unsigned int win_width;
    unsigned int win_height;

    float angle_up;
    float angle_side;

    float angle_change_per_second;

    // units per seconds
    float speed;

    Terrain *terrain; // collision
    float height;

    void changeCamOrientation();
};

#endif