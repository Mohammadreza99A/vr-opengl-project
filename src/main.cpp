#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gl_helper.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "GLAD Init Failed : %s" << std::endl;
        return EXIT_FAILURE;
    }

    glHelper::printContextInfo();

    glHelper::printWelcomeMessage();

    glHelper::initCallbacks(window);

    glHelper::init(window);

    glHelper::mainLoop(window);

    glHelper::cleanup(window);

    return EXIT_SUCCESS;
}
