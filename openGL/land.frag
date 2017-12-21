#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;
in float H;

out vec4 color;
uniform int normal;
uniform float mid;
uniform sampler2D ourTexture1;
uniform int fog;


vec4 Fogcolor = vec4(1.0,1.0,1.0,1.0);
float density = 0.04;
float fogFactor;
float z;

float zNear = 0.1; 
float zFar  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));  
}


void main()
{ 
  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 lightDir = normalize(vec3(0.8f, 0.2f, 0.0f)); 
  float updir = dot(vNormal, vec3(0.0, 1.0, 0.0));
  float kd = max(dot(vNormal, lightDir), 0.0);
  z = gl_FragCoord.z / gl_FragCoord.w;
  fogFactor = exp(-pow(1.0 / (density * z), 2.0));
  if (normal == 1) {
    float sc = dot(vNormal, up);
    color = vec4(sc, 0.0f, 1 - sc, 1.0f);
  }
  else {  
    vec4 maincol;
    if (H + sqrt(mid)< 19.75 && updir > 0.65) {
      maincol = mix(texture(ourTexture1, vTexCoords), vec4(0.992f, 0.874f, 0.466f, 1.0f), -0.65 * updir + 1.65);
    }
    else if (H + sqrt(mid) < 20.25 && updir > 0.65) {
      maincol = mix(texture(ourTexture1, vTexCoords), vec4(0.992f, 0.874f, 0.466f, 1.0f), (-2 * H - 2 * sqrt(mid) + 41) * (-0.65 * updir + 1.65));
    }
    else {
      maincol = texture(ourTexture1, vTexCoords);
    }
    color = mix(maincol, vec4(0.2f + 0.8f * kd, 0.2f + 0.8f * kd, 0.2f + 0.8f * kd, 1.0f), 0.3);
    if (fog == 1) 
    {
      color = mix(color, Fogcolor, fogFactor);
    }
  }
}

