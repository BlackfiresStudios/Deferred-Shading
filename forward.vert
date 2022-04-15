#version 450 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 inNormalVector;
layout (location = 2) in mat4 inRotationMatrix;
layout (location = 6) in mat4 TranslateMatrix;


layout (std140, binding = 0) uniform proj
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;
};

layout (location = 0) out vec3 fragPos;
layout (location = 1) out vec3 outNormalVector;

void main()
{
	gl_Position = ProjectionMatrix * ViewMatrix * TranslateMatrix * inRotationMatrix * vec4(VertexPosition, 1.0f);
	outNormalVector =  vec4(inRotationMatrix * vec4(inNormalVector, 1.0f)).rgb;
	fragPos = vec3(TranslateMatrix * inRotationMatrix * vec4(VertexPosition, 1.0f));
}