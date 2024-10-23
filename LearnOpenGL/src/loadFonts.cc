#include <iostream>
#include <ostream>
#include <string.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <camera.hh>
#include <fonts.hh>
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

  Shader shader("../shaders/loadFonts.vs", "../shaders/loadFonts.fs");

  shader.Use();
  shader.setInt("texture_fonts", 0);

  Fonts font = Fonts("../resources/fonts/ProggyClean.ttf", glm::ivec2(0, 48));
  font.loadCharsByStr("hello OpenGl!");

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  auto RenderText = [&](Fonts &font, Shader &s, std::string text, GLfloat x,
                        GLfloat y, GLfloat scale, glm::vec3 color) {
    // 激活对应的渲染状态
    s.Use();
    s.setVec3("color", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // 遍历文本中所有的字符
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
      Character ch = font.getChar(*c);

      GLfloat xpos = x + ch.Bearing.x * scale;
      GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

      GLfloat w = ch.Size.x * scale;
      GLfloat h = ch.Size.y * scale;
      // 对每个字符更新VBO
      GLfloat vertices[6][4] = {
          {xpos, ypos + h, 0.0, 0.0},    {xpos, ypos, 0.0, 1.0},
          {xpos + w, ypos, 1.0, 1.0},

          {xpos, ypos + h, 0.0, 0.0},    {xpos + w, ypos, 1.0, 1.0},
          {xpos + w, ypos + h, 1.0, 0.0}};
      // 在四边形上绘制字形纹理
      glBindTexture(GL_TEXTURE_2D, ch.TextureID);
      // 更新VBO内存的内容
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // 绘制四边形
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // 更新位置到下一个字形的原点，注意单位是1/64像素
      x += (ch.Advance >> 6) *
           scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
  };

  glm::mat4 projection = glm::ortho(0.0f, 600.0f, 0.0f, 600.0f);
  shader.Use();
  shader.setMat4("projection", projection);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  while (!glfwWindowShouldClose(window)) {
    auto currentFrame = static_cast<float>(glfwGetTime());
    detalTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderText(font, shader, "hello OpenGl!", 25.0f, 25.0f, 1.0f,
               glm::vec3(0.5, 0.8f, 0.2f));
    // RenderText(font, shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f,
    //            glm::vec3(0.3, 0.7f, 0.9f));
    glfwSwapBuffers(window);
    glfwPollEvents();
    moveCamera();
  }

  glfwTerminate();
}
