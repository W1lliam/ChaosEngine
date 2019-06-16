#shader vertex
#version 440 core

/////////////////////
// INPUT VARIABLES //
/////////////////////

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aUV;
layout (location = 3) in vec4 aColor;
layout (location = 4) in vec3 coRPosition;
layout (location = 5) in vec4 boneIndices;
layout (location = 6) in vec4 boneWeights;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
out VS_INFO
{
    vec2 vertexUV;
    vec3 vertexNormal;
    vec3 fragPos;
} v_out;

/////////////////////
// UNIFORM VARIABLES //
/////////////////////

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
  return v + 2.0 * cross(quaternion.xyz, cross(quaternion.xyz, v) + quaternion.w * v);
}

float DotProductQuaternions(vec4 firstQuat, vec4 secondQuat)
{
  return firstQuat.x * secondQuat.x + firstQuat.y * secondQuat.y + firstQuat.z * secondQuat.z + firstQuat.w * secondQuat.w;
}

void AntiPodality(out vec4 firstQuat, vec4 secondQuat)
{
    if(DotProductQuaternions(firstQuat, secondQuat) >= 0)
    {
        firstQuat += secondQuat;
    }
    else
    {
        firstQuat -= secondQuat;
    }
}

vec4 NormalizeQuaternion(vec4 ToNormalize)
{
    float norm = sqrt(DotProductQuaternions(ToNormalize, ToNormalize));
    return ToNormalize / norm;
}

vec4 InverseQuaternion(vec4 ToInverse)
{
    float norm = sqrt(DotProductQuaternions(ToInverse, ToInverse));
    return (ToInverse * vec4(-1,-1,-1, 1)) / norm;
}

vec4 Quaternionmultiply(vec4 q1, vec4 q2)
{
    vec4 qr;
    qr.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
    qr.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
    qr.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
    qr.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
    return qr;
}

vec3 GetTransformedPos(vec3 v, vec3 restCoR, vec3 posedCoR, vec4 offsetRotation)
{
    return posedCoR + rotate_vertex_position((v - restCoR), offsetRotation);
}

void CoRWeight(int boneIndex, float boneWeight, out vec4 qOut, out vec3 tOut, out vec3 nOut)
{
    if (boneWeight > 0)
    {
        tOut += boneWeight * GetTransformedPos(coRPosition, TPosPositions[boneIndex].xyz, AnimPositions[boneIndex].xyz, FinalRotations[boneIndex]);
        nOut += boneWeight * rotate_vertex_position(aNormal, FinalRotations[boneIndex]);
        AntiPodality(qOut, (boneWeight * FinalRotations[boneIndex]));
    }
}

void main(void)
{
    vec3 position;
    vec4 finalQuaternion;
    vec3 finalPosition;

    //Calculate CoRByWeight//
    CoRWeight(int(boneIndices.x), boneWeights.x, finalQuaternion, position, v_out.vertexNormal);
    CoRWeight(int(boneIndices.y), boneWeights.y, finalQuaternion, position, v_out.vertexNormal);
    CoRWeight(int(boneIndices.z), boneWeights.z, finalQuaternion, position, v_out.vertexNormal);
    CoRWeight(int(boneIndices.w), boneWeights.w, finalQuaternion, position, v_out.vertexNormal);

    //Normalize Quaternion Calculate by CoRWeight//
    finalQuaternion = NormalizeQuaternion(finalQuaternion);

    //Transform Vertice by CoR Calculation//
    finalPosition = GetTransformedPos(aPos, coRPosition, position, finalQuaternion);
    gl_Position = projection * view * vec4(finalPosition, 1.0);

    v_out.vertexUV = aUV.xy;
    v_out.fragPos = finalPosition;
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
