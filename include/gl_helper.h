#ifndef GL_HELPER_H
#define GL_HELPER_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <IL/il.h>

#include <iostream>

#include <map>
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "texture.h"

#include "sun.h"
#define WIN_TITLE "VR OpenGL Project"
#define WIN_WIDTH 1024
#define WIN_HEIGHT 720

namespace glHelper
{
    GLFWwindow *initGlfwWindow();
    void printContextInfo();
    void initCallbacks(GLFWwindow *window);
    void init(GLFWwindow *window);
    void mainLoop(GLFWwindow *window);
    void error_callback(int error, const char *description);
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void cleanup(GLFWwindow *window);
} // namespace glHelper

#endif