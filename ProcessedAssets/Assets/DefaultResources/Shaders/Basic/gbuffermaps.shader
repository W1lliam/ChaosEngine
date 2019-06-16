#shader vertex
#version 440 core  // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
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
  mat3 vertexTBN;
} v_out;

uniform mat4 uModel;

void main()
{
  v_out.vertexUV      = aUV.xy;

  vec3 vertexNormal     = (transpose(inverse(uModel)) * vec4(aNormal, 0.0)).xyz;
  vec3 vertexTangent    = (transpose(inverse(uModel)) * vec4(aTangent, 0.0)).xyz;
  vec3 vertexBitangent  = (transpose(inverse(uModel)) * vec4(aBitangent, 0.0)).xyz;
  v_out.vertexTBN       = mat3(vertexTangent, vertexBitangent, vertexNormal);

  v_out.fragPos       = vec3(uModel * vec4(aPos, 1.0));

  gl_Position         = projection * view * uModel * vec4(aPos, 1.0);
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
  mat3 vertexTBN;
} v_in;

uniform sampler2D uTexture;
uniform sampler2D uNormal;
uniform sampler2D uMetallic;
uniform sampler2D uRoughness;
uniform sampler2D uEmissive;

// vec3 getNormalFromMap()
// {
//     vec3 tangentNormal = texture(uNormal, v_in.vertexUV).xyz * 2.0 - 1.0;

//     vec3 Q1  = dFdx(v_in.fragPos);
//     vec3 Q2  = dFdy(v_in.fragPos);
//     vec2 st1 = dFdx(v_in.vertexUV);
//     vec2 st2 = dFdy(v_in.vertexUV);

//     vec3 N   = normalize(v_in.vertexNormal);
//     vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
//     vec3 B  = -normalize(cross(N, T));
//     mat3 TBN = mat3(T, B, N);

//     return normalize(TBN * tangentNormal);
// }

void main()
{
    gPosition             = vec4(v_in.fragPos,1.0);
    // gNormal               = vec4(normalize(v_in.vertexNormal),1.0);
    // vec3 N                = vec3(v_in.vertexTBN[2][0], v_in.vertexTBN[2][1], v_in.vertexTBN[2][2]);
    vec3 normal           = texture(uNormal, v_in.vertexUV).rgb;
    normal                = normalize(normal * 2.0 - 1.0);
    normal                = normalize(v_in.vertexTBN * normal);
    gNormal               = vec4(normal, 1.0);

    gAlbedo               = texture(uTexture, v_in.vertexUV);
    gAlbedo.a             = 1.0;
    gMetallicRoughness.r  = texture(uMetallic, v_in.vertexUV).r;
    gMetallicRoughness.g  = texture(uRoughness, v_in.vertexUV).r;
    gMetallicRoughness.b  = 0.0;
    gMetallicRoughness.a  = 1.0;
    gEmissive             = texture(uEmissive, v_in.vertexUV);
    gEmissive.a           = 1.0;

}
