#version 450 core

layout (location = 0) in vec4 fragPos;
layout (location = 1) in vec3 outNormalVector;


layout (location = 0) out vec3 FragmentPos;
layout (location = 1) out vec3 DiffuseColor;
layout (location = 2) out vec3 NormalColor;


void main()
{	
	FragmentPos = fragPos.xyz;
	DiffuseColor = vec3(1.0f, 1.0f, 1.0f);
	NormalColor = normalize(outNormalVector);
}