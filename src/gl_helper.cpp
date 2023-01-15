#include "gl_helper.h"

Camera camera;
bool isSunMoving = true;
glm::vec3 cameraPosition;
glm::vec3 cameraDirection;
static bool firstLeftMouseButton = true, leftMouseButtonPress = false;
static double prevMouseXPress = WIN_WIDTH / 2.0f, prevMouseYPress = WIN_HEIGHT / 2.0f;
static double prevScrollYOffset = 0;
ISoundEngine *soundEngine = createIrrKlangDevice();
ISound *music;
bool isSnowing = true;
float heightScale = 0.1f;
std::vector<Tree *> trees;
std::vector<std::pair<double, double>> treesPos;

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
    std::cout << "To play 3D audio, press Espace key. Having the right drivers matter" << std::endl;
    std::cout << "Commands for snow particles:" << std::endl;
    std::cout << "\t - to activate snow particles, use W key" << std::endl;
    std::cout << "\t - to deactivate snow particles, use S key" << std::endl;
    std::cout << "Commands for sun movement:" << std::endl;
    std::cout << "\t - to activate sun movement, use Y key" << std::endl;
    std::cout << "\t - to deactivate sun movement, use N key" << std::endl;
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
    glfwSetKeyCallback(window, keyCallback);
    // Framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // Ensure we can capture the keyboard keys and mouse buttons
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
}

void glHelper::init(GLFWwindow *window)
{
    srand(time(0));

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

    fillTreesPos();

    for (size_t i = 0; i < treesPos.size(); i++)
    {
        Tree *tree = new Tree;
        tree->init();
        tree->load();
        trees.push_back(tree);
    }
}

void glHelper::mainLoop(GLFWwindow *window)
{
    float shininess = 32.0f;
    float ambientStrength = 0.5;
    float diffuseStrength = 0.5;
    float specular_strength = 0.5;
    glm::vec3 materialColour = glm::vec3(1.0f, 1.0f, 0.9f);
    glm::vec3 lightPosition = glm::vec3(5.0, 100.0, 2.0);

    Light light(shininess, ambientStrength, diffuseStrength, specular_strength, materialColour);

    Terrain terrain;
    light.on(terrain.getShader());
    terrain.init(1280, 1280);
    glm::mat4 terrainModel = glm::mat4(1.0);
    terrainModel = glm::scale(terrainModel, glm::vec3(384.0, 32.0, 384.0));

    Windmill windmill;
    light.on(windmill.getShader());

    House house;
    light.on(house.getShader());

    SkyBox skyboxCubemap;

    Horse horse(skyboxCubemap.textureID);
    std::vector<Shader *> horseShaders = horse.getShader();
    for (int i = 0; i < (int)horseShaders.size(); i++)
    {
        light.on(horseShaders[i]);
    }

    Sun sun;

    // Snow
    unsigned int nbOfParticles = 20000;
    Snow snowParticles(nbOfParticles);

    glm::vec3 minCoords = glm::vec3(-384, 29, -384);
    glm::vec3 maxCoords = glm::vec3(384, 31, 384);
    snowParticles.setEmiterBoundary(minCoords, maxCoords);

    snowParticles.setLifeDurationSec(2, 5);

    glm::vec3 initialVelocity = glm::vec3(0, -30.0f / 5.0f, 0);
    glm::vec3 variation = glm::vec3(0, 1.0f, 0);
    snowParticles.setInitialVelocity(initialVelocity, variation);
    
    // Bricks
    Bricks bricks(9, 37);
    light.on(bricks.getShader());
    bricks.transform(glm::vec3(2.2, 2.0, -27.9), glm::radians(295.f), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.45, 0.5, 0.5));

    Bricks bricks2(9, 37);
    light.on(bricks2.getShader());
    bricks2.transform(glm::vec3(4.9, 2.0, -33.1), glm::radians(205.f), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.55, 0.5, 0.5));

    // Barrel
    Barrel barrel;
    light.on(barrel.getShader());

    // Dog
    Dog dog;
    light.on(dog.getShader());

    // Camera
    camera.lookAt(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    camera.init(5.5f, &terrain);
    camera.setSpeed(5.0f);
    camera.updatePos();

    glm::mat4 view = camera.getMatrix();
    glm::mat4 perspective = glm::perspective(1.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.01f, 1000.0f);

    // Frame per second function
    double prev = 0;
    int deltaFrame = 0;
    auto fps = [&](double now)
    {
        double deltaTime = now - prev;
        deltaFrame++;
        if (deltaTime > 0.5)
        {
            prev = now;
            const double fpsCount = (double)deltaFrame / deltaTime;
            deltaFrame = 0;
        }
        return deltaTime;
    };

    // Main loop
    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        view = camera.getMatrix();
        camera.getPosition(cameraPosition);
        camera.getDirection(cameraDirection);
        double currentTime = glfwGetTime();

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Light & Sun
        glm::vec3 lightMovement = lightPosition;
        const glm::vec3 sunColor = glm::vec3(1.0f, 1.0f, 0.0f);

        if (isSunMoving)
        {
            lightMovement = lightPosition + glm::vec3(0.0f, cos(currentTime / 2) * 300.0f, sin(currentTime / 2) * 300.0f);
        }
        sun.draw(view, perspective, cameraPosition, lightPosition, lightMovement, sunColor);

        // Trees
        static float prevTime = 0;
        for (size_t i = 0; i < treesPos.size(); i++)
        {
            Transform tree_transf;
            tree_transf.translate(treesPos[i].first, 0.5, treesPos[i].second);
            tree_transf.scale(3.0f, 3.0f, 3.0f);
            trees[i]->setMVP(tree_transf.get_matrix(), view, perspective);
            trees[i]->setLightPos(glm::value_ptr(lightPosition));
            trees[i]->move_leaves(currentTime - prevTime);
            trees[i]->draw();
        }

        // House
        house.draw(view, perspective, cameraPosition, lightMovement);

        // Dog
        dog.draw(view, perspective, cameraPosition, lightMovement, currentTime);

        // Horse
        glm::vec3 lightColor = glm::vec3(2.0f, 2.0f, 1.0f);
        horse.draw(view, perspective, cameraPosition, lightMovement, lightColor);

        // Snow
        snowParticles.setTime(currentTime);
        if (isSnowing)
        {
            snowParticles.draw(view, perspective, cameraPosition, lightPosition);
        }

        // 3D  audio positioning
        vec3df audioPos(cameraPosition.x, cameraPosition.y, cameraPosition.z);
        vec3df audioDir(-1.0 * cameraDirection.x, -1.0 * cameraDirection.y, -1.0 * cameraDirection.z);
        soundEngine->setListenerPosition(audioPos, audioDir, vec3df(0.0, 1.0, 0.0));

        // Terrain
        terrain.draw(terrainModel, camera.getMatrix(), perspective, lightMovement, cameraPosition);

        // Barrel
        barrel.draw(view, perspective, cameraPosition, lightMovement);

        // Windmill
        double deltaTime = fps(currentTime);
        float degree = deltaTime * 100 > 25 ? 14.0 : 8.0;
        windmill.draw(view, perspective, cameraPosition, lightMovement, degree);

        // Bricks
        bricks.draw(view, perspective, cameraPosition, heightScale, lightMovement);
        bricks2.draw(view, perspective, cameraPosition, heightScale, lightMovement);

        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content

        // Skybox
        skyboxCubemap.draw(view, perspective, cameraPosition, lightPosition);
        glDepthFunc(GL_LESS); // set depth function back to default

        fps(currentTime);
        prevTime = currentTime;
        glfwSwapBuffers(window);
    }
}

