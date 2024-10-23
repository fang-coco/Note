#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "game.hh"
#include "glad/glad.h"
#include <cstddef>
#include <iostream>

const unsigned int SCRWIDTH = 800;
const unsigned int SCRHEIGHT = 600;

float deltaTime;
float lastFrame;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

Game BreakOut(SCRWIDTH, SCRHEIGHT);

int main() {

  if (!glfwInit()) {
    std::cout << "error::glfw: init failed!" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow *window =
      glfwCreateWindow(SCRWIDTH, SCRHEIGHT, "BreakOut", NULL, NULL);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "error::glad: loader failed!" << std::endl;
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *window, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });

  BreakOut.Init();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    BreakOut.ProcessInput(deltaTime);
    BreakOut.Update(deltaTime);

    BreakOut.Render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  if (key > 0 && key < KEY_MAX_COUNT) {
    if (action == GLFW_PRESS) {
      BreakOut.Keys[key] = GLFW_TRUE;
    } else if (action == GLFW_RELEASE) {
      BreakOut.Keys[key] = GLFW_FALSE;
      BreakOut.KeysProcessed[key] = GLFW_FALSE;
    }
  }
}
