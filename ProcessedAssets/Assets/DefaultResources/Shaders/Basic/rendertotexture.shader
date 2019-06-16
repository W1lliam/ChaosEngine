#shader vertex
#version 440 core  // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aTexCoords;

layout(std140, binding = 0) uniform Matrices
{
   mat4 projection;  	// 64 bytes  // 0
   mat4 view;		      // 64 bytes  // 64
   vec3 viewPos;      // 16 bytes  // 128
};

out vec2 TexCoords;
out vec3 camPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoords   = aTexCoords.xy;
    camPos      = viewPos;
}

#shader fragment
#version 440 core
#define MAX_LIGHTS 256
#define GAMMA 1.1

struct Light
{
  vec4   lightPosition;            // 16 bytes    // 0         // Position:  [x,y,z, LightType]
  vec4   lightColor;               // 16 bytes    // 16        // Color:     [r,g,b, Intensity]
  vec4   lightDirection;           // 16 bytes    // 32
  vec4   lightLQAF;                // 16 bytes    // 48        // LQAF:      [Linear, Quadratic, Angle, Falloff]
};

layout(std140, binding = 1) uniform LightData
{
   Light  allLights[MAX_LIGHTS];
   uint   lightCount;
};

out vec4 FragColor;
in vec2 TexCoords;
in vec3 camPos;

// uniform sampler2D screenTex;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gEmissive;

vec3 ApplyLight(vec4 position, vec4 direction, vec4 color, vec4 lqaf)
{
  vec3 Normal     = texture(gNormal, TexCoords).rgb;
  float Specular  = texture(gAlbedoSpec, TexCoords).a;
  vec3 FragPos    = texture(gPosition, TexCoords).rgb;
  vec3 viewDir    = normalize(camPos - FragPos);

  vec3 lightDir   = mix(normalize(direction.xyz), normalize(position.xyz - FragPos), step(0.5, position.w));

  vec3 halfwayDir = normalize(lightDir + viewDir);

  vec3 reflectDir = reflect(-lightDir, Normal);


  float spec      = pow(max(dot(Normal, halfwayDir), 0.0), 64.0);
  vec3 specular   = Specular * spec * vec3(color.rgb);

  float NdotL		  =	max(0.0, dot(lightDir, Normal));
  vec3 diffuse	  =	NdotL * color.rgb * color.a;

  float dist = length(position.xyz - FragPos);
  float atten = 1.0 / (1.0 + lqaf.x * dist + lqaf.y * (dist * dist));
  atten = mix(1.0, atten, step(0.5, position.w));

  float outterAngle =  mix(0, lqaf.w, step(0.01,lqaf.z));
  float lightToAngle = degrees(acos(dot(-lightDir, normalize(-direction.xyz))));
  atten = mix(atten, 0, mix(0, smoothstep(lqaf.z, outterAngle, lightToAngle), step(0.5, position.w)));
  return atten * (diffuse + (specular * color.a));
}

void main()
{
  vec3 Albedo     = texture(gAlbedoSpec, TexCoords).rgb;
  vec3 ambient	  =	Albedo * 0.1;       // Create Ambient

  vec3 lightCalculation;
  for(uint i = 0; i < min(lightCount, MAX_LIGHTS); ++i)
  {
    lightCalculation +=	ApplyLight(allLights[i].lightPosition, allLights[i].lightDirection, allLights[i].lightColor, allLights[i].lightLQAF);
  }

  vec3 finalColor =	(ambient + lightCalculation) * Albedo;
  finalColor += texture(gEmissive, TexCoords).rgb;
  FragColor		    =	vec4(finalColor, 1.0);
  // vec4 buf = texture(screenTex, TexCoords);
  // FragColor = buf;
  FragColor.rgb = pow(FragColor.rgb, vec3(1.0/GAMMA));
}



//FragColor = vec4(vec3(1.0 - texture(screenTex, TexCoords)), 1.0); //invert post process effect
