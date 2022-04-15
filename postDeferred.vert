#version 450 core

layout (location = 0) in vec2 VertexPosition;
layout (location = 1) in vec2 inTextureCoords;

layout (location = 1) out vec2 outTextureCoords;

void main()
{
	gl_Position = vec4(VertexPosition, 0.0f, 1.0f);
	outTextureCoords = inTextureCoords;
}