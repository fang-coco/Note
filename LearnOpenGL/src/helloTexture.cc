#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstddef>
#include <iostream>
#include <string.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <stb_image.h>

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPosition;\n"
                                 "layout (location = 1) in vec2 aTexCoords;\n"
                                 "out vec2 TexCoords;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   TexCoords = aTexCoords;\n"
                                 "   gl_Position = vec4(aPosition, 1.0);\n"
                                 "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoords;\n"
    "uniform vec4 color;\n"
    "uniform bool flag;\n"
    "uniform float mixness;\n"
    "uniform sampler2D texture_wood;\n"
    "uniform sampler2D texture_smile;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(texture_wood, TexCoords);\n"
    "   FragColor = mix(texture(texture_wood, TexCoords), "
    "texture(texture_smile, TexCoords), mixness);\n"
    "   if(flag)\n"
    "      FragColor = FragColor * color;\n"
    "}\0";

float vertices[] = {
    0.5f,  0.5f,  0.0f, 2.0f, 2.0f, // top right
    0.5f,  -0.5f, 0.0f, 2.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 0.0f, 2.0f  // top left
};
unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3, // first triangle
    1, 3, 2  // second triangle
};

const unsigned int scrWidth = 800;
const unsigned int scrHeight = 600;

unsigned int loadTexFromFile(const char *);
unsigned int genShaderProgram(const char *, const char *);
void processKeyInput(GLFWwindow *);
void initImgui(GLFWwindow *);

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

  initImgui(window);

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  unsigned int woodTexture = loadTexFromFile("../resources/textures/wood.png");
  unsigned int smileTexture =
      loadTexFromFile("../resources/textures/awesomeface.png");

  unsigned int shaderProgram =
      genShaderProgram(vertexShaderSource, fragmentShaderSource);

  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture_wood"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture_smile"), 1);

  static float mixness = 0.2f;
  static bool polygoneModeWithLine = false;
  static bool showColorWithPos = true;
  ImVec4 fChar_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  static int selected_option = 0;
  const char *options[] = {"GL_REPEAT", "GL_MIRRORED_REPEAT",
                           "GL_CLAMP_TO_EDGE", "GL_CLAMP_TO_BORDER"};

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    {
      ImGui::Begin("Config Setting");
      ImGui::Checkbox("show with lien", &polygoneModeWithLine);
      ImGui::SliderFloat("mixness", &mixness, 0.0f, 1.0f);
      ImGui::Checkbox("color with ", &showColorWithPos);
      ImGui::ColorEdit3("color", (float *)&fChar_color);
      for (int n = 0; n < IM_ARRAYSIZE(options); n++) {
        if (ImGui::RadioButton(options[n], &selected_option, n)) {
          // 选中时的操作（如果需要的话）
          glBindTexture(GL_TEXTURE_2D, woodTexture);
          switch (n) {
          case 0: {

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
          }
          case 1: {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                            GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                            GL_MIRRORED_REPEAT);
            break;
          }
          case 2: {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
          }
          case 3: {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                            GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                            GL_CLAMP_TO_BORDER);
            break;
          }
          default:
            break;
          }
          glBindTexture(GL_TEXTURE_2D, 0);
        }
        ImGui::SameLine(); // 将单选按钮放在同一行
      }

      ImGui::End();
    }
    glPolygonMode(GL_FRONT_AND_BACK, polygoneModeWithLine ? GL_LINE : GL_FILL);

    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "mixness"), mixness);
    glUniform1i(glGetUniformLocation(shaderProgram, "flag"),
                (int)showColorWithPos);
    glm::vec4 color(fChar_color.x, fChar_color.y, fChar_color.z, fChar_color.w);
    glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1,
                 (float *)&color[0]);

    float borderColor[] = {fChar_color.x, fChar_color.y, fChar_color.z,
                           fChar_color.w};
    glClearColor(0.2f, 0.3f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, smileTexture);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

void initImgui(GLFWwindow *window) {

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void processKeyInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

unsigned int genShaderProgram(const char *vertexShaderSource,
                              const char *fragmentShaderSource) {

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
  return shaderProgram;
}

unsigned int loadTexFromFile(const char *path) {
  unsigned int texture;
  glGenTextures(1, &texture);
  int width, height, channel;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path, &width, &height, &channel, 0);
  if (data) {
    glBindTexture(GL_TEXTURE_2D, texture);
    GLenum format;
    if (channel == 1)
      format = GL_RED;
    else if (channel == 3)
      format = GL_RGB;
    else if (channel == 4)
      format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {
    std::cout << "error::textre: data load failed!" << std::endl;
    stbi_image_free(data);
  }
  return texture;
}
