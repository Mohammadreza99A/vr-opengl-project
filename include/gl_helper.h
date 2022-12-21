#ifndef GL_HELPER_H
#define GL_HELPER_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <IL/il.h>

#include <iostream>
#include <map>
#include <vector>

#include "camera.h"
#include "shader.h"
#include "object.h"
#include "sky_box.h"
#include "terrain.h"

#include "house.h"
#include "windmill.h"
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
    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
    void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos);
    void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    void cleanup(GLFWwindow *window);
    void wind_func(float pos[3], float ret[3], float time);
} // namespace glHelper

#endif