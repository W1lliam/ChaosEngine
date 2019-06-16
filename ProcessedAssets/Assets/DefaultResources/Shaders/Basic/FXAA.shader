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
//layout(location = 0) out vec3 color;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTex;
uniform float uGamma;

#define FXAA_MAX 7.0
#define FXAA_REDUCE_MIN 1.0/128.0
#define FXAA_REDUCE_MUL 1.0/7.0

void main()
{
	vec2 texCoordOffset = vec2(1.0 / 1920.0, 1.0 / 1080.0);

	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaTL = dot(luma, texture(screenTex, TexCoords.xy + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
	float lumaTR = dot(luma, texture(screenTex, TexCoords.xy + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
	float lumaBL = dot(luma, texture(screenTex, TexCoords.xy + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
	float lumaBR = dot(luma, texture(screenTex, TexCoords.xy + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
	float lumaM  = dot(luma, texture(screenTex, TexCoords.xy).xyz);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (FXAA_REDUCE_MUL * 0.25), FXAA_REDUCE_MIN);
	float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

	dir = min(vec2(FXAA_MAX, FXAA_MAX),
		max(vec2(-FXAA_MAX, -FXAA_MAX), dir * inverseDirAdjustment)) * texCoordOffset;

	vec3 result1 = (1.0/2.0) * (
		texture(screenTex, TexCoords.xy + (dir * vec2(1.0/3.0 - 0.5))).xyz +
		texture(screenTex, TexCoords.xy + (dir * vec2(2.0/3.0 - 0.5))).xyz);

	vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
		texture(screenTex, TexCoords.xy + (dir * vec2(0.0/3.0 - 0.5))).xyz +
		texture(screenTex, TexCoords.xy + (dir * vec2(3.0/3.0 - 0.5))).xyz);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);

	if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
		FragColor = vec4(result1, 1.0);
	else
		FragColor = vec4(result2, 1.0);

    //FragColor.rgb = FragColor.rgb / (FragColor.rgb + vec3(1.0)); // HDR Calculation

    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/uGamma));
}
