#shader vertex
#version 440 core  // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier
layout (location = 0) in vec3 aPos;
layout (location = 4) in vec3 aTexCoords;

layout(std140, binding = 0) uniform Matrices
{
   mat4 projection;  	// 64 bytes  // 0
   mat4 view;		      // 64 bytes  // 64
   vec3 viewPos;      // 16 bytes  // 128
};

out vec2 texCoords;
out vec3 camPos;

void main()
{
    gl_Position     = vec4(aPos, 1.0);
    texCoords       = aTexCoords.xy;
    camPos          = viewPos;
}

#shader fragment
#version 440 core
#define MAX_LIGHTS 256
#define PI 3.1415926

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

layout (location = 1) out vec4 oBright;

in vec2 texCoords;
in vec3 camPos;

out vec4 FragColor;

uniform float uThreshold;
uniform float uGamma;

uniform bool uShadowsEnabled;
uniform mat4 uLightSpaceMatrix;

uniform float       uExposure;
uniform samplerCube uIrradiance;
uniform samplerCube uPrefilterMap;
uniform sampler2D   uBRDFTable;

uniform sampler2D uShadow;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughness;
uniform sampler2D gEmissive;
uniform sampler2D gUnlit;


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void ApplyLight(vec4 position, vec4 direction, vec4 color, vec4 lqaf, inout vec3 Lo)
{
  vec3 Normal     = texture(gNormal, texCoords).rgb;    //N
  float Specular  = texture(gMetallicRoughness, texCoords).r;
  vec3 FragPos    = texture(gPosition, texCoords).rgb;
  vec3 viewDir    = normalize(camPos - FragPos);        //V
  vec3 albedo     = pow(texture(gAlbedo, texCoords).rgb, vec3(uGamma));
  float metallic  = texture(gMetallicRoughness, texCoords).r;
  float roughness = texture(gMetallicRoughness, texCoords).g;
  //float metallic  = 0.7;
  //float roughness = 0.3;

  vec3 F0 = vec3(0.04);
  F0= mix(F0, albedo, metallic);

  vec3 lightDir   = mix(normalize(-direction.xyz), normalize(position.xyz - FragPos), step(0.5, position.w)); // L

  vec3 halfwayDir = normalize(viewDir + lightDir); // H

  vec3 reflectDir = reflect(-lightDir, Normal);

  float spec      = pow(max(dot(Normal, halfwayDir), 0.0), 64.0);
  //vec3 specular   = Specular * spec * vec3(color.rgb);

  float NdotL     =	max(0.0, dot(Normal, lightDir));
  vec3 diffuse	  =	NdotL * albedo * color.rgb * color.a;

  float dist = length(position.xyz - FragPos);
  //float atten = position.w == 0 ? 1.0 / (dist * dist) : 1.0 / (1.0 + lqaf.x * dist + lqaf.y * (dist * dist));


  float outterAngle  = mix(0, lqaf.w, step(0.01,lqaf.z));
  float lightToAngle = degrees(acos(dot(-lightDir, normalize(direction.xyz))));

  float atten   = mix(1.0, 1.0 / (1.0 + lqaf.x * dist + lqaf.y * (dist * dist)), step(0.5, position.w));
        atten   = mix(atten, 0, mix(0, smoothstep(lqaf.z, outterAngle, lightToAngle), step(0.5, position.w)));
        atten += 0.0001;
  vec3 radiance = color.rgb * atten;


  float NDF = DistributionGGX(Normal, halfwayDir, roughness);
  float G   = GeometrySmith(Normal, viewDir, lightDir, roughness);
  vec3 F    = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - metallic;

  vec3 numerator    = NDF * G * F;
  // float denominator = lightCount * max(dot(Normal, viewDir), 0.0) * NdotL;
  float denominator = 4.0 * max(dot(Normal, viewDir), 0.0) * NdotL;
  vec3 specular     = numerator / max(denominator, 0.001);


  //Lo += (kD * albedo / PI + specular) * radiance * NdotL;
  Lo += vec3((kD * albedo / PI + specular) * radiance * NdotL) * color.a;


  //atten = position.w ? atten : smoothstep(lqaf.z, outterAngle, lightToAngle);
  // return atten * (diffuse + (specular * color.a));
  // return atten * (diffuse + (metallic * color.a));
}

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

