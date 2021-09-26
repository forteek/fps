#include <iostream>
#include <map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

#include "shader/shader.h"
#include "camera/camera.h"
#include "model/model.h"
#include "light/light.h"
#include "light/directional_light.h"
#include "light/point_light.h"

using std::vector;
using std::map;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
void mouse_callback(GLFWwindow* window, double x, double y);
void draw_scene(
    Shader shader,
    Shader lampShader,
    Shader wavyShader,
    Model fish,
    Model seaweed,
    Model sand,
    unsigned int lampVAO,
    const vector<Light*>& lights
);
vector<Light *> generate_lights();
void generate_seaweed();
void remove_vector_value(int value, vector<int> &vec);
void handle_keys();

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(
    glm::vec3(0.0f, 1.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);

map<int, vector<int>> key_groups {
    {GLFW_KEY_W, {MOVEMENT_KEYS}},
    {GLFW_KEY_S, {MOVEMENT_KEYS}},
    {GLFW_KEY_A, {MOVEMENT_KEYS}},
    {GLFW_KEY_D, {MOVEMENT_KEYS}},
};
map<int, int> key_mappings {
    {GLFW_KEY_W, MOVE_FORWARD},
    {GLFW_KEY_S, MOVE_BACKWARD},
    {GLFW_KEY_A, MOVE_LEFT},
    {GLFW_KEY_D, MOVE_RIGHT},
};
vector<int> pressed_keys {};

struct Seaweed {
    glm::vec2 coords;
    float waveStrength;
    float scale;
    float rotation;
};
vector<Seaweed> seaweed_data {};

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

    Shader shader("../src/shader/model_vertex.glsl", "../src/shader/model_fragment.glsl");
    Shader lampShader("../src/shader/lamp_v.glsl", "../src/shader/lamp_f.glsl");
    Shader wavyShader("../src/shader/wavy_vertex.glsl", "../src/shader/model_fragment.glsl");

    float cubeVertices[] = {
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    unsigned int lampVAO;
    glGenVertexArrays(1, &lampVAO);
    glBindVertexArray(lampVAO);

    glVertexAttribPointer(lampShader.attribute("vertex"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Model fish("../models/fish/ryba.obj");
    Model seaweed("../models/seaweed/glon.obj");
    vector<Light*> lights = generate_lights();
    generate_seaweed();
    Model sand("../models/sand/sand.obj");

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handle_keys();
        draw_scene(shader, lampShader, wavyShader, fish, seaweed, sand, lampVAO, lights);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void draw_scene(
    Shader shader,
    Shader lampShader,
    Shader wavyShader,
    Model fish,
    Model seaweed,
    Model sand,
    unsigned int lampVAO,
    const vector<Light*>& lights
) {
    glm::mat4 projection = glm::perspective(glm::radians(camera.get_fov()), 800.0f/600.0f, 0.1f, 100.0f);
    glm::mat4 view = camera.get_view_matrix();

    shader.use();

    shader.setUniformMatrix("projection", projection);
    shader.setUniformMatrix("view", view);
    shader.setUniformVec3("viewPos", camera.get_position());

    int i = 0;
    for (auto light : lights) {
        shader.use();
        shader.setUniformInt("lights[" + std::to_string(i) + "].type", static_cast<int>(light->get_type()));
        shader.setUniformVec3("lights[" + std::to_string(i) + "].ambient", light->get_ambient());
        shader.setUniformVec3("lights[" + std::to_string(i) + "].diffuse", light->get_diffuse());
        shader.setUniformVec3("lights[" + std::to_string(i) + "].specular", light->get_specular());

        wavyShader.use();
        wavyShader.setUniformInt("lights[" + std::to_string(i) + "].type", static_cast<int>(light->get_type()));
        wavyShader.setUniformVec3("lights[" + std::to_string(i) + "].ambient", light->get_ambient());
        wavyShader.setUniformVec3("lights[" + std::to_string(i) + "].diffuse", light->get_diffuse());
        wavyShader.setUniformVec3("lights[" + std::to_string(i) + "].specular", light->get_specular());

        switch (light->get_type()) {
            case LightType::DIRECTIONAL:
                shader.use();
                shader.setUniformVec3("lights[" + std::to_string(i) + "].direction", light->get_direction());
                wavyShader.use();
                wavyShader.setUniformVec3("lights[" + std::to_string(i) + "].direction", light->get_direction());
                shader.use();
                break;
            case LightType::POINT:
                shader.use();
                shader.setUniformVec3("lights[" + std::to_string(i) + "].position", light->get_position());
                shader.setUniformFloat("lights[" + std::to_string(i) + "].constant", light->get_constant());
                shader.setUniformFloat("lights[" + std::to_string(i) + "].linear", light->get_linear());
                shader.setUniformFloat("lights[" + std::to_string(i) + "].quadratic", light->get_quadratic());
                wavyShader.use();
                wavyShader.setUniformVec3("lights[" + std::to_string(i) + "].position", light->get_position());
                wavyShader.setUniformFloat("lights[" + std::to_string(i) + "].constant", light->get_constant());
                wavyShader.setUniformFloat("lights[" + std::to_string(i) + "].linear", light->get_linear());
                wavyShader.setUniformFloat("lights[" + std::to_string(i) + "].quadratic", light->get_quadratic());

                lampShader.use();

                glm::mat4 lampMatrix = glm::mat4(1.0f);
                lampMatrix = glm::translate(lampMatrix, light->get_position());
                lampMatrix = glm::scale(lampMatrix, glm::vec3(0.2f));

                lampShader.setUniformMatrix("projection", projection);
                lampShader.setUniformMatrix("view", view);
                lampShader.setUniformMatrix("model", lampMatrix);

                lampShader.setUniformVec3("color", light->get_specular());

                glBindVertexArray(lampVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                shader.use();
        }

        i++;
    }

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    auto n = (float)glfwGetTime();
    modelMatrix = glm::translate(modelMatrix, glm::vec3(
            cos(n / 2) * 5,
            1.0f,
            sin(n / 2) * 5
    ));
    modelMatrix = glm::rotate(modelMatrix, (-n/2) + (sin(n * 2) * cos(n * 2) / 2), glm::vec3(0,1,0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
    shader.setUniformMatrix("model", modelMatrix);
    fish.draw(shader);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 1.0f, 10.0f));
    shader.setUniformMatrix("model", modelMatrix);
    sand.draw(shader);

    wavyShader.use();
    for (int seaweedIndex = 0; seaweedIndex <= 100; seaweedIndex++) {
        Seaweed instance = seaweed_data[seaweedIndex];
        modelMatrix = glm::mat4(1.0f);

        modelMatrix = glm::translate(modelMatrix,glm::vec3(
                instance.coords.x,
                0.0f,
                instance.coords.y
        ));
        modelMatrix = glm::rotate(modelMatrix, instance.rotation, glm::vec3(0,1,0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(instance.scale));

        wavyShader.setUniformMatrix("projection", projection);
        wavyShader.setUniformMatrix("view", view);
        wavyShader.setUniformMatrix("model", modelMatrix);
        wavyShader.setUniformFloat("currentTime", n);

        seaweed.draw(wavyShader);
    }
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

void remove_vector_value(int value, vector<int> &vec)
{
    vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}

vector<Light*> generate_lights() {
    std::vector<Light*> lights;

    lights.push_back(new DirectionalLight(
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3(0.05f, 0.05f, 0.05f),
            glm::vec3(0.4f, 0.4f, 0.4f),
            glm::vec3(0.5f, 0.5f, 0.5f)
    ));
    lights.push_back(new PointLight(
            glm::vec3(-0.2f, 2.0f, -0.3f),
            1.0f,
            0.09f,
            0.032f,
            glm::vec3(0.05f, 0.05f, 0.05f),
            glm::vec3(0.8f, 0.8f, 0.8f),
            glm::vec3(1.0f, 1.0f, 1.0f)
    ));

    return lights;
}

void generate_seaweed() {
    std::default_random_engine generator(10);
    std::uniform_real_distribution<float> coordsDistribution(-10,10);
    std::uniform_real_distribution<float> waveStrengthDistribution(0.3,1);
    std::uniform_real_distribution<float> scaleDistribution(0.3,0.7);
    std::uniform_real_distribution<float> rotationDistribution(-0.2,0.2);

    for (int i = 0; i < 100; i++) {
        Seaweed seaweed{};
        seaweed.coords = glm::vec2(coordsDistribution(generator), coordsDistribution(generator));
        seaweed.waveStrength = waveStrengthDistribution(generator);
        seaweed.scale = scaleDistribution(generator);
        seaweed.rotation = rotationDistribution(generator) * 90;

        seaweed_data.push_back(seaweed);
    }
}
