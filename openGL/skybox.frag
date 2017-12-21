#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;
in float theta;
 
uniform float r;
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



vec3 perezZenith ( float t, float thetaSun )
{
  const float pi = 3.1415926;
  const vec4  cx1 = vec4 ( 0,       0.00209, -0.00375, 0.00165  );
  const vec4  cx2 = vec4 ( 0.00394, -0.03202, 0.06377, -0.02903 );
  const vec4  cx3 = vec4 ( 0.25886, 0.06052, -0.21196, 0.11693  );
  const vec4  cy1 = vec4 ( 0.0,     0.00317, -0.00610, 0.00275  );
  const vec4  cy2 = vec4 ( 0.00516, -0.04153, 0.08970, -0.04214 );
  const vec4  cy3 = vec4 ( 0.26688, 0.06670, -0.26756, 0.15346  );

  float t2    = t*t;
  float chi   = (4.0 / 9.0 - t / 120.0 ) * (pi - 2.0 * thetaSun );
  vec4  theta = vec4 ( 1, thetaSun, thetaSun*thetaSun, thetaSun*thetaSun*thetaSun );

  float Y = (4.0453 * t - 4.9710) * tan ( chi ) - 0.2155 * t + 2.4192;
  float x = t2 * dot ( cx1, theta ) + t * dot ( cx2, theta ) + dot ( cx3, theta );
  float y = t2 * dot ( cy1, theta ) + t * dot ( cy2, theta ) + dot ( cy3, theta );

  return vec3 ( Y, x, y );
}

//
// Perez allweather func (turbidity, cosTheta, cosGamma)
//

vec3  perezFunc ( float t, float cosTheta, float cosGamma )
{
    float  gamma      = acos ( cosGamma );
    float  cosGammaSq = cosGamma * cosGamma;
    float  aY =  0.17872 * t - 1.46303;
    float  bY = -0.35540 * t + 0.42749;
    float  cY = -0.02266 * t + 5.32505;
    float  dY =  0.12064 * t - 2.57705;
    float  eY = -0.06696 * t + 0.37027;
    float  ax = -0.01925 * t - 0.25922;
    float  bx = -0.06651 * t + 0.00081;
    float  cx = -0.00041 * t + 0.21247;
    float  dx = -0.06409 * t - 0.89887;
    float  ex = -0.00325 * t + 0.04517;
    float  ay = -0.01669 * t - 0.26078;
    float  by = -0.09495 * t + 0.00921;
    float  cy = -0.00792 * t + 0.21023;
    float  dy = -0.04405 * t - 1.65369;
    float  ey = -0.01092 * t + 0.05291;

    return vec3 ( (1.0 + aY * exp(bY/cosTheta)) * (1.0 + cY * exp(dY * gamma) + eY*cosGammaSq),
                  (1.0 + ax * exp(bx/cosTheta)) * (1.0 + cx * exp(dx * gamma) + ex*cosGammaSq),
                  (1.0 + ay * exp(by/cosTheta)) * (1.0 + cy * exp(dy * gamma) + ey*cosGammaSq) );
}

vec3  perezSky ( float t, float cosTheta, float cosGamma, float cosThetaSun )
{
    float thetaSun = acos        ( cosThetaSun );
    vec3  zenith   = perezZenith ( t, thetaSun );
    vec3  clrYxy   = zenith * perezFunc ( t, cosTheta, cosGamma ) / perezFunc ( t, 1.0, cosThetaSun );

    clrYxy [0] *= smoothstep ( 0.0, 0.1, cosThetaSun );     // make sure when thetaSun > PI/2 we have black color
  
    return clrYxy;
}

vec3  convertColor (vec3 clrYxy)
{
                                            // now rescale Y component
    clrYxy [0] = 1.0 - exp ( -clrYxy [0] / 25.0 );

    float ratio = clrYxy [0] / clrYxy [2];  // Y / y = X + Y + Z
    vec3  XYZ;

    XYZ.x = clrYxy [1] * ratio;             // X = x * ratio
    XYZ.y = clrYxy [0];                     // Y = Y
    XYZ.z = ratio - XYZ.x - XYZ.y;          // Z = ratio - X - Y

    const vec3 rCoeffs = vec3 ( 3.240479, -1.53715, -0.49853  );
    const vec3 gCoeffs = vec3 ( -0.969256, 1.875991, 0.041556 );
    const vec3 bCoeffs = vec3 ( 0.055684, -0.204043, 1.057311 );

    return vec3 ( dot ( rCoeffs, XYZ ), dot ( gCoeffs, XYZ ), dot ( bCoeffs, XYZ ) );
}

out vec4 color;
uniform sampler2D ourTexture2;

void main()
{
  
  vec3 lightDir = vec3(cos(theta), sin(theta), 0.0f); 
  float kd = max(dot(vNormal, lightDir), 0.0);
  vec3 col = vec3(0.459f, 0.733f, 0.99f);
  vec3 Sun = lightDir * r;
  
  float thetas = theta;
  float theta1 = acos(vFragPosition[1] / r);
  float gamma = acos(dot(vFragPosition, Sun) / (r * r));
  color = vec4(convertColor(perezSky(5, cos(theta1), cos(gamma), cos(thetas))), 1.0f);
  if (fog == 1) {
    color = mix(color, Fogcolor, fogFactor);
  }
}