void glHelper::fillTreesPos()
{
    treesPos.push_back(std::pair<double, double>(30.0, -20.0));
    treesPos.push_back(std::pair<double, double>(25.0, -20.0));
    treesPos.push_back(std::pair<double, double>(-50.0, 15.0));
    treesPos.push_back(std::pair<double, double>(-15.0, 10.0));
    treesPos.push_back(std::pair<double, double>(10.0, -80.0));
    treesPos.push_back(std::pair<double, double>(-5.0, -5.0));
    treesPos.push_back(std::pair<double, double>(10.0, -70.0));
    treesPos.push_back(std::pair<double, double>(-15.0, -5.0));
    treesPos.push_back(std::pair<double, double>(35.0, -35.0));
    treesPos.push_back(std::pair<double, double>(25.0, -50.0));
}

void glHelper::errorCallback(int error, const char *description)
{
    std::cerr << description << std::endl;
}

void glHelper::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void glHelper::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Close window if ESCAPE key is pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        music = soundEngine->play3D("resources/audio/ambiant_music.wav",
                                    vec3df(5, 5, -35), true, false, true);
        if (music)
        {
            music->setMinDistance(3.0f);
        }
    }

    // Camera input handling
    // Movement with Arrow keys
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.inputHandling('W', 0.5);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.inputHandling('S', 0.5);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.inputHandling('A', 0.5);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.inputHandling('D', 0.5);

    // Rotation with IJKL keys
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        camera.inputHandling('L', 0.1);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        camera.inputHandling('J', 0.1);
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        camera.inputHandling('I', 0.1);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        camera.inputHandling('K', 0.1);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (heightScale > 0.0f)
            heightScale -= 0.0005f;
        else
            heightScale = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (heightScale < 1.0f)
            heightScale += 0.0005f;
        else
            heightScale = 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (heightScale > 0.0f)
            heightScale -= 0.0005f;
        else
            heightScale = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (heightScale < 1.0f)
            heightScale += 0.0005f;
        else
            heightScale = 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        isSnowing = true;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        isSnowing = false;

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        isSunMoving = true;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        isSunMoving = false;
}

void glHelper::cleanup(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}