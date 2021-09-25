#include <iostream>
#include <map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "shader/shader.h"
#include "camera/camera.h"
#include "light/light.h"
#include "light/directional_light.h"
#include "light/point_light.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
void mouse_callback(GLFWwindow* window, double x, double y);
void draw_scene(Shader shader, Shader lightShader, unsigned int cubeVAO, unsigned int lampVAO, unsigned int diffuseMap, unsigned int specularMap, const std::vector<Light*>& lights);
void remove_vector_value(int value, std::vector<int> &vec);
void handle_keys();
unsigned int loadTexture(const char *path);
std::vector<Light *> generate_lights();

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
    glfwWindowHint(GLFW_SAMPLES, 4);

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

    Shader shader("../src/shader/lighted_v.glsl", "../src/shader/lighted_f.glsl");
    Shader lightShader("../src/shader/lamp_v.glsl", "../src/shader/lamp_f.glsl");

    float cubeVertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(shader.attribute("vertex"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(shader.attribute("normal"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(shader.attribute("texCoords"), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lampVAO;
    glGenVertexArrays(1, &lampVAO);
    glBindVertexArray(lampVAO);

    glVertexAttribPointer(lightShader.attribute("vertex"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int diffuseMap = loadTexture("../src/container2.png");
    unsigned int specularMap = loadTexture("../src/container2_specular.png");

    std::vector<Light*> lights = generate_lights();

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handle_keys();
        draw_scene(shader, lightShader, cubeVAO, lampVAO, diffuseMap, specularMap, lights);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lampVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void draw_scene(
    Shader shader,
    Shader lightShader,
    unsigned int cubeVAO,
    unsigned int lampVAO,
    unsigned int diffuseMap,
    unsigned int specularMap,
    const std::vector<Light*>& lights
) {
    glm::mat4 projection = glm::perspective(glm::radians(camera.get_fov()), 800.0f/600.0f, 0.1f, 100.0f);
    glm::mat4 view = camera.get_view_matrix();

    shader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1, 0));

    shader.setUniformMatrix("projection", projection);
    shader.setUniformMatrix("view", view);
    shader.setUniformMatrix("model", model);
    shader.setUniformVec3("viewPos", camera.get_position());

    shader.setUniformVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setUniformInt("material.ambient", 0);
    shader.setUniformInt("material.specular", 1);
    shader.setUniformFloat("material.shininess", 32.0f);

    int i = 0;
    for (auto light : lights) {
        shader.setUniformInt("lights[" + std::to_string(i) + "].type", static_cast<int>(light->get_type()));
        shader.setUniformVec3("lights[" + std::to_string(i) + "].ambient", light->get_ambient());
        shader.setUniformVec3("lights[" + std::to_string(i) + "].diffuse", light->get_diffuse());
        shader.setUniformVec3("lights[" + std::to_string(i) + "].specular", light->get_specular());

        switch (light->get_type()) {
            case LightType::DIRECTIONAL:
                shader.setUniformVec3("lights[" + std::to_string(i) + "].direction", light->get_direction());
                break;
            case LightType::POINT:
                shader.setUniformVec3("lights[" + std::to_string(i) + "].position", light->get_position());
                shader.setUniformFloat("lights[" + std::to_string(i) + "].constant", light->get_constant());
                shader.setUniformFloat("lights[" + std::to_string(i) + "].linear", light->get_linear());
                shader.setUniformFloat("lights[" + std::to_string(i) + "].quadratic", light->get_quadratic());

                lightShader.use();

                model = glm::mat4(1.0f);
                model = glm::translate(model, light->get_position());
                model = glm::scale(model, glm::vec3(0.2f));

                lightShader.setUniformMatrix("projection", projection);
                lightShader.setUniformMatrix("view", view);
                lightShader.setUniformMatrix("model", model);

                lightShader.setUniformVec3("color", light->get_specular());

                glBindVertexArray(lampVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                shader.use();
        }

        i++;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glBindVertexArray(cubeVAO);
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

unsigned int loadTexture(char const * path)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int textureWidth, textureHeight, channelCount;
    unsigned char *image = stbi_load(path, &textureWidth, &textureHeight, &channelCount, 0);

    if (!image) {
        fprintf(stderr, "Failed to load texture.");
        stbi_image_free(image);

        return texture;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);

    return texture;
}

std::vector<Light*> generate_lights() {
    std::vector<Light*> lights;

    lights.push_back(new DirectionalLight(
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3(0.05f, 0.05f, 0.05f),
            glm::vec3(0.4f, 0.4f, 0.4f),
            glm::vec3(0.5f, 0.5f, 0.5f)
    ));
    lights.push_back(new PointLight(
            glm::vec3(1.2f, 1.0f, 2.0f),
            1.0f,
            0.09f,
            0.032f,
            glm::vec3(0.2f, 0.0f, 0.0f),
            glm::vec3(0.5f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
    ));
    lights.push_back(new PointLight(
            glm::vec3(-1.2f, -1.0f, -2.0f),
            1.0f,
            0.09f,
            0.032f,
            glm::vec3(0.0f, 0.2f, 0.0f),
            glm::vec3(0.0f, 0.5f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    ));

    return lights;
}
