#include "imgui.h"
#include <MacTypes.h>
#include <cstddef>
#include <cstdint>
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stb_image.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Simple helper function to load an image into a OpenGL texture with common
// settings
bool LoadTextureFromMemory(const void *data, size_t data_size,
                           GLuint *out_texture, int *out_width,
                           int *out_height) {
  // Load from file
  int image_width = 0;
  int image_height = 0;
  unsigned char *image_data =
      stbi_load_from_memory((const unsigned char *)data, (int)data_size,
                            &image_width, &image_height, NULL, 4);
  if (image_data == NULL)
    return false;

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Upload pixels into texture
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);

  *out_texture = image_texture;
  *out_width = image_width;
  *out_height = image_height;

  return true;
}

// Open and read a file, then forward to LoadTextureFromMemory()
bool LoadTextureFromFile(const char *file_name, GLuint *out_texture,
                         int *out_width, int *out_height) {
  FILE *f = fopen(file_name, "rb");
  if (f == NULL)
    return false;
  fseek(f, 0, SEEK_END);
  size_t file_size = (size_t)ftell(f);
  if (file_size == -1)
    return false;
  fseek(f, 0, SEEK_SET);
  void *file_data = IM_ALLOC(file_size);
  fread(file_data, 1, file_size, f);
  bool ret = LoadTextureFromMemory(file_data, file_size, out_texture, out_width,
                                   out_height);
  IM_FREE(file_data);
  return ret;
}
// bool loadTextureFromFile(const char *path, unsigned int *out_texture,
//                          unsigned int *out_width, unsigned int *out_height);

int main() {
  if (!glfwInit()) {
    std::cout << "ERROR::glfw init failed!" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "load imgui texture", NULL, NULL);

  if (window == nullptr) {
    std::cout << "ERROR::glfw window create failed!" << std::endl;
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync
  //
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "ERROR::glad load failed!" << std::endl;
    return -1;
  }

  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *window, int width, int height) {
                                   glViewport(0, 0, width, height);
                                 });
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  });

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  const char *glsl_version = "#version 150";
  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  auto r = ImGui_ImplOpenGL3_Init(glsl_version);
  if (!r) {
    std::cout << "ERROR::opengl3 init failed!" << std::endl;
    return -1;
  }

  const char *path = "../resources/textures/awesomeface.png";
  unsigned int my_image_texture = 0;
  int my_image_width = 0;
  int my_image_height = 0;

  auto ret = LoadTextureFromFile(path, &my_image_texture, &my_image_width,
                                 &my_image_height);
  std::cout << my_image_texture << "--" << my_image_width << "--"
            << my_image_height << std::endl;

  if (!ret) {
    std::cout << "ERROR::texture image load faied!" << std::endl;
    return -1;
  }

  std::cout << "debug1---" << std::endl;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  auto font1 = io.Fonts->AddFontFromFileTTF(
      "../resources/fonts/Roboto-Medium.ttf", 50.0f);
  auto font2 =
      io.Fonts->AddFontFromFileTTF("../resources/fonts/ProggyTiny.ttf", 20.0f);
  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      ImGui::Begin("OpenGL Texture Text");
      ImGui::Text("pointer = %x", my_image_texture);
      ImGui::Text("size = %d x %d", my_image_width, my_image_height);
      ImGui::Image((void *)(intptr_t)my_image_texture,
                   ImVec2(my_image_width, my_image_height));
      ImGui::PushFont(font1);
      ImGui::Text("Flip Y Coordinates");
      ImGui::PopFont();
      ImGui::Image((void *)(intptr_t)my_image_texture,
                   ImVec2(my_image_width, my_image_height), ImVec2(0.0f, 1.0f),
                   ImVec2(1.0f, 0.0f));
      ImGui::PushFont(font2);
      ImGui::Text("scale half size");
      ImGui::PopFont();
      ImGui::Image((void *)(intptr_t)my_image_texture,
                   ImVec2(my_image_width * 0.5, my_image_height * 0.5));

      auto displaypiexle = ImVec2(10, 10);
      auto displaysize = ImVec2(200, 300);
      ImGui::Text("custom pixels");
      ImGui::Image((void *)(intptr_t)my_image_texture, displaysize,
                   ImVec2(displaypiexle.x / my_image_width,
                          displaypiexle.y / my_image_height),
                   ImVec2((displaypiexle.x + displaysize.x) / my_image_width,
                          (displaypiexle.y + displaysize.y) / my_image_height));
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    // int display_w, display_h;
    // glfwGetFramebufferSize(window, &display_w, &display_h);
    // glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

// bool loadTextureFromFile(const char *path, unsigned int *out_texture,
//                          unsigned int *out_width, unsigned int *out_height) {
//   GLuint textureID;
//   glGenTextures(1, &textureID);
//
//   int width, height, channel;
//   unsigned char *data = stbi_load(path, &width, &height, &channel, 0);
//   if (data) {
//     glBindBuffer(GL_TEXTURE_2D, textureID);
//
//     GLenum format;
//     if (channel == 1)
//       format = GL_RED;
//     if (channel == 3)
//       format = GL_RGB;
//     if (channel == 4)
//       format = GL_RGBA;
//
//     glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
//                  GL_UNSIGNED_BYTE, data);
//
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//     stbi_image_free(data);
//   } else {
//     std::cout << "ERROR::Texture load failed!" << std::endl;
//     return false;
//   }
//   *out_texture = textureID;
//   *out_width = width;
//   *out_height = height;
//   return true;
// }
