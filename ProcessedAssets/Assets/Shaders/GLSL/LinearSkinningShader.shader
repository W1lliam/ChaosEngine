#shader vertex
#version 440 core
/////////////////////
// INPUT VARIABLES //
/////////////////////
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangents;
layout (location = 3) in vec3 aBitangents;
layout (location = 4) in vec3 aUV;
layout (location = 5) in vec4 aColor;
layout (location = 6) in vec4 boneIndices;
layout (location = 7) in vec4 boneWeights;
layout (location = 8) in vec3 CoRWeight;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
out VS_INFO
{
    vec2 vertexUV;
    vec3 vertexNormal;
    vec3 fragPos;
} v_out;

///////////////////////
// UNIFORM VARIABLES //
///////////////////////
layout(std140, binding = 0) uniform Matrices
{
   mat4 projection;  	// 64 bytes  // 0
   mat4 view;		      // 64 bytes  // 64
   vec3 viewPos;      // 16 bytes  // 128
};

uniform mat4 uModel;

uniform vec4 TPosPositions[64];
uniform vec4 AnimPositions[64];
uniform vec4 FinalRotations[64];

///////////////
//  METHODS //
//////////////

vec3 rotate_vertex_position(vec3 position, vec4 quaternion)
{
  vec3 v = position.xyz;
  return (v + 2.0 * cross(quaternion.xyz, (cross(quaternion.xyz, v) + quaternion.w * v)));
}

vec3 GetTransformedPos(vec3 v, vec3 restCoR, vec3 posedCoR, vec4 offsetRotation)
{
    return posedCoR + rotate_vertex_position((v - restCoR), offsetRotation);
}

void main()
{
    vec3 finalPosition;
    finalPosition = boneWeights.x * GetTransformedPos(aPos, TPosPositions[int(boneIndices.x)].xyz, AnimPositions[int(boneIndices.x)].xyz, FinalRotations[int(boneIndices.x)]);
    finalPosition += boneWeights.y * GetTransformedPos(aPos, TPosPositions[int(boneIndices.y)].xyz, AnimPositions[int(boneIndices.y)].xyz, FinalRotations[int(boneIndices.y)]);
    finalPosition += boneWeights.z * GetTransformedPos(aPos, TPosPositions[int(boneIndices.z)].xyz, AnimPositions[int(boneIndices.z)].xyz, FinalRotations[int(boneIndices.z)]);
    finalPosition += boneWeights.w * GetTransformedPos(aPos, TPosPositions[int(boneIndices.w)].xyz, AnimPositions[int(boneIndices.w)].xyz, FinalRotations[int(boneIndices.w)]);
    gl_Position = projection * view * uModel * vec4(finalPosition, 1.0);

    v_out.vertexUV = aUV.xy;
    v_out.fragPos = finalPosition;
    v_out.vertexNormal = boneWeights.x * rotate_vertex_position(aNormal, FinalRotations[int(boneIndices.x)]);
    v_out.vertexNormal += boneWeights.y * rotate_vertex_position(aNormal, FinalRotations[int(boneIndices.y)]);
    v_out.vertexNormal += boneWeights.z * rotate_vertex_position(aNormal, FinalRotations[int(boneIndices.z)]);
    v_out.vertexNormal += boneWeights.w * rotate_vertex_position(aNormal, FinalRotations[int(boneIndices.w)]);
    v_out.vertexNormal = (view * vec4(v_out.vertexNormal, 0.0)).xyz;
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
  vec3 vertexNormal;
  vec3 fragPos;
} v_in;

uniform sampler2D uTexture;
uniform vec3 emissive;
uniform float metallic;
uniform float roughness;

void main()
{
    gPosition           = vec4(v_in.fragPos,1.0);
    gNormal             = vec4(normalize(v_in.vertexNormal),1.0);
    gAlbedo.rgb         = texture(uTexture, v_in.vertexUV).rgb;
    gAlbedo.a           = 1.0;//texture(specular, v_in.vertexUV).r;
    gMetallicRoughness  = vec4(0.1,0.8,0.0,0.0);//texture(specular, v_in.vertexUV).r;
    gEmissive           = vec4(emissive, 1.0);
}
