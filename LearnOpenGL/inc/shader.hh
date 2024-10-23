#pragma once

#include <GLFW/glfw3.h>
#include <cstddef>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
  unsigned int ID;
  Shader(const char *vertexPath, const char *fragmentPath,
         const char *geometryPath = nullptr) {
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    vShaderFile.exceptions(std::fstream::failbit | std::fstream::badbit);
    fShaderFile.exceptions(std::fstream::failbit | std::fstream::badbit);
    gShaderFile.exceptions(std::fstream::failbit | std::fstream::badbit);
    try {
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);

      std::stringstream vShaderStream, fShaderStream;
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();

      vShaderFile.close();
      fShaderFile.close();

      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();

      if (geometryPath != nullptr) {
        gShaderFile.open(geometryPath);
        std::stringstream gShaderStream;
        gShaderStream << gShaderFile.rdbuf();
        gShaderFile.close();
        geometryCode = gShaderStream.str();
      }
    } catch (std::ifstream::failure &e) {
      std::cout << "ERROR SHADER FILE NOT SUCCESSFULLY READ: " << e.what()
                << std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    unsigned int geometry;
    if (geometryPath != nullptr) {
      const char *gShaderCode = geometryCode.c_str();
      geometry = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(geometry, 1, &gShaderCode, NULL);
      glCompileShader(geometry);
      checkCompileErrors(geometry, "GEOMETRY");
    }

    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    if (geometryPath != nullptr) {
      glAttachShader(this->ID, geometry);
    }
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr) {
      glDeleteShader(geometry);
    }
  }

  void Use() { glUseProgram(this->ID); }

  void setBool(const std::string &name, bool val) {
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)val);
  }

  void setInt(const std::string &name, int val) {
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), val);
  }

  void setFloat(const std::string &name, float val) {
    glUniform1f(glGetUniformLocation(this->ID, name.c_str()), val);
  }

  void setVec2(const std::string &name, const glm::vec2 &val) {
    glUniform2fv(glGetUniformLocation(this->ID, name.c_str()), 1, &val[0]);
  }

  void setVec3(const std::string &name, const glm::vec3 &val) {
    glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, &val[0]);
  }

  void setVec4(const std::string &name, const glm::vec4 &val) {
    glUniform4fv(glGetUniformLocation(this->ID, name.c_str()), 1, &val[0]);
  }

  void setMat3(const std::string &name, const glm::mat3 &val) {
    glUniformMatrix3fv(glGetUniformLocation(this->ID, name.c_str()), 1,
                       GL_FALSE, &val[0][0]);
  }

  void setMat4(const std::string &name, const glm::mat4 &val) {
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1,
                       GL_FALSE, &val[0][0]);
  }

private:
  void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                  << infoLog << std::endl;
      }
    } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINK_ERROR of type: " << type << "\n"
                  << infoLog << std::endl;
      }
    }
  }
};
