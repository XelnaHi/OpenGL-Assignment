#include <cmath>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "stb/stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.2f;

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
    glfwSwapInterval(1); // enables vsync, syncs to monitor refresh
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // CPU

    // CPU asks drivers for OS-specific function pointers
    if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress)) // CPU, prepping for GPU integration
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // These two initializations heavily reduce code clutter in main file.
    Shader shaderOrangee("shaders/secondBasic.vert", "shaders/basicShaderOrange.frag");
    Shader shaderYellow("shaders/secondBasic.vert", "shaders/basicShaderYellow.frag");

    // set up vertex data, vertex buffer objects and vertex attribute pointers.
    // First vertex data
    float firstTriangle[] = {
        // CPU
        -0.9f, -0.5f, 0.0f, // left
        -0.0f, -0.5f, 0.0f, // right
        -0.45f, 0.5f, 0.0f, // top
    };

    // Second vertex data
    float secondTriangle[] = {
        // CPU
        0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // left
        0.9f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // right
        0.45f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f // top
    };

    float texCoords[] = {
        0.0f, 0.0f, // lower-left corner
        1.0f, 0.0f, // lower-right corner
        0.5f, 1.0f // top-center corner
    };

    float vertices[] = {
        // positions            // colors           // texture coords
        0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f,      1.0f, 1.0f, 0.0f,   0.0f, 1.0f // top left
    };


    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
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

    // texture 2
    // ---------
        glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("res/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data) {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    unsigned int VBOs[2], VAOs[2]; // CPU
    glGenVertexArrays(2, VAOs); // Generates two separate vertex array objects (vao:s).  CPU
    glGenBuffers(2, VBOs); // Generates two separate vertex buffer objects (vbo:s). CPU

    // first triangle setup
    glBindVertexArray(VAOs[0]);
    // Sets the current vertex array object target. CPU. VAO stores the vertax buffer layout, which is a way to determine which set of bytes correspond to which "graphical element", such as vertex positions, colors, textures, textureID's, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); // CPU.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    // Vertex attributes stay the same
    glEnableVertexAttribArray(2);

    // position — location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // color — location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // second triangle setup
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]); // and a different VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float horTrans = 0.001f;

    shaderOrangee.use();
    glUniform1i(glGetUniformLocation(shaderOrangee.ID, "texture1"), 0);
    // or set it via the texture class
    shaderOrangee.setInt("texture2", 1);
    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // prepare to use the first shader program when rendering the first triangle
        shaderOrangee.use();
        shaderOrangee.setFloat("mixValue", mixValue);
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // then we draw the second triangle using the data from the second VAO
        // when we draw the second triangle we want to use a different shader program so we switch to the shader program with our yellow fragment shader.

        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        shaderYellow.use();
        shaderYellow.SetUniform4f("ourColor", 0.0f, greenValue, 0.0f, 1.0f);
        shaderYellow.setFloat("u_HorTrans", horTrans);
        horTrans += 0.001f;

        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3); // this call should output a yellow triangle

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

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        std::cout << "mix value: " << mixValue << std::endl;
        mixValue += 0.01f;
        if (mixValue >= 1.0f)
            mixValue = 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        std::cout << "mix value: " << mixValue << std::endl;
        mixValue -= 0.01f;
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
