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
    // positions          // texcoords   // normals
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,    0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,    0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,    0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,    0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,    0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,    0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,    0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,    0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,    0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,    0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,    0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,    0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,    1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,    1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 1.0f,    1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 1.0f,    1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, 0.0f,    1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,    1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,    0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f,    0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,    0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,    0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,    0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,    0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,    0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,    0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,    0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,    0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,    0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,    0.0f,  1.0f,  0.0f
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


    unsigned int VBOs[2], VAOs[2]; // CPU
    glGenVertexArrays(2, VAOs); // Generates two separate vertex array objects (vao:s).  CPU
    glGenBuffers(2, VBOs); // Generates two separate vertex buffer objects (vbo:s). CPU

    // Texture boxes + light source object. Uses vertex positions and texture coordinates - discards normals
    glBindVertexArray(VAOs[0]);
    // Sets the current vertex array object target. CPU. VAO stores the vertax buffer layout, which is a way to determine which set of bytes correspond to which "graphical element", such as vertex positions, colors, textures, textureID's, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); // CPU.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // light hit object - uses the vertex positons, Normals but discards texture coords.
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // vertex surface normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (5 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

    shaderOrangee.use();
    glUniform1i(glGetUniformLocation(shaderOrangee.ID, "texture1"), 0);
    shaderOrangee.setInt("texture2", 1);


    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // prepare to use the first shader program
        shaderOrangee.use();
        glm::mat4 model = glm::mat4(1.0f); //identity matrix
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1F, 100.0F);
        view = camera.GetViewMatrix();

        unsigned int viewLoc = glGetUniformLocation(shaderOrangee.ID, "u_View");
        shaderOrangee.setMat4("u_Projection", projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        shaderOrangee.setFloat("mixValue", mixValue);
        glBindVertexArray(VAOs[0]);

        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            if (i % 3 == 0) {
                incAngle += 0.01f;
                model = glm::rotate(model, glm::radians(incAngle), glm::vec3(1.0f, 0.3f, 0.5f));
            } else {
                model = glm::rotate(model, glm::radians(angle * i), glm::vec3(1.0f, 0.3f, 0.5f));
            }
            shaderOrangee.setMat4("u_Model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Switch active shader target (light source)
        lightSourceShader.use();

        lightSourceShader.setMat4("u_Projection", projection);
        lightSourceShader.setMat4("u_View", view);

        glm::mat4 lightSourceModel = glm::mat4(1.0f);
        lightSourceModel = glm::translate(lightSourceModel, lightPos);
        lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.2f));
        lightSourceShader.setMat4("u_Model", lightSourceModel);
        // Switch active VAO target
        glBindVertexArray(VAOs[1]);
        // Draw the lightsource object
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Switch active shader target (light object)
        lightObjectShader.use();
        // Set uniform colors
        lightObjectShader.setVec3("u_lightSourcePos", lightPos);
        lightObjectShader.setVec3("u_ViewPos", camera.Position);

        // Set uniform material properties
        lightObjectShader.setVec3("u_Material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        lightObjectShader.setVec3("u_Material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        lightObjectShader.setVec3("u_Material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        lightObjectShader.setFloat("u_Material.shininess", 32.0f);

        // Set light source uniforms
        lightObjectShader.setVec3("u_Light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
        lightObjectShader.setVec3("u_Light.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
        lightObjectShader.setVec3("u_Light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        // Source light colors
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        lightObjectShader.setVec3("u_Light.ambient", ambientColor);
        lightObjectShader.setVec3("u_Light.diffuse", diffuseColor);

        // Set uniform matrices (project & view)
        lightObjectShader.setMat4("u_Projection", projection);
        lightObjectShader.setMat4("u_View", view);

        // Reset model matrix. Experimenting with not having to re-define model matrices per object type but rather just reset them.
        float time = glfwGetTime();
        float sinAng = time * 100.0f + sin(time * 2) * 20 ;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPos.x, lightPos.y - 1.f, lightPos.z + 2.f)); // Try to offset the cube to be affected by light a little from the light source.
        model = glm::rotate(model, glm::radians(sinAng), glm::vec3(lightPos.x, lightPos.y - 1.f, lightPos.z + 2.f));
        lightObjectShader.setMat4("u_Model", model);


        // VAO 1 is still bound but will suffice
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
