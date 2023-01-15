#include "gl_helper.h"

Camera camera;
bool isSunMoving = true;
GLfloat cameraPosition[3];
GLfloat cameraDirection[3];
static bool firstLeftMouseButton = true, leftMouseButtonPress = false;
static double prevMouseXPress = WIN_WIDTH / 2.0f, prevMouseYPress = WIN_HEIGHT / 2.0f;
static double prevScrollYOffset = 0;
ISoundEngine *soundEngine = createIrrKlangDevice();
ISound *music;
bool isSnowing = true;
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
    glfwSetKeyCallback(window, key_callback);
    // Framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    float ambient_strength = 0.5;
    float diffuse_strength = 0.5;
    float specular_strength = 0.5;
    glm::vec3 materialColour = glm::vec3(1.0f, 1.0f, 0.9f);
    glm::vec3 light_pos = glm::vec3(5.0, 20.0, 2.0);

    Light light(shininess, ambient_strength, diffuse_strength, specular_strength, materialColour);

    Terrain terrain;
    glm::mat4 terrainModel = glm::mat4(1.0);
    terrainModel = glm::scale(terrainModel, glm::vec3(384.0, 32.0, 384.0));

    light.setLight(terrain.getShader());

    Windmill windmill;
    light.setLight(windmill.getShader());

    House house;
    light.setLight(house.getShader());

    SkyBox skyboxCubemap;

    Horse horse(skyboxCubemap.textureID);

    Sun sun;

    GLfloat light_position[3];
    light_position[0] = light_pos.x;
    light_position[1] = light_pos.y;
    light_position[2] = light_pos.z;

    camera.lookAt(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    camera.init(5.5f, &terrain);
    camera.setSpeed(5.0f);
    camera.updatePos();

    glm::mat4 view = camera.getMatrix();
    glm::mat4 perspective = glm::perspective(1.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.01f, 1000.0f);

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
        }
        return deltaTime;
    };

    unsigned int nbOfParticles = 20000;
    SnowManager snow_particles_manager(nbOfParticles);
    Bricks bricks(3, 9);
    bricks.transform(glm::vec3(-0.91, 2.0, -26.46), glm::radians(115.f), glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.5, 0.5, 0.5));

    Bricks bricks2(3, 9);
    bricks2.transform(glm::vec3(3.6, 2.0, -36.1), glm::radians(25.f), glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.55, 0.5, 0.5));

    Barrel barrel;

    snow_particles_manager.set_emiter_boundary(-384, 384, 29, 31, -384, 384);
    snow_particles_manager.set_life_duration_sec(2, 5);
    snow_particles_manager.set_initial_velocity(0, -30.0f / 5.0f, 0, 0, 1.0f, 0); // 30/5 unit per second, with +- 1.0

    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        view = camera.getMatrix();
        camera.getPosition(cameraPosition);
        camera.getDirection(cameraDirection);
        double currentTime = glfwGetTime();
        static float prevTime = 0;
        snow_particles_manager.set_time(currentTime);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 delta = light_pos;

        if (isSunMoving)
        {
            delta = glm::vec3(5.0, 5.0, -30.0) + glm::vec3(0.0f, cos(currentTime / 2) * 300.0f, sin(currentTime / 2) * 300.0f);
        }

        for (size_t i = 0; i < treesPos.size(); i++)
        {
            Transform tree_transf;
            tree_transf.translate(treesPos[i].first, 0.5, treesPos[i].second);
            tree_transf.scale(3.0f, 3.0f, 3.0f);
            trees[i]->setMVP(tree_transf.get_matrix(), view, perspective);
            trees[i]->setLightPos(glm::value_ptr(light_pos));
            trees[i]->move_leaves(currentTime - prevTime);
            trees[i]->draw();
        }

        house.draw(view, perspective, glm::make_vec3(cameraPosition), delta);
        const glm::vec3 sun_colour = glm::vec3(1.0f, 1.0f, 0.0f);
        sun.draw(view, perspective, glm::make_vec3(cameraPosition), light_pos, delta, sun_colour);

        horse.draw(view, perspective, glm::make_vec3(cameraPosition), light_pos, glm::vec3(2.0f, 2.0f, 1.0f));

        if (isSnowing)
        {
            snow_particles_manager.draw(view, perspective, cameraPosition, light_position);
        }

        // 3D  audio positioning
        vec3df audioPos(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
        vec3df audioDir(-1.0 * cameraDirection[0], -1.0 * cameraDirection[1], -1.0 * cameraDirection[2]);
        soundEngine->setListenerPosition(audioPos, audioDir, vec3df(0.0, 1.0, 0.0));

        // draw the terrain
        terrain.draw(terrainModel, camera.getMatrix(), perspective, delta, glm::make_vec3(cameraPosition));

        barrel.draw(view, perspective, glm::make_vec3(cameraPosition), light_pos);

        double deltaTime = fps(currentTime);
        float degree = deltaTime * 100 > 25 ? 14.0 : 8.0;
        windmill.draw(view, perspective, glm::make_vec3(cameraPosition), delta, degree);

        bricks.draw(view, perspective, glm::make_vec3(cameraPosition), light_pos);

        bricks2.draw(view, perspective, glm::make_vec3(cameraPosition), light_pos);

        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxCubemap.draw(view, perspective, glm::make_vec3(cameraPosition), light_pos);
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