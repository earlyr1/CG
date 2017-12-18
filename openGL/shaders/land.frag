#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;


uniform sampler2D ourTexture1;

void main()
{
  vec3 lightDir = vec3(1.0f, 1.0f, 0.0f); 

  float kd = max(dot(vNormal, lightDir), 0.0);
  color = texture(ourTexture1, vTexCoords) * (1.0f, 1.0f, 1.0f, 0.6f);

}