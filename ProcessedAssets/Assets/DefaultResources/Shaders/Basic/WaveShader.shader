
  #shader vertex
  #version 440 core
	#define PI 3.1415926
  layout (location = 0) in vec3 aPos;
  layout (location = 1) in vec3 aNormal;
  layout (location = 2) in vec3 aTangents;
  layout (location = 3) in vec3 aBitangents;
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
    vec2 vertexUV;
    vec3 fragPos;
    vec3 vertexNormal;

  } v_out;

  uniform mat4 uModel;

	uniform float time;

	vec3 GerstnerWave (vec4 wave, vec3 p, inout vec3 tangent, inout vec3 binormal)
	{
		    float steepness = wave.z;
		    float wavelength = wave.w;
		    float k = 2 * PI / wavelength;
			float c = sqrt(9.8 / k);
			vec2 d = normalize(wave.xy);
			float f = k * (dot(d, p.xz) - c * time);
			float a = steepness / k;

			//p.x += d.x * (a * cos(f));
			//p.y = a * sin(f);
			//p.z += d.y * (a * cos(f));

			tangent += vec3(-d.x * d.x * (steepness * sin(f)),
											d.x * (steepness * cos(f)),
											-d.x * d.y * (steepness * sin(f))
											);
			binormal += vec3(-d.x * d.y * (steepness * sin(f)),
												d.y * (steepness * cos(f)),
												-d.y * d.y * (steepness * sin(f))
												);
			return vec3(d.x * (a * cos(f)),
									a * sin(f),
									d.y * (a * cos(f))
									);
		}

  void main()
  {
		//Wave
		vec4 WA = vec4(1,0,0.5,10);
		vec4 WB = vec4(0,1,0.25,20);
		vec4 WC = vec4(1,1,0.15,10);

		vec3 gridPoint = aPos.xyz;

		vec3 tangent = 0;
		vec3 binormal = 0;
		vec3 p = gridPoint;

		p += GerstnerWave(WA, gridPoint, tangent, binormal);
		p += GerstnerWave(WB, gridPoint, tangent, binormal);
		p += GerstnerWave(WC, gridPoint, tangent, binormal);
		vec3 normal = normalize(cross(binormal, tangent));

		vertexData.vertex.xyz = p;
		vertexData.normal = normal;
		//EndWave

    v_out.vertexUV        = aUV.xy;
    v_out.fragPos         = vec3(uModel * vec4(aPos, 1.0));

    v_out.vertexNormal     = (transpose(inverse(uModel)) * vec4(aNormal, 0.0)).xyz;

    gl_Position           = projection * view * uModel * vec4(aPos, 1.0);
  }

  #shader fragment
  #version 440 core
  #define GAMMA 1.1

  layout (location = 0) out vec4 gPosition;
  layout (location = 1) out vec4 gNormal;
  layout (location = 2) out vec4 gAlbedo;
  layout (location = 3) out vec4 gMetallicRoughness;
  layout (location = 4) out vec4 gEmissive;


  in VS_INFO
  {
    vec2 vertexUV;
    vec3 fragPos;
    vec3 vertexNormal;

  } v_in;

  uniform vec3  albedo;
  uniform vec3  emissive;
  uniform float metallic;
  uniform float roughness;

  void main()
  {
      gPosition           = vec4(v_in.fragPos,1.0);
      gNormal             = vec4(normalize(v_in.vertexNormal),1.0);
      gAlbedo.rgb         = albedo;
      gAlbedo.a           = 1.0;
      gMetallicRoughness  = vec4(metallic, roughness, 0.0, 1.0);
      gEmissive           = vec4(emissive, 1.0);
  }
