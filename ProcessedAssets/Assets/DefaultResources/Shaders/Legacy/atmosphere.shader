#shader vertex
#version 440 core  // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aUV;
layout (location = 3) in vec4 aColor;

layout(std140, binding = 0) uniform Matrices
{
   mat4 projection;  	// 64 bytes  // 0
   mat4 view;		      // 64 bytes  // 64
   vec3 viewPos;      // 16 bytes  // 128
};

uniform mat4 uModel;

out vec3 TexCoords;
void main()
{
    TexCoords       = aPos.xyz;
    gl_Position     = projection * view * uModel * vec4(aPos, 1.0);

    //gl_Position=gl_Position.xyww;
}

#shader fragment
#version 440 core
#define GAMMA 1.1
#define MAX_LIGHTS 10

struct Light
{
  vec4   lightPosition;            // 16 bytes    // 0         // Position:  [x,y,z, LightType]
  vec4   lightColor;               // 16 bytes    // 16        // Color:     [r,g,b, Intensity]
  vec4   lightDirection;           // 16 bytes    // 32
  vec4   lightLQAF;                // 16 bytes    // 48        // LQAF:      [Linear, Quadratic, Angle, Falloff]
};

layout(std140, binding = 1) uniform LightData
{
   Light  allLights[MAX_LIGHTS];
   uint   lightCount;
};

in vec3 TexCoords;
out vec4 FragColor;

void main()
{
  vec3 v = normalize(TexCoords);

  vec3 sunDirection = allLights[0].lightDirection.xyz;
  vec3 direction = vec3(0.0, 1.0, 0.0);
  direction = normalize(sunDirection);

  float g= dot(v, sunDirection);
  float s = direction.y;

  vec3 skyTint = vec3(0.078, 0.796, 1.0);

  float a = (0.526 + 5.0 * exp(-1.5 * acos(g))) * (1 - exp(-0.8 / max(g, 0)));
  float b = (0.526 + 5.0 * exp(-1.5 * acos(s))) * (1 - exp(-0.8));

  FragColor = vec4(skyTint * a / b, 1.0);
}
