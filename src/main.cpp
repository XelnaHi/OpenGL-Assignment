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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// screen settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

    // These two initializations heavily reduce code clutter in main file.
    Shader shaderOrangee("shaders/secondBasic.vert", "shaders/basicShaderOrange.frag");
    Shader shaderYellow("shaders/secondBasic.vert", "shaders/basicShaderYellow.frag");
    Shader lightSourceShader("shaders/lightSource.vert", "shaders/lightSource.frag");
    Shader lightObjectShader("shaders/lightObject.vert", "shaders/lightObject.frag");

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
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
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


    unsigned int VBOs[2], VAOs[2]; // CPU-isolated call.
    glGenVertexArrays(2, VAOs); // Generates unique integer ID's on the GPU. The call itself to store these values are issued via CPU. Will be used to store configurations detailing how memory on GPU is supposed to be read. (see AttribPointers below.)
    glGenBuffers(2, VBOs); // Generates unique integer ID's for buffer objects on GPU. Call is issued from CPU. The buffer will later the allocated memory in GPU.

    // light source object
    glBindVertexArray(VAOs[0]); // Sets the current active vertex array. Writing data/configurations will be related to this specific VertexArray. VAO stores the vertex buffer layout, which is a way to determine which set of bytes correspond to which "graphical element", such as vertex positions, colors, textures, textureID's, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); // Sets the current active buffer object.
    // vertex positions
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Writes to (allocates) memory on the GPU holding the specified data.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0); // Specifies the memory layout of the buffer object, telling the GPU how the sequential data should be interpreted.
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
     *  The result of the above matrix is calculated using: glm::lookAt(cameraPos, targetPos, worldUp)
     */

    glm::vec3 lampPositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 0.6f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0, 0.0),
        glm::vec3(0.2f, 0.2f, 1.0f)
    };

    lightObjectShader.use();
    lightObjectShader.setInt("u_Material.diffuse", 0);
    lightObjectShader.setInt("u_Material.specular", 1);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightObjectShader.use();
        lightObjectShader.setVec3("u_ViewPos", camera.Position); // determines the spread of light originating from its ppint of impact
        lightObjectShader.setFloat("u_Material.shininess", 32.0f);

        // Directional light
        lightObjectShader.setVec3("u_DirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightObjectShader.setVec3("u_DirLight.ambient", glm::vec3(0.5f, 0.24f, 0.14f));
        lightObjectShader.setVec3("u_DirLight.diffuse", glm::vec3(0.7f, 0.42f, 0.26f));
        lightObjectShader.setVec3("u_DirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        // Point light #1
        lightObjectShader.setVec3("u_PointLights[0].position", pointLightPositions[0]);
        lightObjectShader.setVec3("u_PointLights[0].ambient", glm::vec3(pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1));
        lightObjectShader.setVec3("u_PointLights[0].diffuse", glm::vec3(pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z));
        lightObjectShader.setVec3("u_PointLights[0].specular", glm::vec3(pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z));
        lightObjectShader.setFloat("u_PointLights[0].constant", 1.0f);
        lightObjectShader.setFloat("u_PointLights[0].linear", 0.09f);
        lightObjectShader.setFloat("u_PointLights[0].quadratic", 0.032f);

        // Point light #2
        lightObjectShader.setVec3("u_PointLights[1].position", pointLightPositions[1]);
        lightObjectShader.setVec3("u_PointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightObjectShader.setVec3("u_PointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightObjectShader.setVec3("u_PointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightObjectShader.setFloat("u_PointLights[1].constant", 1.0f);
        lightObjectShader.setFloat("u_PointLights[1].linear", 0.09f);
        lightObjectShader.setFloat("u_PointLights[1].quadratic", 0.032f);

        // Point light #3
        lightObjectShader.setVec3("u_PointLights[2].position", pointLightPositions[2]);
        lightObjectShader.setVec3("u_PointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightObjectShader.setVec3("u_PointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightObjectShader.setVec3("u_PointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightObjectShader.setFloat("u_PointLights[2].constant", 1.0f);
        lightObjectShader.setFloat("u_PointLights[2].linear", 0.09f);
        lightObjectShader.setFloat("u_PointLights[2].quadratic", 0.032f);
        
        // Point light #4
        lightObjectShader.setVec3("u_PointLights[3].position", pointLightPositions[3]);
        lightObjectShader.setVec3("u_PointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightObjectShader.setVec3("u_PointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightObjectShader.setVec3("u_PointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightObjectShader.setFloat("u_PointLights[3].constant", 1.0f);
        lightObjectShader.setFloat("u_PointLights[3].linear", 0.09f);
        lightObjectShader.setFloat("u_PointLights[3].quadratic", 0.032f);

        // u_SpotLight
        lightObjectShader.setVec3("u_SpotLight.position", camera.Position);
        lightObjectShader.setVec3("u_SpotLight.direction", camera.Front);
        lightObjectShader.setVec3("u_SpotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        lightObjectShader.setVec3("u_SpotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        lightObjectShader.setVec3("u_SpotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightObjectShader.setFloat("u_SpotLight.constant", 1.0f);
        lightObjectShader.setFloat("u_SpotLight.linear", 0.09f);
        lightObjectShader.setFloat("u_SpotLight.quadratic", 0.032f);
        lightObjectShader.setFloat("u_SpotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightObjectShader.setFloat("u_SpotLight.outerCutOff", glm::cos(glm::radians(15.0f))); 


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1F,
                                                100.0F);
        glm::mat4 view = camera.GetViewMatrix();
        lightObjectShader.setMat4("u_Projection", projection);
        lightObjectShader.setMat4("u_View", view);
        glm::mat4 model = glm::mat4(1.0f);
        lightObjectShader.setMat4("u_Model", model);

        // assign the non-specular texture to textureSlot 0
        lightObjectShader.setInt("u_Material.diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // assign the specular texture to textureSlot 1
        lightObjectShader.setInt("u_Material.specular", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // lightObjectShader.setInt("u_Material.emission", 2);
        // glActiveTexture(GL_TEXTURE2);
        // glBindTexture(GL_TEXTURE_2D, emissionMap);

        incAngle += 0.05f;

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
            lightObjectShader.setMat4("u_Model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Source light colors
        glm::vec3 lightColor;
        lightColor.x = (float) sin(glfwGetTime() * 2.0f);
        lightColor.y = (float) sin(glfwGetTime() * 0.7f);
        lightColor.z = (float) sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        lightObjectShader.setVec3("u_DirLight.ambient", ambientColor);
        lightObjectShader.setVec3("u_DirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        // change to diffuseColor to get randomized color values based on the color of the light source.

        // Reset model matrix. Experimenting with not having to re-define model matrices per object type but rather just reset them.
        float time = glfwGetTime();
        float sinAng = time * 100.0f + sin(time * 2) * 20;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPos.x, lightPos.y - 1.f, lightPos.z + 2.f));
        // Try to offset the cube to be affected by light a little from the light source.
        model = glm::rotate(model, glm::radians(sinAng), glm::vec3(lightPos.x, lightPos.y - 1.f, lightPos.z + 2.f));
        lightObjectShader.setMat4("u_Model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightSourceShader.use();
        lightSourceShader.setMat4("u_Projection", projection);
        lightSourceShader.setMat4("u_View", view);
        lightSourceShader.setVec3("u_Color", glm::vec3(pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1));

        glBindVertexArray(VAOs[0]); // lamp objects (light sources)
        for (unsigned int i = 0; i < 2; i++) {
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

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    shaderYellow.Unbind();
    shaderOrangee.Unbind();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
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
