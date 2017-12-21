#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;
uniform sampler2D ourTexture2;

void main()
{
  vec3 lightDir = vec3(1.0f, 1.0f, 0.0f); 

  vec3 col = vec3(0.1, 0.1, 0.1);

  float kd = max(dot(vNormal, lightDir), 0.0);

  color = mix(texture(ourTexture2, vTexCoords), vec4(7 * col, 0.5f), 0.75);
  
}