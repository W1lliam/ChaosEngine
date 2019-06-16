#shader vertex
#version 440 core
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

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uImage;
uniform sampler2D uBloomBlur;
uniform float uIntensity;

void main()
{
    vec3 finalColor = texture(uImage, TexCoords).rgb;
    vec3 bloomColor = texture(uBloomBlur, TexCoords).rgb;

    //bloomColor = bloomColor / (bloomColor + vec3(1.0));
    
    finalColor += (bloomColor * uIntensity);

    FragColor = vec4(finalColor, 1.0);
}
