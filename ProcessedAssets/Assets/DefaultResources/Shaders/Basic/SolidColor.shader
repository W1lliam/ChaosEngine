#shader vertex
#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangents;
layout (location = 3) in vec3 aBitangents;
layout (location = 4) in vec3 aUV;
layout (location = 5) in vec4 aColor;

layout(std140, binding = 0) uniform Matrices
{
   mat4 projection;  	// 64 bytes  // 0
   mat4 view;		      // 64 bytes  // 64
   vec3 viewPos;      // 16 bytes  // 128
};

out VS_INFO
{
  vec2 vertexUV;
  vec3 fragPos;
  vec3 vertexNormal;

} v_out;

uniform mat4 uModel;

void main()
{
  v_out.vertexUV        = aUV.xy;
  v_out.fragPos         = vec3(uModel * vec4(aPos, 1.0));

  v_out.vertexNormal     = (transpose(inverse(uModel)) * vec4(aNormal, 0.0)).xyz;

  gl_Position           = projection * view * uModel * vec4(aPos, 1.0);
}

#shader fragment
#version 440 core
#define GAMMA 1.1

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gMetallicRoughness;
layout (location = 4) out vec4 gEmissive;


in VS_INFO
{
  vec2 vertexUV;
  vec3 fragPos;
  vec3 vertexNormal;

} v_in;

uniform vec3  albedo;
uniform vec3  emissive;
uniform float metallic;
uniform float roughness;

void main()
{
    gPosition           = vec4(v_in.fragPos,1.0);
    gNormal             = vec4(normalize(v_in.vertexNormal),1.0);
    gAlbedo.rgb         = albedo;
    gAlbedo.a           = 1.0;
    gMetallicRoughness  = vec4(metallic, roughness, 0.0, 1.0);
    gEmissive           = vec4(emissive, 1.0);
}
