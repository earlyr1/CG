#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;


uniform mat4 camera;

out vec2 TexCoord;

void main() {
  gl_Position = projection * view * model * vec4(vertex, 1.0f);
  TexCoord = tex;
}