#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <charconv>
#include <cstddef>
#include <iostream>
#include <string.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui.h>

const unsigned int scrWidth = 800;
const unsigned int scrHeight = 600;

void processKeyInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}
int main() {

  if (!glfwInit()) {
    std::cout << "error::glfw: init failed!" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(scrWidth, scrHeight, "helloWindow", NULL, NULL);
  if (window == nullptr) {
    std::cout << "error::glfw: window create failed!" << std::endl;
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "error::glad: loader failed!" << std::endl;
    return -1;
  }

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *window, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");

  float vertices[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f  // top left
  };
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 3, 2  // second triangle
  };
  /*
   *          1(-0.5, 0.75)     2(-0.25, 0.75)                  9(0.5, 0.75)
   *
   *                            3(-0.25, 0.5)                   10(0.5, 0.5)
   *
   *                            4(-0.25, 0.25)     8(0.25, 0.25)
   *
   *                            5(-0.25, 0.0)      7(0.25, 0.0)
   *
   *
   *
   *          0(-0.5, -0.75)    6(-0.25, -0.75)
   * */
  float fChar_vertices[] = {
      -0.5f,  -0.75f, 0.0f, -0.5f,  0.75f, 0.0f, -0.25f, 0.75f, 0.0f,
      -0.25f, 0.5f,   0.0f, -0.25f, 0.25f, 0.0f, -0.25f, 0.0f,  0.0f,
      -0.25f, -0.75f, 0.0f, 0.25f,  0.0f,  0.0f, 0.25f,  0.25f, 0.0f,
      0.5f,   0.75f,  0.0f, 0.5f,   0.5f,  0.0f,
  };
  unsigned int fChar_indices[] = {0, 1, 2, 0, 2, 6, 5, 4,  8,
                                  5, 7, 8, 3, 2, 9, 3, 10, 9};

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(fChar_vertices), fChar_vertices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fChar_indices), fChar_indices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  const char *vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "out vec3 position;\n"
      "void main()\n"
      "{\n"
      "   position = aPos;\n"
      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";

  const char *fragmentShaderSource = "#version 330 core\n"
                                     "out vec4 FragColor;\n"
                                     "in vec3 position;\n"
                                     "uniform vec4 color;\n"
                                     "uniform bool showWithPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   if (showWithPos)\n"
                                     "      FragColor = vec4(position, 1.0f);\n"
                                     "   else\n"
                                     "      FragColor = color;\n"
                                     "}\0";

  int success;
  char inflog[512];

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, inflog);
    std::cout << "error::shader: vertex shader compile failed: " << inflog
              << std::endl;
    return -1;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, inflog);
    std::cout << "error::shader: fragment shader compile failed: " << inflog
              << std::endl;
    return -1;
  }

  GLuint shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, inflog);
    std::cout << "error::shader: program link failed: " << inflog << std::endl;
    return -1;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  static bool polygoneModeWithLine = false;
  static bool showColorWithPos = true;
  ImVec4 fChar_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    {
      ImGui::Begin("Config Setting");
      ImGui::Checkbox("show with lien", &polygoneModeWithLine);
      ImGui::Checkbox("color with pos", &showColorWithPos);
      ImGui::ColorEdit3("color", (float *)&fChar_color);
      ImGui::End();
    }
    glPolygonMode(GL_FRONT_AND_BACK, polygoneModeWithLine ? GL_LINE : GL_FILL);
    glUniform1i(glGetUniformLocation(shaderProgram, "showWithPos"),
                (int)showColorWithPos);
    glm::vec4 color(fChar_color.x, fChar_color.y, fChar_color.z, fChar_color.w);
    glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1,
                 (float *)&color[0]);
    glClearColor(0.2f, 0.3f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }
    processKeyInput(window);
  }
  glfwTerminate();
}
