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
  vec4 vertexColor;
  vec2 vertexUV;
  vec3 viewDir;
  vec3 vertexNormal;
  vec3 fragPos;
} v_out;


out vec3 cameraPos;
out vec4 fragPosLight;

uniform mat4 uModel;

uniform mat4 uLightSpaceMatrix;

void main()
{
    v_out.fragPos         = vec3(uModel * vec4(aPos, 1.0));

    v_out.vertexColor     = vec4(aColor.xyz, 1.0);
    v_out.vertexUV        = aUV.xy;
    v_out.viewDir         = normalize(viewPos - v_out.fragPos);
    v_out.vertexNormal    = (transpose(inverse(uModel)) * vec4(aNormal, 1.0)).xyz;

    cameraPos = viewPos;
    fragPosLight = uLightSpaceMatrix * vec4(v_out.fragPos, 1.0);
    gl_Position = projection * view * uModel * vec4(aPos, 1.0);
}

#shader fragment
#version 440 core

#define GAMMA 1.1
#define MAX_LIGHTS 256

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

uniform sampler2D uTexture;
uniform sampler2D uShadow;

out vec4 FragColor;

in VS_INFO
{
  vec4 vertexColor;
  vec2 vertexUV;
  vec3 viewDir;
  vec3 vertexNormal;
  vec3 fragPos;
} v_in;

in vec3 cameraPos;
in vec4 fragPosLight;

float sampleShadowMap(sampler2D shadow, vec2 coords, float compare, vec2 texelSize)
{
  vec2 pixelpos = coords/texelSize + vec2(0.5);
  vec2 fracPart = fract(pixelpos);
  vec2 startTexel = (pixelpos - fracPart) * texelSize;

  float blTexel = step(compare, texture(shadow, startTexel).r);
  float brTexel = step(compare, texture(shadow, startTexel + vec2(texelSize.x, 0.0)).r);
  float tlTexel = step(compare, texture(shadow, startTexel + vec2(0.0, texelSize.y)).r);
  float trTexel = step(compare, texture(shadow, startTexel + texelSize).r);

  float mixA = mix(blTexel, tlTexel, fracPart.y);
  float mixB = mix(brTexel, trTexel, fracPart.y);

  return mix(mixA, mixB, fracPart.x);
}

float ApplyShadow(vec4 fragPosLightspace)
{
  vec2 poissonDisk[16] = vec2[](
     vec2( -0.94201624, -0.39906216 ),
     vec2( 0.94558609, -0.76890725 ),
     vec2( -0.094184101, -0.92938870 ),
     vec2( 0.34495938, 0.29387760 ),
     vec2( -0.91588581, 0.45771432 ),
     vec2( -0.81544232, -0.87912464 ),
     vec2( -0.38277543, 0.27676845 ),
     vec2( 0.97484398, 0.75648379 ),
     vec2( 0.44323325, -0.97511554 ),
     vec2( 0.53742981, -0.47373420 ),
     vec2( -0.26496911, -0.41893023 ),
     vec2( 0.79197514, 0.19090188 ),
     vec2( -0.24188840, 0.99706507 ),
     vec2( -0.81409955, 0.91437590 ),
     vec2( 0.19984126, 0.78641367 ),
     vec2( 0.14383161, -0.14100790 )
  );

  vec3 coords = fragPosLightspace.xyz / fragPosLightspace.w;
  coords = coords.xyz * vec3(0.5) + vec3(0.5);        // transform from -1 - 1 to 0 - 1

  float closestDepth = texture(uShadow, coords.xy).r;

  float currentDepth = coords.z;
  //float bias = max(0.05 * (dot(normalize(v_in.vertexNormal), allLights[0].lightDirection.xyz)), 0.0015);
  //float bias = 0.005 * tan(acos(clamp(dot(normalize(v_in.vertexNormal), allLights[0].lightDirection.xyz), 0.0, 1.0) ));
  //bias = clamp(bias, 0,0.01);
  float bias = 0.015;
  float shadow = 1.0;
   vec2 texelSize = 1.0/textureSize(uShadow, 0);
  for(int x = -2; x <= 2; ++x)
  {
    for(int y = -2; y <= 2; ++y)
    {
      vec2 coordsOffset = vec2(y,x)*texelSize;
      shadow -= sampleShadowMap(uShadow, coords.xy + coordsOffset, currentDepth - bias, texelSize);
    }
  }
  shadow /= 25.0;

  // for(int i = 0; i < 4; ++i)
  // {
  //   if(texture( uShadow, coords.xy + poissonDisk[i]/700.0 ).z  <  coords.z - bias )
  //   {
  //     shadow-=0.2;
  //   }
  // }
  shadow = mix(0.0, shadow, step(coords.z, 1.0));

    //spot light
   //if ( texture( shadowMap, (ShadowCoord.xy/ShadowCoord.w) ).z  <  (ShadowCoord.z-bias)/ShadowCoord.w )
  return shadow * allLights[0].lightColor.a ;
}

vec3 ApplyLight(vec4 position, vec4 direction, vec4 color, vec4 lqaf)
{
  vec3 normal		  =	normalize(v_in.vertexNormal);

  vec3 lightDir   = mix(normalize(direction.xyz), normalize(position.xyz - v_in.fragPos), step(0.5, position.w));

  vec3 halfwayDir = normalize(lightDir + v_in.viewDir);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec      = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
  vec3 specular   = 1.0 * spec * vec3(color.rgb);

  float NdotL		  =	max(0.0, dot(lightDir, normal));
  vec3 diffuse	  =	NdotL * color.rgb * color.a;

  float dist = length(position.xyz - v_in.fragPos);
  float atten = 1.0 / (1.0 + lqaf.x * dist + lqaf.y * (dist * dist));
  atten = mix(1.0, atten, step(0.5, position.w));

  float outterAngle =  mix(0, lqaf.w, step(0.01,lqaf.z));

  float lightToAngle = degrees(acos(dot(-lightDir, normalize(-direction.xyz))));

  atten = mix(atten, 0, mix(0, smoothstep(lqaf.z, outterAngle, lightToAngle), step(0.5, position.w)));

  //float shadow = ApplyShadow(fragPosLight);
  return atten * (diffuse + (specular * color.a));
}

void main()
{
    vec3 ambient	  =	vec3(0.1);       // Create Ambient
    vec3 lightCalculation;
    for(uint i = 0; i < lightCount; ++i)
    {
      lightCalculation +=	ApplyLight(allLights[i].lightPosition, allLights[i].lightDirection, allLights[i].lightColor, allLights[i].lightLQAF);
    }
    //vec3 finalColor =	(ambient + ApplyLight(lightPosition, lightDirection, lightColor, lightLQAF)) * texture(uTexture, v_in.vertexUV).rgb;
    float shadow = ApplyShadow(fragPosLight);
    vec3 finalColor =	(ambient + (1.0 - shadow) * lightCalculation) * texture(uTexture, v_in.vertexUV).rgb;
    FragColor		    =	vec4(finalColor, 1.0);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/GAMMA));
}
