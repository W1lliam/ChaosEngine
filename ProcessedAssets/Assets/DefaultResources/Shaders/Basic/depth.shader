#shader vertex
#version 440 core  // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier
layout (location = 0) in vec3 aPos;

uniform mat4 uLightSpaceMatrix;
uniform mat4 uModel;

void main()
{
    gl_Position = uLightSpaceMatrix * uModel * vec4(aPos, 1.0);
}

#shader fragment
#version 440 core

out vec4 FragColor;

void main()
{
    // FragColor     = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);
    //FragColor     = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);

    // gl_FragDepth  = gl_FragCoord.z;
}
