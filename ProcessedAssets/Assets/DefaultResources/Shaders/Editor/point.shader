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

out VertexData
{
  smooth 				vec4 m_color;
	noperspective float m_size;
} VertexOut;

uniform float uSize;

void main()
{
    gl_Position 				= projection * view * vec4(aPos, 1.0);
		VertexOut.m_color 	= aColor;
		VertexOut.m_size		= max(uSize, 2.0);

    gl_PointSize = VertexOut.m_size;
}

#shader fragment
#version 440 core

in VertexData
{
  smooth vec4 m_color;
	noperspective float m_size;
} VertexIn;

in vec3 campos;
out vec4 FragColor;

void main()
{
	FragColor 	= VertexIn.m_color;
	float d 		= length(gl_PointCoord.xy - vec2(0.5));
	d 					= smoothstep(0.5, 0.5 - (2.0 / VertexIn.m_size), d);
	FragColor.a *= d;
}
