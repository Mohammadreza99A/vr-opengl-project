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
float heightScale = 0.1f;

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
    std::cout << "\t - for rotation you can also use the mouse by hold left mouse key and moving it" << std::endl;
    std::cout << "\t - scroll wheel (vertical) of the mouse can be used to move forward and backward" << std::endl;
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
    // Mouse actions callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    // Framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Ensure we can capture the keyboard keys and mouse buttons
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
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
    light.setLight(horse.getShader());
    
    glm::vec3 horseColor= horse.getColor();
    glm::vec3 horsePos=horse.get_position();

    PointLight point_light(horseColor, 0.05, 1, 1,horsePos,1.0,0.09,0.032);

    point_light.setLight(horse.getShader());
    point_light.setLight(house.getShader());
    point_light.setLight(windmill.getShader());
    point_light.setLight(terrain.getShader());

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
            std::cout << "\r FPS: " << fpsCount << std::endl;
        }
        return deltaTime;
    };

    unsigned int nbOfParticles = 20000;
    SnowManager snow_particles_manager(nbOfParticles);
    Bricks bricks(9, 37);
    bricks.transform(glm::vec3(2.2, 2.0, -27.9), glm::radians(295.f), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.45, 0.5, 0.5));

    Bricks bricks2(9, 37);
    bricks2.transform(glm::vec3(4.9, 2.0, -33.1), glm::radians(205.f), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.55, 0.5, 0.5));

    Barrel barrel;

    snow_particles_manager.set_emiter_boundary(-384, 384, 29, 31, -384, 384);
    snow_particles_manager.set_life_duration_sec(2, 5);
    snow_particles_manager.set_initial_velocity(0, -30.0f / 5.0f, 0, 0, 1.0f, 0); // 30/5 unit per second, with +- 1.0

    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window))
    {
        view = camera.getMatrix();
        camera.getPosition(cameraPosition);
        camera.getDirection(cameraDirection);
        glfwPollEvents();
        double currentTime = glfwGetTime();
        snow_particles_manager.set_time(currentTime);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 delta = light_pos;

        if (isSunMoving)
        {
            delta = glm::vec3(5.0, 5.0, -30.0) + glm::vec3(0.0f, cos(currentTime / 2) * 300.0f, sin(currentTime / 2) * 300.0f);
        }
        horse.draw(view, perspective, glm::make_vec3(cameraPosition), light_pos, glm::vec3(2.0f, 2.0f, 1.0f));

        horseColor = horse.getColor();

        point_light.setColor(house.getShader(),horseColor);
        house.draw(view, perspective, glm::make_vec3(cameraPosition), delta);
        const glm::vec3 sun_colour = glm::vec3(1.0f, 1.0f, 0.0f);
        sun.draw(view, perspective, glm::make_vec3(cameraPosition), light_pos, delta, sun_colour);

          

        if (isSnowing)
        {
            snow_particles_manager.draw(view, perspective, cameraPosition, light_position);
        }

        // 3D  audio positioning
        vec3df audioPos(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
        vec3df audioDir(-1.0 * cameraDirection[0], -1.0 * cameraDirection[1], -1.0 * cameraDirection[2]);
        soundEngine->setListenerPosition(audioPos, audioDir, vec3df(0.0, 1.0, 0.0));

        // draw the terrain
        point_light.setColor(terrain.getShader(),horseColor);
        terrain.draw(terrainModel, camera.getMatrix(), perspective, delta, glm::make_vec3(cameraPosition));

        point_light.setColor(house.getShader(),horseColor);
        barrel.draw(view, perspective, glm::make_vec3(cameraPosition), delta);

        double deltaTime = fps(currentTime);
        float degree = deltaTime * 100 > 25 ? 14.0 : 8.0;
        point_light.setColor(windmill.getShader(),horseColor);
        windmill.draw(view, perspective, glm::make_vec3(cameraPosition), delta, degree);

        bricks.draw(view, perspective, glm::make_vec3(cameraPosition),heightScale, delta);

        bricks2.draw(view, perspective, glm::make_vec3(cameraPosition),heightScale, delta);

        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxCubemap.draw(view, perspective, glm::make_vec3(cameraPosition), delta);
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
        camera.inputHandling('W', 1.0);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.inputHandling('S', 1.0);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.inputHandling('A', 1.0);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.inputHandling('D', 1.0);

    // Rotation with IJKL keys
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        camera.inputHandling('L', 0.15);
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        camera.inputHandling('J', 0.15);
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        camera.inputHandling('I', 0.15);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        camera.inputHandling('K', 0.15);

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

void glHelper::mouse_button_callback(GLFWwindow *window, int button,
                                     int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        if (!leftMouseButtonPress)
        {
            leftMouseButtonPress = true;
            firstLeftMouseButton = true;
        }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        if (leftMouseButtonPress)
        {
            leftMouseButtonPress = false;
            firstLeftMouseButton = true;
        }
}

void glHelper::mouse_cursor_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    if (!leftMouseButtonPress)
        return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstLeftMouseButton)
    {
        prevMouseXPress = xpos;
        prevMouseYPress = ypos;
        firstLeftMouseButton = false;
    }

    float xoffset = xpos - prevMouseXPress;
    float yoffset = prevMouseYPress - ypos; // reversed since y-coordinates go from bottom to top

    prevMouseXPress = xpos;
    prevMouseYPress = ypos;

    // camera.processMouseMovement(xoffset, yoffset);
}

void glHelper::mouse_scroll_callback(GLFWwindow *window,
                                     double xoffset, double yoffset)
{
    prevScrollYOffset += yoffset;

    if (prevScrollYOffset + yoffset > prevScrollYOffset)
        camera.inputHandling('W', 0.5);
    if (prevScrollYOffset + yoffset < prevScrollYOffset)
        camera.inputHandling('S', 0.5);
}

void glHelper::cleanup(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}