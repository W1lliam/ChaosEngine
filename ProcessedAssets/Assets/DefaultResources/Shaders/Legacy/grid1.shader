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
    M = mat4(1.0);

    M[3][0] = viewPos.x;
    M[3][2] = viewPos.z;

    gl_Position         = projection * view *  M * vec4(aPos, 1.0);   // multiplication inversed because opengl transposed before sending the info, now is not transposed due to ubo (BEFORE P*V*M :: NOW M*V*P)

    v_out.fragPos       = vec3(M * vec4(aPos, 1.0));

    v_out.vertexUV      =  aUV.xy - vec2(0.5);
    //v_out.vertexUV      =  v_out.fragPos.xz / 1.0;
    v_out.vertexNormal  = aNormal;

    cameraPos           = viewPos;
}

#shader fragment
#version 420 core // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier

uniform sampler2D uTexture;

out vec4 FragColor;

in VS_INFO
{
  vec2 vertexUV;
  vec3 vertexNormal;
  vec3 fragPos;
} v_in;

in vec3 cameraPos;

void main()
{
  float dist = distance(cameraPos.y, v_in.fragPos.y);

  float alpha1 = step(1.0 - step(fract(v_in.fragPos.x / 1.0) , 0.01), step(fract(v_in.fragPos.z / 1.0) , 0.01));

  alpha1 *= max(0.0, 0.5 - length(v_in.vertexUV));

  float alpha2 = step(1.0 - step(fract(v_in.fragPos.x / 5.0), 0.01), step(fract(v_in.fragPos.z / 5.0), 0.01));
  alpha2 *= max(0.0, 0.5 - length(v_in.vertexUV));

  float difference = clamp((dist - 3.0) / (8.0 - 3.0), 0.0, 1.0);
  float finalAlpha = mix(alpha1, alpha2, difference);

  //falloff
  float oDist         =  5.0;
  float tDist         =  40.0;

  vec2  viewdirW      = cameraPos.xz - v_in.fragPos.xz;
  float viewdist      = length(viewdirW);
  float falloff       = clamp((viewdist - oDist) / (tDist - oDist), 0.0, 1.0);

  finalAlpha *= (1.0 - falloff);

  FragColor = vec4(0.5,0.5,0.5,finalAlpha);
}
