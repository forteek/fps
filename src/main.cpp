#include <iostream>
#include <map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "shader/shader.h"
#include "camera/camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
void mouse_callback(GLFWwindow* window, double x, double y);
void draw_scene(Shader shader);
void remove_vector_value(int value, std::vector<int> &vec);
void handle_keys();

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);

std::map<int, std::vector<int>> key_groups {
    {GLFW_KEY_W, {MOVEMENT_KEYS}},
    {GLFW_KEY_S, {MOVEMENT_KEYS}},
    {GLFW_KEY_A, {MOVEMENT_KEYS}},
    {GLFW_KEY_D, {MOVEMENT_KEYS}},
};
std::map<int, int> key_mappings {
    {GLFW_KEY_W, MOVE_FORWARD},
    {GLFW_KEY_S, MOVE_BACKWARD},
    {GLFW_KEY_A, MOVE_LEFT},
    {GLFW_KEY_D, MOVE_RIGHT},
};
std::vector<int> pressed_keys {};

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
    glClearColor(0.75, 0.75, 1, 1);
    glEnable(GL_DEPTH_TEST);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    return window;
}

int main() {
    GLFWwindow* window = initialize_program();

    Shader shader("../src/shader/simple_v.glsl", "../src/shader/simple_f.glsl");

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load("../src/chest.png", &texWidth, &texHeight, &nrChannels, 0);

    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        fprintf(stderr, "Failed to load texture.");
    }
    stbi_image_free(image);

    float vertices[] = {
            // positions                 // tex coords
            -1.0f, -1.0f,  1.0f, 1.0f,   0.0f, 0.5f, // front left bottom
            -1.0f,  1.0f,  1.0f, 1.0f,   0.0f, 1.0f, // front left top
             1.0f, -1.0f,  1.0f, 1.0f,   0.5f, 0.5f, // front right bottom
            -1.0f,  1.0f,  1.0f, 1.0f,   0.0f, 1.0f, // front left top
             1.0f, -1.0f,  1.0f, 1.0f,   0.5f, 0.5f, // front right bottom
             1.0f,  1.0f,  1.0f, 1.0f,   0.5f, 1.0f, // front right top

            -1.0f, -1.0f, -1.0f, 1.0f,   0.5f, 0.0f, // back left bottom
            -1.0f,  1.0f, -1.0f, 1.0f,   0.5f, 0.5f, // back left top
             1.0f, -1.0f, -1.0f, 1.0f,   1.0f, 0.0f, // back right bottom
            -1.0f,  1.0f, -1.0f, 1.0f,   0.5f, 0.5f, // back left top
             1.0f, -1.0f, -1.0f, 1.0f,   1.0f, 0.0f, // back right bottom
             1.0f,  1.0f, -1.0f, 1.0f,   1.0f, 0.5f, // back right top

            -1.0f, -1.0f, -1.0f, 1.0f,   0.5f, 0.0f, // back left bottom
            -1.0f,  1.0f, -1.0f, 1.0f,   0.5f, 0.5f, // back left top
            -1.0f, -1.0f,  1.0f, 1.0f,   1.0f, 0.0f, // front left bottom
            -1.0f,  1.0f, -1.0f, 1.0f,   0.5f, 0.5f, // back left top
            -1.0f, -1.0f,  1.0f, 1.0f,   1.0f, 0.0f, // front left bottom
            -1.0f,  1.0f,  1.0f, 1.0f,   1.0f, 0.5f, // front left top

             1.0f, -1.0f, -1.0f, 1.0f,   0.5f, 0.0f, // back right bottom
             1.0f,  1.0f, -1.0f, 1.0f,   0.5f, 0.5f, // back right top
             1.0f, -1.0f,  1.0f, 1.0f,   1.0f, 0.0f, // front right bottom
             1.0f,  1.0f, -1.0f, 1.0f,   0.5f, 0.5f, // back right top
             1.0f, -1.0f,  1.0f, 1.0f,   1.0f, 0.0f, // front right bottom
             1.0f,  1.0f,  1.0f, 1.0f,   1.0f, 0.5f, // front right top

            -1.0f,  1.0f,  1.0f, 1.0f,   0.0f, 0.0f, // front left top
             1.0f,  1.0f,  1.0f, 1.0f,   0.0f, 0.5f, // front right top
            -1.0f,  1.0f, -1.0f, 1.0f,   0.5f, 0.0f, // back left top
             1.0f,  1.0f,  1.0f, 1.0f,   0.0f, 0.5f, // front right top
            -1.0f,  1.0f, -1.0f, 1.0f,   0.5f, 0.0f, // back left top
             1.0f,  1.0f, -1.0f, 1.0f,   0.5f, 0.5f, // back right top

            -1.0f, -1.0f,  1.0f, 1.0f,   0.0f, 0.0f, // front left bottom
             1.0f, -1.0f,  1.0f, 1.0f,   0.0f, 0.5f, // front right bottom
            -1.0f, -1.0f, -1.0f, 1.0f,   0.5f, 0.0f, // back left bottom
             1.0f, -1.0f,  1.0f, 1.0f,   0.0f, 0.5f, // front right bottom
            -1.0f, -1.0f, -1.0f, 1.0f,   0.5f, 0.0f, // back left bottom
             1.0f, -1.0f, -1.0f, 1.0f,   0.5f, 0.5f, // back right bottom
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(shader.attribute("vertex"), 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(shader.attribute("texCoord"), 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handle_keys();
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

    glm::mat4 projection = glm::perspective(glm::radians(camera.get_fov()), 800.0f/600.0f, 0.1f, 100.0f);
    glm::mat4 view = camera.get_view_matrix();
    glm::mat4 model = glm::mat4(1.0f);
//    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1, 0));

    shader.setUniformMatrix("projection", projection);
    shader.setUniformMatrix("view", view);
    shader.setUniformMatrix("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void handle_keys()
{
    for (int &key : pressed_keys) {
        for (int &keyGroup : key_groups[key]) {
            if (keyGroup == MOVEMENT_KEYS) {
                camera.handle_key(key_mappings[key], deltaTime);
            }
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    if (action == GLFW_PRESS) {
        pressed_keys.push_back(key);
    } else if (action == GLFW_RELEASE) {
        remove_vector_value(key, pressed_keys);
    }
}

void mouse_callback(GLFWwindow*, double x, double y)
{
    camera.handle_mouse(x, y);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void remove_vector_value(int value, std::vector<int> &vec)
{
    vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}
