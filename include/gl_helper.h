#ifndef GL_HELPER_H
#define GL_HELPER_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <IL/il.h>

#include <iostream>
#include <map>
#include <vector>

#include "irrKlang.h"
using namespace irrklang;

#include "camera.h"
#include "shader.h"
#include "object.h"
#include "sky_box.h"
#include "terrain.h"
#include "transform.h"

#include "sun.h"
#include "house.h"
#include "windmill.h"
#include "light.h"
#include "drawable.h"
#include "snow.h"
#include "horse.h"
#include "tree.h"

#include "bricks.h"
#include "barrel.h"

#include "dog.h"

#define WIN_TITLE "VR OpenGL Project"
#define WIN_WIDTH 1024
#define WIN_HEIGHT 720

namespace glHelper
{
    GLFWwindow *initGlfwWindow();
    void printWelcomeMessage();
    void printContextInfo();
    void initCallbacks(GLFWwindow *window);
    void init(GLFWwindow *window);
    void mainLoop(GLFWwindow *window);
    void error_callback(int error, const char *description);
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void fillTreesPos();
    void cleanup(GLFWwindow *window);
} // namespace glHelper

#endif