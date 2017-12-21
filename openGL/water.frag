#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;
uniform sampler2D ourTexture2;

uniform int fog;

vec4 Fogcolor = vec4(1.0,1.0,1.0,1.0);
float density = 0.04;
float fogFactor;
float z;
float transparency = 0.7;

float zNear = 0.1; 
float zFar  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));  
}


void main()
{
  z = gl_FragCoord.z / gl_FragCoord.w;
  fogFactor = exp(-pow(1.0 / (density * z), 2.0));

  vec3 lightDir = vec3(1.0f, 1.0f, 0.0f); 

  vec3 col = vec3(0.1, 0.1, 0.1);

  float kd = max(dot(vNormal, lightDir), 0.0);

  color = mix(texture(ourTexture2, vTexCoords), vec4(7 * col, 0.5f), transparency);
  if (fog == 1) 
  {
  	color = mix(color, Fogcolor, fogFactor);
  }
  
}