float ApplyShadow()
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

  vec4 fragPosLightspace = uLightSpaceMatrix * vec4(texture(gPosition, texCoords).rgb, 1.0);
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

void main()
{
  vec3 albedo     = texture(gAlbedo, texCoords).rgb;
  vec3 normal     = texture(gNormal, texCoords).rgb;
  vec3 fragPos    = texture(gPosition, texCoords).rgb;

  vec3 Lo = vec3(0.0);
  for(uint i = 0; i < min(lightCount, MAX_LIGHTS); ++i)
  {
    ApplyLight(allLights[i].lightPosition, allLights[i].lightDirection, allLights[i].lightColor, allLights[i].lightLQAF, Lo);
  }
  float shadow = uShadowsEnabled ? ApplyShadow() : 0.0;



 //Environment Diffuse
  float roughness = texture(gMetallicRoughness, texCoords).g;
  float metallic  = texture(gMetallicRoughness, texCoords).r;

  vec3 F0 = vec3(0.04);
  F0= mix(F0, albedo, metallic);
  vec3 viewDir    = normalize(camPos - fragPos);

  vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);

  vec3 kS = F;
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - metallic;

  vec3 irradiance   = texture(uIrradiance, normal).rgb;
       irradiance   = vec3(1.0) - exp(-irradiance * uExposure);
       irradiance   = pow(irradiance, vec3(1.0/uGamma));
//   irradiance        = vec3(pow(irradiance, vec3(2.2)));
  vec3 diffuse      = vec3(irradiance * albedo);

//Environment Reflection
  vec3 reflectDir = reflect(-viewDir, normal);
  const float MAX_REFLECTION_LOD = 4.0;
  vec3 prefilteredColor = textureLod(uPrefilterMap, reflectDir,  roughness * MAX_REFLECTION_LOD).rgb;
       prefilteredColor = vec3(1.0) - exp(-prefilteredColor * uExposure);
       prefilteredColor = pow(prefilteredColor, vec3(1.0/uGamma));
  vec2 envBRDF          = texture(uBRDFTable, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
  vec3 specular         = prefilteredColor * (F * envBRDF.x + envBRDF.y);

  vec3 ambient	        = kD * diffuse + specular;

  vec3 finalColor =	(ambient + (1.0 - shadow) * Lo);
  //finalColor = finalColor / (finalColor + vec3(1.0)); // HDR Calculation
  finalColor += texture(gEmissive, texCoords).rgb;

  vec3 unlitColor =	texture(gUnlit, texCoords).rgb;
  finalColor = unlitColor == vec3(0.0)? finalColor : unlitColor;

  FragColor		    =	vec4(finalColor, 1.0);
  //// vec4 buf = texture(screenTex, texCoords);
  //// FragColor = buf;

  //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/GAMMA));

  float brightness = dot(FragColor.rgb, vec3(0.2125, 0.7154, 0.0721));

    //oBright = mix(vec4(oColor.rgb, 1.0), vec4(0.0, 0.0, 0.0, 1.0), step(brightness, threshold));

    if(brightness > uThreshold)
    {
        oBright = vec4(FragColor.rgb, 0.0);
    }
    else
    {
        oBright = vec4(0.0, 0.0, 0.0, 0.0);
    }

    if(any(isnan(oBright)) || any(isinf(oBright)))
    {
      oBright = vec4(1.0, 0.0, 0.0, 1.0);
    }

    if(any(isnan(FragColor)) || any(isinf(FragColor)))
    {
      FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
}


//FragColor = vec4(vec3(1.0 - texture(screenTex, texCoords)), 1.0); //invert post process effect
