#shader vertex
#version 440 core  // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

layout(std140, binding = 0) uniform Matrices
{
   mat4 projection;  	// 64 bytes  // 0
   mat4 view;		      // 64 bytes  // 64
   vec3 viewPos;      // 16 bytes  // 128
};

out vec3 cameraPos;

struct Vertex
{
  noperspective float m_edgeDistance;
  noperspective float m_size;
  smooth vec4 m_color;
}

out Vertex vData;

void main()
{
    vData.m_color = aColor.rgba;
    vData.m_size = 2.0;
    gl_Position = projection * view * vec4(aPos, 1.0);

    cameraPos = viewPos;
}

#shader fragment
#version 440 core

#define GAMMA 1.0

in vec3 cameraPos;
in Vertex vData;

layout(location=0) out vec4 fResult;

void main()
{
  fResult = vData.m_color;

  float d = abs(vData.m_edgeDistance) / vData.m_size;
	d = smoothstep(1.0, 1.0 - (1.5 / vData.m_size), d);
	fResult.a *= d;
}

#shader geometry
#version 440 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in Vertex vData[];
out Vertex vDataOut;

void main()
{
  vec2 uViewport =(1920,1280);
  vec2 pos0 = gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
	vec2 pos1 = gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;

  vec2 dir = pos0 - pos1;
  dir = normalize(vec2(dir.x, dir.y * uViewport.y / uViewport.x));
  vec2 tng0 = vec2(-dir.y, dir.x);
	vec2 tng1 = tng0 * vData[1].m_size / uViewport;
	tng0 = tng0 * vData[0].m_size / uViewport;

  // line start
	gl_Position = vec4((pos0 - tng0) * gl_in[0].gl_Position.w, gl_in[0].gl_Position.zw); 
	vDataOut.m_edgeDistance = -vData[0].m_size;
	vDataOut.m_size = vData[0].m_size;
	vDataOut.m_color = vData[0].m_color;
	EmitVertex();
	
	gl_Position = vec4((pos0 + tng0) * gl_in[0].gl_Position.w, gl_in[0].gl_Position.zw);
	vDataOut.m_color = vData[0].m_color;
	vDataOut.m_edgeDistance = vData[0].m_size;
	vDataOut.m_size = vData[0].m_size;
	EmitVertex();
	
	// line end
	gl_Position = vec4((pos1 - tng1) * gl_in[1].gl_Position.w, gl_in[1].gl_Position.zw);
	vDataOut.m_edgeDistance = -vData[1].m_size;
	vDataOut.m_size = vData[1].m_size;
	vDataOut.m_color = vData[1].m_color;
	EmitVertex();
	
	gl_Position = vec4((pos1 + tng1) * gl_in[1].gl_Position.w, gl_in[1].gl_Position.zw);
	vDataOut.m_color = vData[1].m_color;
	vDataOut.m_size = vData[1].m_size;
	vDataOut.m_edgeDistance = vData[1].m_size;
	EmitVertex();
}