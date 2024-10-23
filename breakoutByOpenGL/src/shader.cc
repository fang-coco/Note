#include "shader.hh"
#include "glad/glad.h"
#include <cstddef>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ostream>

Shader::Shader() {}

Shader &Shader::Use() {
  glUseProgram(this->ID);
  return *this;
}

void Shader::Compile(const char *vertexSource, const char *fragmentSource,
                     const char *geometrySource) {

  unsigned int sVertex, sFragment, sGeometry;

  sVertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(sVertex, 1, &vertexSource, NULL);
  glCompileShader(sVertex);
  checkCompileErrors(sVertex, "vertex");

  sFragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(sFragment, 1, &fragmentSource, NULL);
  glCompileShader(sFragment);
  checkCompileErrors(sFragment, "fragment");

  if (geometrySource != nullptr) {
    sGeometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(sGeometry, 1, &geometrySource, NULL);
    glCompileShader(sGeometry);
    checkCompileErrors(sGeometry, "geometry");
  }

  this->ID = glCreateProgram();
  // std::cout << "debug::shaderProgramID: " << this->ID << std::endl;
  glAttachShader(this->ID, sVertex);
  glAttachShader(this->ID, sFragment);
  if (geometrySource != nullptr)
    glAttachShader(this->ID, sGeometry);

  glLinkProgram(this->ID);
  checkCompileErrors(this->ID, "program");

  glDeleteShader(sVertex);
  glDeleteShader(sFragment);
  if (geometrySource != nullptr)
    glDeleteShader(sGeometry);
}

void Shader::SetFloat(const char *name, float value, bool useShader) {
  if (useShader)
    this->Use();
  glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void Shader::SetInteger(const char *name, int value, bool useShader) {
  if (useShader)
    this->Use();
  glUniform1i(glGetUniformLocation(this->ID, name), value);
}

void Shader::SetVector2f(const char *name, float x, float y, bool useShader) {
  if (useShader)
    this->Use();
  glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}

void Shader::SetVector2f(const char *name, const glm::vec2 &value,
                         bool useShader) {
  this->SetVector2f(name, value.x, value.y, useShader);
}
void Shader::SetVector3f(const char *name, float x, float y, float z,
                         bool useShader) {
  if (useShader)
    this->Use();
  glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}

void Shader::SetVector3f(const char *name, const glm::vec3 &value,
                         bool useShader) {
  this->SetVector3f(name, value.x, value.y, value.z, useShader);
}

void Shader::SetVector4f(const char *name, float x, float y, float z, float w,
                         bool useShader) {
  if (useShader)
    this->Use();
  glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}

void Shader::SetVector4f(const char *name, const glm::vec4 &value,
                         bool useShader) {

  this->SetVector4f(name, value.x, value.y, value.z, value.w, useShader);
}

void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix,
                        bool useShader) {

  if (useShader)
    this->Use();
  glad_glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false,
                          glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(unsigned int object, std::string type) {
  int success;
  char inflog[512];
  if (type != "program") {
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    if (!success) {

      glGetShaderInfoLog(object, 512, NULL, inflog);
      std::cout << "error::shader: compile failed: " << type << "\t" << inflog
                << std::endl;
    }
  } else {
    glGetProgramiv(object, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(object, 512, NULL, inflog);
      std::cout << "error::shader: linke failed: " << type << "\t" << inflog
                << std::endl;
    }
  }
}
