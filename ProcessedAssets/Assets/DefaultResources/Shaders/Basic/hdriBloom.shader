#shader vertex
#version 440 core  // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier
layout (location = 0) in vec3 aPos;
layout (location = 4) in vec3 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoords   = aTexCoords.xy;
}

#shader fragment
#version 440 core

layout (location = 0) out vec4 oColor;
layout (location = 1) out vec4 oBright;

in vec2 TexCoords;

uniform sampler2D screenTex;
uniform float threshold;

void main()
{

    oColor = texture(screenTex, TexCoords);
    float brightness = dot(oColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    //oBright = mix(vec4(oColor.rgb, 1.0), vec4(0.0, 0.0, 0.0, 1.0), step(brightness, threshold));
    
    if(brightness > threshold)
    {
        oBright = vec4(oColor.rgb, 1.0);
    }
    else
    {
        vec4(0.0, 0.0, 0.0, 1.0);
    }
}
