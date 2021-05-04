#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader/shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
void draw_scene(Shader shader);

GLFWwindow* initialize_program() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "fps", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to initialize window.");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD.");
        exit(EXIT_FAILURE);
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.5, 1, 0.75, 1);
    glEnable(GL_DEPTH_TEST);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    return window;
}

int main() {
    GLFWwindow* window = initialize_program();

    Shader shader("../src/shader/simple_v.glsl", "../src/shader/simple_f.glsl");

    float vertices[] = {
            0, 1, 0, 1,
            -1, -1, 1, 1,
            1, -1, 1, 1,
            -1, -1, -1, 1,
            1, -1, -1, 1
    };
    float colors[] = {
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
            1, 1, 0,
            0, 1, 1
    };
    unsigned int indices[] = {
            0, 1, 2,
            0, 1, 3,
            0, 3, 4,
            0, 2, 4,
            1, 2, 3,
            2, 3, 4
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(shader.attribute("vertex"), 4, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(shader.attribute("color"), 3, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_scene(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void draw_scene(Shader shader) {
    shader.use();

    glm::mat4 projection = glm::perspective(glm::radians(50.0f), 800.0f/600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, -5.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));

    shader.setUniformMatrix("projection", projection);
    shader.setUniformMatrix("view", view);
    shader.setUniformMatrix("model", model);

    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, nullptr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod) {}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
