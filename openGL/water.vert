#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

float A = 0.125f;
float w = 1.0f;

out vec2 vTexCoords;
out vec3 vFragPosition;
out vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{ 
  vec3 vert = vertex;
  vert[1] += A * sin(w * time / 3 - 2.1415f * vertex[0]);
  gl_Position = projection * view * model * vec4(vert, 1.0f);

  vTexCoords = texCoords;
  vFragPosition = vec3(model * vec4(vert, 1.0f));
  vNormal = mat3(transpose(inverse(model))) * normal;
}