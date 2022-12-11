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

void glHelper::printWelcomeMessage()
{
    std::cout << "Commands for camera actions:" << std::endl;
    std::cout << "\t - for movement, use arrow keys" << std::endl;
    std::cout << "\t - for rotation, use IJKL keys" << std::endl;
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

    Shader shader("shaders/vertexShader.glsl", "shaders/fragShader.glsl");

    char path[] = PATH_TO_OBJECTS "/house.obj";

    Object house(path);
    house.makeObject(shader);
    house.model = glm::translate(house.model, glm::vec3(0.0, 0.0, -30.0));
    house.model = glm::rotate(house.model, glm::radians(25.f), glm::vec3(0.0, 1.0, 0.0));
    house.model = glm::scale(house.model, glm::vec3(0.4, 0.4, 0.4));

    const glm::vec3 light_pos = glm::vec3(1.0, 2.0, 2.0);

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

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 perspective = camera.GetProjectionMatrix();

    std::string pathToHosuTex = PATH_TO_TEXTURE "/house/house_texture.jpg";
    Texture textureHouse(pathToHosuTex);

    // Loading cube map for skybox
    std::vector<std::string> faces;
    faces.push_back(PATH_TO_TEXTURE "/skybox/posx.jpg");
    faces.push_back(PATH_TO_TEXTURE "/skybox/negx.jpg");
    faces.push_back(PATH_TO_TEXTURE "/skybox/posy.jpg");
    faces.push_back(PATH_TO_TEXTURE "/skybox/negy.jpg");
    faces.push_back(PATH_TO_TEXTURE "/skybox/posz.jpg");
    faces.push_back(PATH_TO_TEXTURE "/skybox/negz.jpg");
    SkyBox skyboxCubemap(faces);
    skyboxCubemap.load();

    Shader skyboxShader("shaders/skyBoxV.glsl", "shaders/skyBoxF.glsl");

    skyboxShader.use();
    skyboxShader.setInteger("skybox", 0);

    glfwSwapInterval(1);

    // Main loop until escape key is pressed
    while (!glfwWindowShouldClose(window))
    {
        view = camera.GetViewMatrix();
        glfwPollEvents();
        double currentTime = glfwGetTime();

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind Texture
        textureHouse.bind();

        shader.use();
        // 1. send the relevant uniform to the shader
        shader.setMatrix4("M", house.model);
        shader.setMatrix4("itM", glm::transpose(glm::inverse(house.model)));
        shader.setMatrix4("V", view);
        shader.setMatrix4("P", perspective);
        shader.setVector3f("u_view_pos", camera.Position);
        shader.setVector3f("u_light_pos", light_pos);

        house.draw();

        // draw skybox as last
        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMatrix4("V", view);
        skyboxShader.setMatrix4("P", perspective);
        skyboxCubemap.draw();
        glDepthFunc(GL_LESS); // set depth function back to default

        fps(currentTime);
        glfwSwapBuffers(window);
    }
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
    // Movement with Arrow keys
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(FORWARD, 0.5);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(BACKWARD, 0.5);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(LEFT, 0.5);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(RIGHT, 0.5);

    // Rotation with IJKL keys
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(1, 0.0, 1);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(-1, 0.0, 1);
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(0.0, 1.0, 1);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(0.0, -1.0, 1);
}

void glHelper::cleanup(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}