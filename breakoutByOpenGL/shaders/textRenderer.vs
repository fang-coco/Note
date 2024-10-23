#version 330 core
layout (location = 0) in vec4 vertexs;

out vec2 TexCoords;

uniform mat4 projection;

void main() {
  TexCoords = vertexs.zw;
  gl_Position = projection * vec4(vertexs.xy, 0.0f, 1.0f);
}
