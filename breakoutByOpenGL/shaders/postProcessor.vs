#version 330 core
layout (location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform bool confuse;
uniform bool chaos;
uniform bool shake;
uniform float time;

void main() {
  gl_Position = vec4(vertex.xy, 0.0f, 1.0f);

  vec2 texture = vertex.zw;

  if (chaos) {
    float strength = 0.3f;
    TexCoords = texture + vec2(sin(time), cos(time)) * strength;
  } else if (confuse) {
    TexCoords = vec2(1.0f - texture);
  } else {
    TexCoords = texture;
  }

  if (shake) {
    float strength = 0.01;
    gl_Position =vec4(gl_Position.xy + vec2(cos(time * 10), cos(time * 15)) * strength, 0.0f, 1.0f);
  }
}
