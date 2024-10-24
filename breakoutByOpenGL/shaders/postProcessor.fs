#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform vec2 offsets[9];
uniform int edge_kernel[9];
uniform float blur_kernel[9];


uniform bool confuse;
uniform bool chaos;
uniform bool shake;

void main() {
  vec4 color = vec4(0.0f);
  vec3 sample[9];
  if (chaos || shake) {
    for (int i = 0; i < 9; ++i) {
      sample[i] = texture(scene, TexCoords.st + offsets[i]).rgb;
    }
  }

  if (chaos) {
    for (int i = 0; i < 9; ++i) {
      color += vec4(sample[i] * edge_kernel[i], 0.0f);
    }
    color.a = 1.0f;
  } else if (confuse) {
    color = vec4(1.0f - texture(scene, TexCoords).rgb, 1.0f);
  } else if (shake){
    for (int i = 0; i < 9; ++i) {
      color += vec4(sample[i] * blur_kernel[i], 0.0f);
    }
    color.a = 1.0f;
  } else {
    color = texture(scene, TexCoords);
  }
  FragColor = color;
}
