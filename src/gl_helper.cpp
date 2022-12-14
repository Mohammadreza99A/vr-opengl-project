#include "gl_helper.h"

Camera camera(glm::vec3(0.0, 0.0, 0.1));

GLFWwindow *glHelper::initGlfwWindow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
}

void glHelper::printContextInfo()
{
    std::cout << "Vendor: \t" << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: \t" << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: \t" << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: \t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void glHelper::initCallbacks(GLFWwindow *window)
{
    // Keyboard Callback
    glfwSetKeyCallback(window, key_callback);
    // Framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void glHelper::init(GLFWwindow *window)
{
    ilInit();

    glClearColor(0.3f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_MULTISAMPLE);

    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
}

void glHelper::mainLoop(GLFWwindow *window)
{
    // Sun
    Shader lightingShader("shaders/lightVertexShader.glsl", "shaders/lightFragShader.glsl");
    Shader sunShader("shaders/sunVertexShader.glsl", "shaders/sunFragShader.glsl");

    char pathSun[] = PATH_TO_OBJECTS "/sun.obj";

    Object sun(pathSun);
    sun.makeObject(sunShader, false);

    // House
    char pathHouse[] = PATH_TO_OBJECTS "/house.obj";

    Object house(pathHouse);
    house.makeObject(lightingShader);
    house.model = glm::translate(house.model, glm::vec3(5.3, 0.0, -30.0));
    house.model = glm::rotate(house.model, glm::radians(25.f), glm::vec3(0.0, 1.0, 0.0));
    house.model = glm::scale(house.model, glm::vec3(0.4, 0.4, 0.4));

    double prev = 0;
    int deltaFrame = 0;
    // fps function
    auto fps = [&](double now)
    {
        double deltaTime = now - prev;
        deltaFrame++;
        if (deltaTime > 0.5)
        {
            prev = now;
            const double fpsCount = (double)deltaFrame / deltaTime;
            deltaFrame = 0;
            std::cout << "\r FPS: " << fpsCount << std::endl;
        }
    };

    glm::vec3 light_pos = glm::vec3(1.0f, 2.0f, 2.0f);

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 perspective = glm::perspective(1.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.01f, 1000.0f);

    std::string pathToSunTex = PATH_TO_TEXTURE "/sun/sun_texture.png";
    Texture textureSun(pathToSunTex);
    std::string pathToHosuTex = PATH_TO_TEXTURE "/house/house_texture.jpg";
    Texture textureHouse(pathToHosuTex);

    float ambient = 0.5;
    float diffuse = 1.0;
    float specular = 0.8;

    glm::vec3 materialColour = glm::vec3(1.0f, 1.0f, 0.9f);

    // Rendering

    lightingShader.use();
    lightingShader.setFloat("shininess", 32.0f);
    lightingShader.setVector3f("materialColour", materialColour);
    lightingShader.setFloat("light.ambient_strength", ambient);
    lightingShader.setFloat("light.diffuse_strength", diffuse);
    lightingShader.setFloat("light.specular_strength", specular);
    // lightingShader.setFloat("light.constant", 1.0);
    // lightingShader.setFloat("light.linear", 0.14);
    // lightingShader.setFloat("light.quadratic", 0.07);

    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window))
    {
        view = camera.GetViewMatrix();
        glfwPollEvents();
        double currentTime = glfwGetTime();
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind Texture
        textureHouse.bind();
        lightingShader.use();

        lightingShader.setMatrix4("M", house.model);
        lightingShader.setMatrix4("itM", glm::transpose(glm::inverse(house.model)));
        lightingShader.setMatrix4("V", view);
        lightingShader.setMatrix4("P", perspective);
        lightingShader.setVector3f("u_view_pos", camera.Position);

        auto delta = glm::vec3(5.3f, sin(currentTime) * 40.0f, -40 + cos(currentTime) * 40.0f);

        // auto delta =  light_pos ;
        // std::cout << delta.z <<std::endl;
        lightingShader.setVector3f("light.light_pos", delta);

        house.draw();

        textureSun.bind();
        glm::vec3 sunColour = glm::vec3(1.0f, 1.0f, 0.0f);

        sun.model = glm::mat4(1.0f);
        sun.model = glm::translate(sun.model, delta);
        sun.model = glm::scale(sun.model, glm::vec3(2.0f, 2.0f, 1.0f));
        sunShader.use();
        sunShader.setMatrix4("M", sun.model);
        sunShader.setMatrix4("itM", glm::transpose(glm::inverse(sun.model)));
        sunShader.setMatrix4("V", view);
        sunShader.setMatrix4("P", perspective);
        sunShader.setVector3f("u_view_pos", camera.Position);
        sunShader.setVector3f("sunColour", sunColour);

        sun.draw();

        glDepthFunc(GL_LESS);
        fps(currentTime);
        glfwSwapBuffers(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
}

void glHelper::error_callback(int error, const char *description)
{
    std::cerr << description << std::endl;
}

void glHelper::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void glHelper::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Close window if ESCAPE key is pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        std::cout << "Space key was pressed" << std::endl;

    // Camera input handling
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(UP, 1);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(DOWN, 1);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(LEFT, 1);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(RIGHT, 1);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(FORWARD, 1);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(BACKWARD, 1);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(-1, 0.0, 1);
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(1, 0.0, 1);
}

void glHelper::cleanup(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}