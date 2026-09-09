#include <cmath>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "stb/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// screen settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// uniform values
float mixValue = 0.2f;

// Camera init
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// mouse callback settings
float lastMouseX = SCR_WIDTH / 2;
float lastMouseY = SCR_HEIGHT / 2;
bool firstMouse = true;

// game loop settings
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// model rotations
float incAngle = 20.0f;
float angle = 20.0f;

// light settings
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// optimization keybind
bool queryInstancedPerformance = true;

// ---- LEAF -----
// animation initiation
bool leafShouldUp = false;
float leafHeight = 0.0f; // keep track current height position
bool leafTargetUp = false; // determine whether the leaf should go up or down
bool fKeyHeld = false; // used to limit key presses to a single press instead of per frame, which can be many

// leaf rotation
float leafSpingAngle = 10.0f;
float leafSpingAngleUp = leafSpingAngle + 100.0f;
float leafSpingAngleDown = leafSpingAngle + 100.0f * deltaTime;


int main() {
    // glfw: initialize and configure
    glfwInit(); // CPU
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // CPU
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // CPU
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // CPU

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL); // CPU
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // CPU
    glfwSwapInterval(1);
    // enables vsync, syncs to monitor refresh. Not sure this actually works the way I had hoped it too. Also, feels redundant now with deltaTime in place.


    // CPU asks drivers for OS-specific function pointers
    if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress)) // CPU, prepping for GPU integration
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Callback registrations
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // These initializations heavily reduce code clutter in main file.
    Shader lightSourceShader("shaders/lightSource.vert", "shaders/lightSource.frag");
    Shader cubeShader("shaders/lightObject.vert", "shaders/lightObject.frag");
    Shader leafShader("shaders/leafSway.vert", "shaders/lightObject.frag");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(7.0f, 0.0f, -8.0f),
        glm::vec3(9.0f, 5.0f, -17.0f),
        glm::vec3(6.5f, -2.2f, -10.5f),
        glm::vec3(4.8f, -2.0f, -14.3f),
        glm::vec3(9.4f, -0.4f, -11.5f),
        glm::vec3(6.7f, 3.0f, -15.5f),
        glm::vec3(9.3f, -2.0f, -10.5f),
        glm::vec3(9.5f, 2.0f, -10.5f),
        glm::vec3(9.5f, 0.2f, -9.5f),
        glm::vec3(9.3f, 1.0f, -9.5f)
    };

    std::vector<glm::vec3> leafPositions = {
        glm::vec3(-0.42f, -1.45f, 0.36f),
        glm::vec3(-1.03f, -1.06f, -0.32f),
        glm::vec3(-1.06f, -1.09f, -1.11f),
        glm::vec3(-0.16f, -1.53f, -0.98f),
        glm::vec3(-0.18f, -0.77f, -0.90f),
        glm::vec3(-0.66f, -0.97f, 1.07f),
        glm::vec3(0.19f, -1.20f, 1.14f),
        glm::vec3(-1.09f, -0.74f, -0.50f),
        glm::vec3(-0.85f, -1.48f, -0.46f),
        glm::vec3(0.76f, -1.42f, 0.20f),
        glm::vec3(0.33f, -1.23f, 0.11f),
        glm::vec3(-1.05f, -1.54f, -0.71f),
        glm::vec3(0.43f, -1.17f, -0.45f),
        glm::vec3(0.21f, -1.15f, -0.48f),
        glm::vec3(0.71f, -0.90f, -0.61f),
        glm::vec3(0.18f, -1.07f, 0.90f),
        glm::vec3(0.55f, -1.31f, 1.15f),
        glm::vec3(-0.92f, -1.18f, 0.62f),
        glm::vec3(-0.84f, -1.11f, -1.11f),
        glm::vec3(0.40f, -0.84f, 0.18f),
        glm::vec3(0.90f, -1.29f, 0.47f),
        glm::vec3(0.23f, -1.02f, -0.11f),
        glm::vec3(0.82f, -0.66f, -0.06f),
        glm::vec3(0.39f, -1.54f, 0.48f),
        glm::vec3(0.35f, -0.61f, 0.77f),
    };


    unsigned int texture1, texture2;

    // texture 1 setup
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // texture 1 configs
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate texture 1
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("res/textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 2 setup
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // texture 2 configs
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate texture 2
    data = stbi_load("res/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    unsigned int VBOs[2], VAOs[2]; // CPU-isolated allocation.
    glGenVertexArrays(2, VAOs);
    // Generates unique integer ID's on the GPU. The call itself to store these values are issued via CPU. Will be used to store configurations detailing how memory on GPU is supposed to be read. (see AttribPointers below.)
    glGenBuffers(2, VBOs);
    // Generates unique integer ID's for buffer objects on GPU. Call is issued from CPU. The buffer will later the allocated memory in GPU.

    // light source object
    glBindVertexArray(VAOs[0]);
    // Sets the current active vertex array. Writing data/configurations will be related to this specific VertexArray. VAO stores the vertex buffer layout, which is a way to determine which set of bytes correspond to which "graphical element", such as vertex positions, colors, textures, textureID's, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); // Sets the current active buffer object.
    // vertex positions
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Writes to (allocates) memory on the GPU holding the specified data.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    // Specifies the memory layout of the buffer object, telling the GPU how the sequential data should be interpreted.
    glEnableVertexAttribArray(0);


    // light hit object
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // vertex surface normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // vertex textures
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int diffuseMap;
    // texture setup
    glGenTextures(1, &diffuseMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    // texture 3 configs
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate texture 3
    data = stbi_load("res/textures/container2.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int specularMap;
    // texture 3 setup
    glGenTextures(1, &specularMap);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    // texture 3 configs
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("res/textures/container2_specular.png", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 4 setup
    unsigned int emissionMap;
    // texture 4 setup
    glGenTextures(1, &emissionMap);
    glBindTexture(GL_TEXTURE_2D, emissionMap);

    // texture 4 configs
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate texture 4
    data = stbi_load("res/textures/matrix.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    /* lookAt matrix example */
    /*  R = camera right vector
     *  U = camera up vector
     *  D = camera direction vector
     *  P = camera's position
     *  __           __         --          --
     *  |Rx, Ry, Rz, 0]         |1, 0, 0, -Px|
     *  |Ux, Uy, Uz, 0|     *   |0, 1, 0, -Py|
     *  |Dx, Dy, Dz, 0|         |0, 0, 1, -Pz|
     *  |0,  0,  0,  0|         |0, 0, 0,  1 |
     *  --          --          --          --
     *  The result of the above matrix is equivalent to using: glm::lookAt(cameraPos, targetPos, worldUp)
     */

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.2f),
        glm::vec3(1.0f, 1.0, 0.0),
        glm::vec3(0.2f, 0.2f, 1.0f),
    };


    srand(time(0));
    // the leaf model scaled to 1.0 is **huge**! thus, the desired scaling right now is quite drastic (somewhere between 0.1 and 0.01)
    std::vector<float> leafScales;
    for (size_t i = 0; i < leafPositions.size(); i++) {
        int randomNr = (rand() % 6) + 1;
        float modelScale = 0.1 / randomNr;
        leafScales.push_back(modelScale);
    }


    // This basically encapsulates vertex setup (VAO, VBO, EBO), materials and texture unit application for model objects.
    Model modelObj("res/textures/dry-leaf-rawscan/foglia.obj");
    modelObj.setupInstancing(leafPositions.size());

    cubeShader.use();
    cubeShader.setInt("u_Material.texture_diffuse1", 0);
    cubeShader.setInt("u_Material.specular", 1);


    GLuint queryID[2];
    glGenQueries(2, queryID);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // clear previous frame and set window background
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeShader.use();
        cubeShader.setVec3("u_ViewPos", camera.Position);

        cubeShader.setFloat("u_Material.shininess", 32.0f); // determines light spread

        /* Disco lamp color for Dir Light. Unused atm because i dont want a global disco ball. Saving in case I want to reuse.
        glm::vec3 lightColor;
        lightColor.x = (float) sin(glfwGetTime() * 2.0f);
        lightColor.y = (float) sin(glfwGetTime() * 0.7f);
        lightColor.z = (float) sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
*/

        // Directional light
        cubeShader.setDirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.5f, 0.5f, 0.3f),
                                       glm::vec3(0.05f, 0.76f, 0.64f), glm::vec3(0.5f, 0.5f, 0.5f));

        // Point light #1
        cubeShader.setPointLight(0, pointLightPositions[0],
                                 glm::vec3(pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1,
                                           pointLightColors[0].z * 0.1),
                                 glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z),
                                 glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z),
                                 1.0f, 0.09f, 0.032f);

        // Point light #2
        cubeShader.setPointLight(1, pointLightPositions[1],
                                 glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f),
                                 glm::vec3(1.0f, 1.0f, 1.0f),
                                 1.0f, 0.09f, 0.032f);

        // Point light #3
        cubeShader.setPointLight(2, pointLightPositions[2],
                                 glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f),
                                 glm::vec3(1.0f, 1.0f, 1.0f),
                                 1.0f, 0.09f, 0.032f);

        // Point light #4
        cubeShader.setPointLight(3, pointLightPositions[3],
                                 glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f),
                                 glm::vec3(1.0f, 1.0f, 1.0f),
                                 1.0f, 0.09f, 0.032f);

        // Spotlight
        cubeShader.setSpotLight(camera.Position, camera.Front,
                                glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)),
                                glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
                                1.0f, 0.09f, 0.032f);

        cubeShader.setFloat("u_Time", currentFrame);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1F,
                                                100.0F);
        glm::mat4 view = camera.GetViewMatrix();
        cubeShader.setMat4("u_Projection", projection);
        cubeShader.setMat4("u_View", view);
        glm::mat4 model = glm::mat4(1.0f);
        cubeShader.setMat4("u_Model", model);

        // assign the non-specular texture to textureSlot 0
        cubeShader.setInt("u_Material.texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // assign the specular texture to textureSlot 1
        cubeShader.setInt("u_Material.texture_specular1", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        cubeShader.setInt("u_Material.emission", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);

        incAngle += 0.05f;

        // Renders the wooden boxes. Uses a more manual shader setup as opposed to the below leaf models, imported via Assimp.
        glBindVertexArray(VAOs[1]); // cube objects, scattered throughout view
        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            if (i % 3 == 0) {
                model = glm::rotate(model, glm::radians(incAngle), glm::vec3(1.0f, 0.3f, 0.5f));
            } else {
                model = glm::rotate(model, glm::radians(angle * i), glm::vec3(1.0f, 0.3f, 0.5f));
            }
            cubeShader.setMat4("u_Model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        leafShader.use();
        leafShader.setVec3("u_ViewPos", camera.Position);
        leafShader.setFloat("u_Material.shininess", 32.0f);

        // Directional light
        leafShader.setDirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.1f, 0.5f, 0.3f),
                                       glm::vec3(0.05f, 0.76f, 0.64f), glm::vec3(0.5f, 0.5f, 0.5f));

        // Point light #1
        leafShader.setPointLight(0, pointLightPositions[0], glm::vec3(pointLightColors[0].x * 0.1,
                                                                      pointLightColors[0].y * 0.1,
                                                                      pointLightColors[0].z * 0.1),
                                 glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z),
                                 glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z), 1.0f,
                                 0.09f, 0.032f);

        // Point light #1
        leafShader.setPointLight(0, pointLightPositions[0],
                                 glm::vec3(pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1,
                                           pointLightColors[0].z * 0.1),
                                 glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z),
                                 glm::vec3(pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z),
                                 1.0f, 0.09f, 0.032f);

        // Point light #2
        leafShader.setPointLight(1, pointLightPositions[0],
                                 glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f),
                                 glm::vec3(1.0f, 1.0f, 1.0f),
                                 1.0f, 0.09f, 0.032f);

        // Point light #3
        leafShader.setPointLight(2, pointLightPositions[2],
                                 pointLightColors[0], pointLightColors[1], pointLightColors[2],
                                 1.0f, 0.09f, 0.032f);

        // Point light #4
        leafShader.setPointLight(3, pointLightPositions[3],
                                 glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f),
                                 glm::vec3(1.0f, 1.0f, 1.0f),
                                 1.0f, 0.09f, 0.032f);

        // Spotlight
        leafShader.setSpotLight(camera.Position, camera.Front,
                                glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)),
                                glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
                                1.0f, 0.09f, 0.032f);

        leafShader.setFloat("u_Time", currentFrame);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1F,
                                      100.0F);
        view = camera.GetViewMatrix();
        leafShader.setMat4("u_Projection", projection);
        leafShader.setMat4("u_View", view);
        model = glm::mat4(1.0f);

        glm::vec3 yOffset = glm::vec3(.0f, 1.0f, 0.0f);


        // alternate between positive and negative y-translation based on target flag
        const float targetY = 2.0f;
        const float decayRate = 1.5f;
        float target = leafTargetUp ? targetY : 0.0f;
        leafHeight += (target - leafHeight) * (1.0f - std::exp(-decayRate * deltaTime));


        float angleTargetUp = 500.0f;
        float angleTarget = leafTargetUp ? angleTargetUp : static_cast<float>(sin(glfwGetTime())) * 45.0f;
        leafSpingAngle += (angleTarget - leafSpingAngle) * (1.0f - std::exp(-decayRate * deltaTime));

        const float vibrationRate = static_cast<float>(sin(glfwGetTime() * 100));
        bool isVibrationThreshold = leafHeight >= targetY - 0.8f;


        if (queryInstancedPerformance) {
            glBeginQuery(GL_TIME_ELAPSED, queryID[0]);

            std::vector<glm::mat4> leafModels;
            leafModels.reserve(leafPositions.size());

            for (int i = 0; i < leafPositions.size(); i++) {
                glm::vec3 leafWorldPos = leafPositions[i] + yOffset * leafHeight;
                if (isVibrationThreshold) {
                    leafWorldPos.x += vibrationRate * 0.002f;
                }

                model = glm::mat4(1.0f);
                model = glm::translate(model, leafWorldPos);
                model = glm::scale(model, glm::vec3(leafScales[i]));

                model = glm::rotate(model, glm::radians(leafSpingAngle),
                                    glm::vec3(0.0f, 1.0f, 0.0f));
                leafModels.push_back(model);
            }

            leafShader.setBool("u_ConditionalOptimizer", true);

            modelObj.drawInstanced(leafShader, leafModels);

            glEndQuery(GL_TIME_ELAPSED);

            GLint success = 0;
            glGetQueryObjectiv(queryID[0], GL_QUERY_RESULT_AVAILABLE, &success);
            GLint64 elapsed_ns; // nanoseconds
            glGetQueryObjecti64v(queryID[0], GL_QUERY_RESULT, &elapsed_ns);
            // potentially queries the result of GL_QUERY_RESULT_AVAILABLE before it returned by the asynchronous function. If that's the case, CPU will be stalled. Haven't found a way to use the async query result yet.
            printf("INSTANCED DRAW: GPU took: %.3f ms to draw.\n", elapsed_ns / 1000000.0);
        } else {
            glBeginQuery(GL_TIME_ELAPSED, queryID[1]);
            // begin GPU frame query scope. Returns the time elapsed value as part of an async call.

            for (int i = 0; i < leafPositions.size(); ++i) {
                glm::vec3 leafWorldPos = leafPositions[i] + yOffset * leafHeight;
                if (isVibrationThreshold) {
                    leafWorldPos.x += vibrationRate * 0.002f;
                }

                model = glm::mat4(1.0f);
                model = glm::translate(model, leafWorldPos);
                model = glm::scale(model, glm::vec3(leafScales[i]));

                model = glm::rotate(model, glm::radians(leafSpingAngle),
                                    glm::vec3(0.0f, 1.0f, 0.0f));
                leafShader.setBool("u_ConditionalOptimizer", false);
                leafShader.setMat4("u_Model", model);
                modelObj.draw(leafShader);
            }
            glEndQuery(GL_TIME_ELAPSED);

            GLint success = 0;
            glGetQueryObjectiv(queryID[1], GL_QUERY_RESULT_AVAILABLE, &success);
            GLint64 elapsed_ns; // nanoseconds
            glGetQueryObjecti64v(queryID[1], GL_QUERY_RESULT, &elapsed_ns);
            // potentially queries the result of GL_QUERY_RESULT_AVAILABLE before it returned by the asynchronous function. If that's the case, CPU will be stalled. Haven't found a way to use the async query result yet.
            printf("NON-INSTANCED DRAW: GPU took: %.3f ms to draw.\n", elapsed_ns / 1000000.0);
        }


        lightSourceShader.use();
        lightSourceShader.setMat4("u_Projection", projection);
        lightSourceShader.setMat4("u_View", view);

        glBindVertexArray(VAOs[0]);
        const size_t pointLightCount = sizeof(pointLightPositions) / sizeof(pointLightPositions[0]);
        for (size_t i = 0; i < pointLightCount; i++) {
            lightSourceShader.setVec3("u_Color", pointLightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));

            lightSourceShader.setMat4("u_Model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose. This gets cleaned up on application quit regardless, but for verbosity's sake.
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteProgram(lightSourceShader.ID);
    glDeleteProgram(leafShader.ID);
    glDeleteProgram(cubeShader.ID);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}


static bool fKeyWasDown = false;
static bool tKeyWasDown = false;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    // leaf hover
    bool fKeyIsDown = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (fKeyIsDown && !fKeyWasDown) {
        leafTargetUp = !leafTargetUp;
    }
    fKeyWasDown = fKeyIsDown;

    bool fKeyIsReleased = glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE;
    if (fKeyIsReleased && leafTargetUp) {
        leafTargetUp = !leafTargetUp;
    }

    bool tKeyIsDown = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
    if (tKeyIsDown && !tKeyWasDown) {
        queryInstancedPerformance = !queryInstancedPerformance;
    }
    tKeyWasDown = tKeyIsDown;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessCameraAcceleration(PRESSED);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.ProcessCameraAcceleration(RELEASED);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

    lastMouseX = xpos;
    lastMouseY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
