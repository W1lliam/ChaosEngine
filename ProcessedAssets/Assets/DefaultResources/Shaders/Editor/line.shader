#shader vertex
#version 440 core  // 330 :: changed opengl version from 3.3 to 4.2 to implement binding qualifier
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;

layout(std140, binding = 0) uniform Matrices
{
   mat4 projection;  	// 64 bytes  // 0
   mat4 view;		      // 64 bytes  // 64
   vec3 viewPos;      // 16 bytes  // 128
};

out VertexData
{
  smooth 				vec4 m_color;
	noperspective float m_size;
} VertexOut;

//uniform mat4 uModel;

void main()
{
    gl_Position 				= projection * view * vec4(aPos.xyz, 1.0);
		VertexOut.m_color 	= aColor;
    VertexOut.m_color.a *= smoothstep(0.0, 1.0, aPos.w / 2.0);
		VertexOut.m_size		= max(aPos.w, 2.0);
}

#shader geometry
#version 440 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in VertexData
{
  smooth 				vec4 	m_color;
	noperspective float m_size;
} vData[];

out VertexData
{
   	smooth 				vec4 m_color;
		noperspective float m_size;
		noperspective float m_edgeDistance;
} vDataOut;

	void main()
	{
		vec2 uViewport = vec2(1280,720);

		vec2 pos0 = gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
		vec2 pos1 = gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;

		vec2 dir 	= pos0 - pos1;
		dir 			= normalize(vec2(dir.x, dir.y * uViewport.y / uViewport.x)); // correct for aspect ratio
		vec2 tng0 = vec2(-dir.y, dir.x);
		vec2 tng1 = tng0 * vData[1].m_size / uViewport;
		tng0 			= tng0 * vData[0].m_size / uViewport;

	 // line start
		gl_Position 	= vec4((pos0 - tng0) * gl_in[0].gl_Position.w, gl_in[0].gl_Position.zw);
		vDataOut.m_edgeDistance = -vData[0].m_size;
		vDataOut.m_size 	= vData[0].m_size;
		vDataOut.m_color 	= vData[0].m_color;
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

		EndPrimitive();
	}

#shader fragment
#version 440 core

in VertexData
{
  smooth vec4 m_color;
	noperspective float m_size;
	noperspective float m_edgeDistance;
} VertexIn;

out vec4 FragColor;

void main()
{
	FragColor 	= VertexIn.m_color;
	float d 		= abs(VertexIn.m_edgeDistance) / VertexIn.m_size;
	d 					= smoothstep(1.0, 1.0 - (2.0 / VertexIn.m_size), d);
	FragColor.a *= d;
}
