#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D Image;
uniform vec3 spriteColor;

void main() {
  FragColor = vec4(spriteColor, 1.0f) * texture(Image, TexCoords);
}
