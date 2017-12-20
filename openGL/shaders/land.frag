#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;
in float H;

out vec4 color;
uniform int normal;

uniform sampler2D ourTexture1;

void main()
{ 
  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 lightDir = vec3(0.0f, 1.0f, 0.0f); 
  float updir = dot(vNormal, vec3(0.0, 1.0, 0.0));
  float kd = max(dot(vNormal, lightDir), 0.0);
  if (normal == 1) {
  	float sc = dot(vNormal, up);
  	color = vec4(sc, 0.0f, 1 - sc, 1.0f);
  }
  else {  
  	vec4 maincol;
    if (H < 18.75 && updir > 0.72) {
      maincol = mix(texture(ourTexture1, vTexCoords), vec4(0.992f, 0.874f, 0.466f, 1.0f), 0.5);
    }
  	else if (H < 19 && updir > 0.72) {
      maincol = mix(texture(ourTexture1, vTexCoords), vec4(0.992f, 0.874f, 0.466f, 1.0f), -2 * H + 38);
    }
    else {
      maincol = texture(ourTexture1, vTexCoords);
    }
    color = mix(maincol, vec4(0.2f + 0.8f * kd, 0.2f + 0.8f * kd, 0.2f + 0.8f * kd, 1.0f), 0.3);
  }
}

