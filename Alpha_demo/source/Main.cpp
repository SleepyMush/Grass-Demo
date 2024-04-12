#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include "renderer/Shader.hpp"
#include "renderer/Texture.hpp"
#include "renderer/Model.hpp"
#include <iostream>
//Maths
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

namespace GL {

    GLFWwindow* window = NULL;
    const unsigned int screen_width = 1920;
    const unsigned int screen_height = 1080;

    void Init() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(screen_width, screen_height, "Alpha Demo", NULL, NULL);
        if (window == NULL) {
            std::cout << "GLfw context is incorrect" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
    }

    GLFWwindow* GetWindowPoiner() {
        return window;
    }

    bool WindowShouldNotClose() {
        return !glfwWindowShouldClose(window);
    }

    void CloseWindow() {
        glfwSetWindowShouldClose(window, true);
    }

    void EnableCursor() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void DisableCursor() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    int GetWindowWidth() {
        return screen_width;
    }

    int GetWindowHeight() {
        return screen_height;
    }

    void SetMousePos(double x, double y) {
        glfwSetCursorPos(window, x, y);
    }
}

namespace Input {

    double mouseX = 0;
    double mouseY = 0;
    double mouseOffsetX = 0;
    double mouseOffsetY = 0;
    bool keyPressed[372];
    bool keyDown[372];
    bool keyDownLastFrame[372];

    void Update() {

        // Keyboard
        for (int i = 32; i < 349; i++) {
            if (glfwGetKey(GL::GetWindowPoiner(), i) == GLFW_PRESS) {
                keyDown[i] = true;
            }
            else {
                keyDown[i] = false;
            }
            if (keyDown[i] && !keyDownLastFrame[i]) {
                keyPressed[i] = true;
            }
            else {
                keyPressed[i] = false;
            }
            keyDownLastFrame[i] = keyDown[i];
        }



        // Mouse
        double x, y;
        glfwGetCursorPos(GL::GetWindowPoiner(), &x, &y);
        mouseOffsetX = x - mouseX;
        mouseOffsetY = y - mouseY;
        mouseX = x;
        mouseY = y;
    }

    bool KeyPressed(unsigned int keycode) {
        return keyPressed[keycode];
    }

    bool KeyDown(unsigned int keycode) {
        return keyDown[keycode];
    }
}

struct Camera {

    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);

    glm::mat4 GetViewMatrix() {
        glm::mat4 m = glm::translate(glm::mat4(1), position);
        m = glm::rotate(m, rotation.z, glm::vec3(0, 0, 1));
        m = glm::rotate(m, rotation.y, glm::vec3(0, 1, 0));
        m = glm::rotate(m, rotation.x, glm::vec3(1, 0, 0));
        return glm::inverse(m);
    }
};

struct Plane {

    unsigned int VBO, VAO, EBO;

    Plane() {} // Empty constructor {

    void Load() {

        float vertices[] = {
            // positions         // colors          //Texture 
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,       // bottom right
            -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,       // bottom left
             0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,       // top right
            -0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f        // top left
        };
        unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        3, 2, 0    // second triangle
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
};

Shader shader;
Shader instancedShader;
Model grassModel;
Model grassModelRotated;
Plane plane;
Camera camera;

glm::vec3 _cameraPos(0.0f);
std::unordered_map<std::string, Texture> textures;

void printMat(glm::mat4& mat)
{
    std::cout << glm::to_string(mat) << std::endl;
}


glm::mat4 grassMats[10201];

void Init() {

    GL::Init();

    camera.position = glm::vec3(-0.65, 0.5f, 0.65);
    camera.rotation.x = glm::radians(-35.0f);
    camera.rotation.y = glm::radians(-45.0f);

    textures["Grass"] = Texture("resources/textures/Grass.png");
    //textures["NumGrid"] = Texture("resources/textures/NumGrid.png");
    textures["Grass_Ground_Texture"] = Texture("resources/textures/Grass_Ground_Texture.png");

    shader.Load("resources/shaders/Shader.vert", "resources/shaders/Shader.frag");
    grassModel.Load("resources/models/Grass.obj");
    plane.Load();

    instancedShader.Load("resources/shaders/InstancedVert.vert", "resources/shaders/Shader.frag");

    int i = 0;
    for (float z = -0.5; z < 0.5; z += 0.01)
    {

        for (float x = -.5; x < .5; x += 0.01)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(x, 0.0099f, z));
            grassMats[i] = model;
            i++;
        }

    }

    // vertex buffer object
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 10201 * sizeof(glm::mat4), &grassMats[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < grassModel.meshes.size(); i++)
    {
        unsigned int VAO = grassModel.meshes[i].VAO;
        glBindVertexArray(VAO);
        // vertex attributes
        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

}


