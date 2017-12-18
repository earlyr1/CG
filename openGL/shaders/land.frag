#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;
uniform int normal;

uniform sampler2D ourTexture1;

void main()
{ vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 lightDir = vec3(1.0f, 1.0f, 0.0f); 

  float kd = max(dot(vNormal, lightDir), 0.0);
  if (normal == 1) {
  	float sc = dot(vNormal, up);
  	color = vec4(0.0f, sc, 1 - sc, 1.0f);
  }
  else {
  	color = texture(ourTexture1, vTexCoords) * (1.0f, 1.0f, 1.0f, 0.6f);
  }

}