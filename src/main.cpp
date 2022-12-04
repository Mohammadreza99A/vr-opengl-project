#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "gl_helper.h"

int main()
{

    glfwSetErrorCallback(glHelper::error_callback);

    if (!glfwInit())
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    GLFWwindow *window = glHelper::initGlfwWindow();
    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glHelper::printContextInfo();

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_NO_ERROR)
    {
        std::cerr << "GLEW Init Failed : %s" << std::endl;
        return EXIT_FAILURE;
    }

    glHelper::initCallbacks(window);

    glHelper::init(window);

    glHelper::mainLoop(window);

    glHelper::cleanup(window);

    return EXIT_SUCCESS;
}
