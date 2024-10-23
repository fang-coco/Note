#include <iostream>
#include <ostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <camera.hh>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <model.hh>
#include <shader.hh>

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

float lastFrame = 0.0f;
float detalTime = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[GLFW_KEY_LAST];

float lastX = 0.0f;
float lastY = 0.0f;
bool firstMouse = true;

void moveCamera() {
  if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(Camera_Movement::FORWARD, detalTime);
  if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(Camera_Movement::BACKWARD, detalTime);
  if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(Camera_Movement::LEFT, detalTime);
  if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(Camera_Movement::RIGHT, detalTime);
}

int main() {

  if (!glfwInit()) {
    std::cout << "ERROR::GLFW: init failed!" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

  if (window == nullptr) {
    std::cout << "ERROR::GLFW: window creat failed!" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwSetErrorCallback([](int error, const char *description) {
    std::cout << "ERROR:: " << description << std::endl;
  });

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *window, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });

  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (action == GLFW_PRESS) {
      keys[key] = true;
    } else if (action == GLFW_RELEASE) {
      keys[key] = false;
    }
  });

  glfwSetCursorPosCallback(window,
                           [](GLFWwindow *window, double xpos, double ypos) {
                             auto xoffset = static_cast<float>(xpos);
                             auto yoffset = static_cast<float>(ypos);
                             if (firstMouse) {
                               lastX = xoffset;
                               lastY = yoffset;
                               firstMouse = false;
                             }

                             xoffset = xoffset - lastX;
                             yoffset = lastY - yoffset;
                             camera.ProcessMouseMovement(xoffset, yoffset);
                           });

  glfwSetScrollCallback(
      window, [](GLFWwindow *window, double xoffset, double yoffset) {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
      });

  Model nanosuitModel("../resources/objects/nanosuit/nanosuit.obj");
  Shader shader("../shaders/loadModel.vs", "../shaders/loadModel.fs");

  shader.Use();
  shader.setInt("texture_diffuse1", 0);

  glEnable(GL_DEPTH_TEST);
  while (!glfwWindowShouldClose(window)) {

    auto currentFrame = static_cast<float>(glfwGetTime());
    detalTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    shader.Use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    nanosuitModel.Draw(shader);

    glfwSwapBuffers(window);
    glfwPollEvents();
    moveCamera();
  }

  glfwTerminate();
}
