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

layout(std140, binding = 0) uniform Matrices
{
   mat4 projection;  	// 64 bytes  // 0
   mat4 view;		      // 64 bytes  // 64
   vec3 viewPos;      // 16 bytes  // 128
};

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
uniform mat4 uModel;

uniform vec4 FinalPositions[64];
uniform vec4 FinalRotations[64];
///////////////
//  METHODS //
//////////////

mat2x4 GetDualQuatFromTransform(vec3 FinalPos, vec4 Rot)
{
    mat2x4 dq;
    vec4 secondline;
    dq[0] = Rot;

    secondline.w = -0.5f* dot(FinalPos, Rot.xyz);
    secondline.x =  0.5f* ( FinalPos.x * Rot.w + FinalPos.y * Rot.z - FinalPos.z * Rot.y);
    secondline.y =  0.5f* (-FinalPos.x * Rot.z + FinalPos.y * Rot.w + FinalPos.z * Rot.x);
    secondline.z =  0.5f* ( FinalPos.x * Rot.y - FinalPos.y * Rot.x + FinalPos.z * Rot.w);
    dq[1] = secondline;
    return dq;
}

void main()
{
  mat2x4 dq0 = GetDualQuatFromTransform(FinalPositions[int(boneIndices.x)].xyz, FinalRotations[int(boneIndices.x)]);
  mat2x4 dq1 = GetDualQuatFromTransform(FinalPositions[int(boneIndices.y)].xyz, FinalRotations[int(boneIndices.y)]);
  mat2x4 dq2 = GetDualQuatFromTransform(FinalPositions[int(boneIndices.z)].xyz, FinalRotations[int(boneIndices.z)]);
  mat2x4 dq3 = GetDualQuatFromTransform(FinalPositions[int(boneIndices.w)].xyz, FinalRotations[int(boneIndices.w)]);

  mat2x4 blendedDQ = dq0 * boneWeights.x;
  blendedDQ += dq1 * boneWeights.y;
  blendedDQ += dq2 * boneWeights.z;
  blendedDQ += dq3 * boneWeights.w;


  float len = length(blendedDQ[0]);
  blendedDQ /= len;
  //vec3 position = aPos + 2.0*cross(blendedDQ[0].yzw, cross(blendedDQ[0].yzw, aPos) + blendedDQ[0].x * aPos);
  //vec3 trans = 2.0*(blendedDQ[0].x * blendedDQ[1].yzw - blendedDQ[1].x * blendedDQ[0].yzw + cross(blendedDQ[0].yzw, blendedDQ[1].yzw));
  //position += trans;

  vec3 position = aPos.xyz + 2.0*cross(blendedDQ[0].xyz, cross(blendedDQ[0].xyz, aPos.xyz) + blendedDQ[0].w * aPos.xyz);
  vec3 trans = 2.0*(blendedDQ[0].w * blendedDQ[1].xyz - blendedDQ[1].w * blendedDQ[0].xyz + cross(blendedDQ[0].xyz, blendedDQ[1].xyz));
  position += trans;

  gl_Position = projection * view * uModel * vec4(position, 1.0f);

  v_out.vertexUV = aUV.xy;
  v_out.fragPos = position;
  v_out.vertexNormal = aNormal + 2.0* cross(blendedDQ[0].yzw, (cross(blendedDQ[0].yzw, aNormal) + blendedDQ[0].x* aNormal));
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
//uniform sampler2D specular;
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
