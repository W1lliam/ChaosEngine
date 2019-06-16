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

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(uImage, 0); // gets size of single texel
    vec3 result = texture(uImage, TexCoords).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uImage, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(uImage, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uImage, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(uImage, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }

    //result = max(0.0, result);
    FragColor = vec4(result, 1.0);
}
