#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;
in float theta;
 
uniform float r;

float t = 0.1;

float  A =  0.17872 * t - 1.46303;
float  B = -0.35540 * t + 0.42749;
float  C = -0.02266 * t + 5.32505;
float  D =  0.12064 * t - 0.57705;
float  E = -0.06696 * t + 0.37027;

float F(float theta, float gamma)
{
  return (1 + A * exp(B / cos(theta))) * (1 + C * exp(D * gamma) + E * cos(gamma) * cos(gamma));	
}

out vec4 color;
uniform sampler2D ourTexture2;

void main()
{
  float thetas = theta;
  float theta1 = theta;
  vec3 lightDir = vec3(cos(theta), sin(theta), 0.0f); 
  vec3 Sun = lightDir * r;
  vec3 col = vec3(0.459f, 0.733f, 0.99f) * 0.3;
  theta1 = asin(dot(vFragPosition, Sun) / (r * r));
  float kd = max(dot(vNormal, lightDir), 0.0);
  float gamma = atan(sqrt(vFragPosition[0] * vFragPosition[0] + vFragPosition[2] * vFragPosition[2]), vFragPosition[1]);
  float Y = F(theta1, gamma) / F(0, thetas);
  color = vec4(Y * col, 1.0f);
  
}