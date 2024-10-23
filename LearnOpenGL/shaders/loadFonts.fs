#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture_fonts;
uniform vec3 color;

void main() {
  vec4 sampleColor = vec4(1.0f, 1.0f, 1.0f, texture(texture_fonts, TexCoords).r);
  FragColor = vec4(color, 1.0f) * sampleColor;
}
