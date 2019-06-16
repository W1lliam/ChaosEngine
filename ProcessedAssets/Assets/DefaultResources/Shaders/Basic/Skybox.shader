#shader vertex
#version 440 core
layout (location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform Matrices
{
   mat4 projection;    // 64 bytes  // 0
   mat4 view;		   // 64 bytes  // 64
   vec3 viewPos;       // 16 bytes  // 128
};

out vec3 localPos;

void main()
{
    localPos = aPos;

    mat4 rotView = mat4(mat3(view)); // remove translation from the view matrix
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}

#shader fragment
#version 440 core

in vec3 localPos;
out vec4 fragColor;
  
uniform float       uExposure;
uniform float       uGamma;
uniform samplerCube uEnvironmentMap;
layout (location = 5) out vec4 gUnlit;
  
void main()
{
    vec3 envColor = texture(uEnvironmentMap, localPos).rgb;
    envColor = vec3(1.0) - exp(-envColor * uExposure);
    envColor = pow(envColor, vec3(1.0/uGamma)); 
  
    gUnlit = vec4(envColor, 1.0);
}