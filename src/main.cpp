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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

    glViewport(0, 0, 800, 600);
    glClearColor(0.5, 1, 0.75, 1);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    return window;
}

int main() {
    GLFWwindow* window = initialize_program();

    Shader shader("../src/shader/constant_v.glsl", "../src/shader/constant_f.glsl");
    shader.use();

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_scene(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void draw_scene(Shader shader) {
    glm::mat4 projection = glm::perspective(glm::radians(50.0f), 800.0f/600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, -5.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    glm::mat4 model = glm::mat4(1.0f);

    glUniformMatrix4fv(shader.uniform("projection"), 1, false, glm::value_ptr(projection));
    glUniformMatrix4fv(shader.uniform("view"), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(shader.uniform("model"), 1, false, glm::value_ptr(model));

    float verts[]={
            0, 10, 0, 1,
            -10, -10, 0, 1,
            10, -10, 0, 1
    };
    int vertexCount=3;

    glEnableVertexAttribArray(shader.attribute("vertex"));
    glVertexAttribPointer(shader.attribute("vertex"), 4, GL_FLOAT, false, 0, verts);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableVertexAttribArray(shader.attribute("vertex"));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod) {}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