void Update() {

    Input::Update();

    //std::cout << "Cam Pos: " << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << "\n";
    //std::cout << "Cam Rot: " << camera.rotation.x << ", " << camera.rotation.y << ", " << camera.rotation.z << "\n";

    if (Input::KeyDown(GLFW_KEY_ESCAPE)) {
        GL::CloseWindow();
    }

    // Enter mouse look
    if (Input::KeyPressed(GLFW_KEY_E)) {
        Input::mouseX = GL::GetWindowWidth() / 2;  // This is stops mouse look jumping from large offset since the last frame
        Input::mouseY = GL::GetWindowHeight() / 2; // This is stops mouse look jumping from large offset since the last frame
        GL::SetMousePos(GL::GetWindowWidth() / 2, GL::GetWindowHeight() / 2);
        GL::DisableCursor();
    }

    // Leave mouse look
    if (!Input::KeyDown(GLFW_KEY_E)) {
        GL::EnableCursor();
    }

    // Mouselook
    if (Input::KeyDown(GLFW_KEY_E)) {
        float mouseSensitivity = 0.002f;
        camera.rotation.x += -Input::mouseOffsetY * mouseSensitivity;
        camera.rotation.y += -Input::mouseOffsetX * mouseSensitivity;
        camera.rotation.x = std::min(camera.rotation.x, 1.5f);
        camera.rotation.x = std::max(camera.rotation.x, -1.5f);
    }

    if (Input::KeyPressed(GLFW_KEY_0)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

void Render() {

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.4f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)GL::screen_width / (float)GL::screen_height, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    instancedShader.use();
    instancedShader.setMat4("projection", projection);
    instancedShader.setMat4("view", view);
    instancedShader.setFloat("time", (float)glfwGetTime());

    //// Grass
    //glm::mat4 grassModelMatrix = glm::mat4(1.0f);
    //grassModelMatrix = glm::translate(grassModelMatrix, glm::vec3(0.0f, 0.2f, 0.0f));
    //grassModelMatrix = glm::scale(grassModelMatrix, glm::vec3(1.050f, 1.050f, 1.050f));
    //shader.setMat4("model", grassModelMatrix);
    instancedShader.use();
    textures["Grass"].bind(0);
    //grassModel.Draw();

    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 rotate90 = glm::rotate(identity, glm::radians(90.0f), glm::vec3(0, 1, 0));

    instancedShader.setMat4("model", identity);
    for (unsigned int i = 0; i < grassModel.meshes.size(); i++)
    {
        glBindVertexArray(grassModel.meshes[i].VAO);
        glDrawElementsInstanced(
            GL_TRIANGLES, grassModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, 10201
        );
    }

    instancedShader.setMat4("model", rotate90);
    for (unsigned int i = 0; i < grassModel.meshes.size(); i++)
    {
        glBindVertexArray(grassModel.meshes[i].VAO);
        glDrawElementsInstanced(
            GL_TRIANGLES, grassModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, 10201
        );
    }

    // Grid
    shader.use();
    glm::mat4 gridModelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("model", gridModelMatrix);
    //textures["NumGrid"].bind(0);
    textures["Grass_Ground_Texture"].bind(0);
    plane.Draw();

    glfwSwapBuffers(GL::GetWindowPoiner());
    glfwPollEvents();
}

void CleanUp() {

    for (auto& texture : textures) {
        texture.second.cleanUp();
    }

    glfwTerminate();
}

int main() {

    Init();

    while (GL::WindowShouldNotClose()) {

        Update();
        Render();
    }

    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
};