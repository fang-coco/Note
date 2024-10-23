#include <cstddef>
#include <freetype2/ft2build.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include FT_FREETYPE_H
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <shader.hh>
#include <vector>

struct Character {
  GLuint TextureID;
  glm::ivec2 Size;
  glm::ivec2 Bearing;
  GLint64 Advance;
};
int main() {
  if (!glfwInit()) {
    std::cout << "error::glfw init failed!" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(800, 800, "LearnOpenGl", NULL, NULL);
  if (window == nullptr) {
    std::cout << "error::glfw window create failed!" << std::endl;
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *window, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });

  glfwSetErrorCallback([](int error, const char *description) {
    std::cout << "error::glfw " << description << std::endl;
  });

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "error::glad load failed!" << std::endl;
    glfwTerminate();
    return -1;
  }

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  std::unordered_map<GLchar, Character> characters;
  auto RenderText = [&](Shader &s, std::string text, GLfloat x, GLfloat y,
                        GLfloat scale, glm::vec3 color) {
    // 激活对应的渲染状态
    s.Use();
    s.setVec3("color", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // 遍历文本中所有的字符
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
      Character ch = characters[*c];

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
      // 绘制四边形
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // 更新位置到下一个字形的原点，注意单位是1/64像素
      // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
      x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
  };

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cout << "error::freetype2 library init failed!" << std::endl;
    return -1;
  }

  static int selected_fish = 0;
  std::string fontsDir = "../resources/fonts/";
  std::vector<std::string> fontfiles{
      "Cousine-Regular.ttf",
      "DroidSans.ttf",
  };
  Shader shader("../shaders/loadFonts.vs", "../shaders/loadFonts.fs");

  glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 800.0f);
  shader.Use();
  shader.setMat4("projection", projection);
  shader.setInt("texture_fonts", 0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }
    glClearColor(0.2f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      ImGui::Begin("Param Setting");

      // Simple selection popup (if you want to show the current selection
      // inside the Button itself, you may want to build a string using the
      // "###" operator to preserve a constant ID with a variable label)
      if (ImGui::Button("fontType.."))
        ImGui::OpenPopup("my_select_popup");
      ImGui::SameLine();
      ImGui::TextUnformatted(fontfiles[selected_fish].c_str());
      if (ImGui::BeginPopup("my_select_popup")) {
        ImGui::SeparatorText("Aquarium");
        for (int i = 0; i < fontfiles.size(); i++)
          if (ImGui::Selectable(fontfiles[i].c_str()))
            selected_fish = i;
        ImGui::EndPopup();
      }
      ImGui::End();
    }

    FT_Face face;
    std::string fontpath = (fontsDir + fontfiles[selected_fish]);
    if (FT_New_Face(ft, fontpath.c_str(), 0, &face)) {
      std::cout << "error::face from fontpath: " << fontpath << " new failed!"
                << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 49);
    characters.clear();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (GLubyte i = 0; i < 128; ++i) {
      if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
        std::cout << "error::freetype: failed to load glphy" << std::endl;
        continue;
      }
      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                   face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                   face->glyph->bitmap.buffer);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      Character character{
          texture,
          glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
          glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
          face->glyph->advance.x};
      characters.insert(std::make_pair(i, character));
    }
    // FT_Done_Face(face);

    // RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f,
    //            glm::vec3(0.5, 0.8f, 0.2f));
    RenderText(shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f,
               glm::vec3(0.3, 0.7f, 0.9f));

    // Rendering
    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  FT_Done_FreeType(ft);
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}
