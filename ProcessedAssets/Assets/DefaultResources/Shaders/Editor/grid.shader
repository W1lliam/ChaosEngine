//////////////////////////////////////
// Author: William Perez            //
// 2019 - All rights reserved       //
// Do not use without authorization //
//////////////////////////////////////

#shader vertex
#version 420 core  // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier
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

out VS_INFO
{
  vec2 vertexUV;
  vec3 vertexNormal;
  vec3 fragPos;
} v_out;

/*uniform*/mat4 M;

out vec3 cameraPos;

void main()
{
    float lineSpacing   = 1.0;

    M = mat4(1.0);

    M[3][0] = viewPos.x;
    M[3][2] = viewPos.z;

    gl_Position         = projection * view *  M * vec4(aPos, 1.0);   // multiplication inversed because opengl transposed before sending the info, now is not transposed due to ubo (BEFORE P*V*M :: NOW M*V*P)
    v_out.fragPos       = vec3(M * vec4(aPos, 1.0));

    v_out.vertexUV      = v_out.fragPos.xz / lineSpacing;
    v_out.vertexNormal  = aNormal;

    cameraPos           = viewPos;
}

#shader fragment
#version 420 core // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier

uniform sampler2D uTexture;

layout (location = 5) out vec4 gUnlit;

in VS_INFO
{
  vec2 vertexUV;
  vec3 vertexNormal;
  vec3 fragPos;
} v_in;

in vec3 cameraPos;

float MAG(float p_lt, float p_ls, float p_lp)
{
  vec2 wrapped        = fract((v_in.vertexUV / p_lp) + 0.5) - 0.5;
  vec2 range          = abs(wrapped);
  vec2 speeds;
  speeds              = fwidth(v_in.vertexUV / p_ls);
  vec2 pirxelrange    = range/speeds;

  float result        = clamp(min(pirxelrange.x, pirxelrange.y) - p_lt, 0.0, 1.0);;
  return result;
}

void main()
{
  vec4 gridColor      = vec4(0.5,0.5,0.5,1.0);
  vec4 baseColor      = vec4(0.3,0.3,0.3,0.0);

  float cl            = MAG(0.01, 0.7, 1.0);
  float ml            = MAG(0.01, 2.0, 5.0);
  float fl            = MAG(0.1, 8.0, 20.0);

  float cmit         =  10.0;
  float cmet         =  20.0;
  float mfit         =  60.0;
  float mfet         =  80.0;

  float height        = distance(cameraPos.y, v_in.fragPos.y);
  float df            = clamp((height - cmit) / (cmet - cmit), 0.0, 1.0);
  float dff           = clamp((height - mfit) / (mfet - mfit), 0.0, 1.0);

  float inl           = mix(cl, ml, df);
  float fnl           = mix(inl, fl, dff);

  vec4 param          = mix(gridColor, baseColor, fnl);

  float oDist         = 5.0;
  float tDist         = 40.0;

  vec2  viewdirW      = cameraPos.xz - v_in.fragPos.xz;
  float viewdist      = length(viewdirW);
  float fff           = clamp((viewdist - oDist) / ((tDist + height) - oDist), 0.0, 1.0);
  param.a             *= (1.0 - fff);
  gUnlit              = param;
}

//////////////////////////////////////
// Author: William Perez            //
// 2019 - All rights reserved       //
// Do not use without authorization //
//////////////////////////////////////




























//////////////////////////////////////
// Author: William Perez            //
// 2019 - All rights reserved       //
// Do not use without authorization //
//////////////////////////////////////
