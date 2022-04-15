#version 450 core

#define LIGHTS_NUMBER 20

layout (location = 0) in vec3 fragPos;
layout (location = 1) in vec3 outNormalVector;

layout (std140, binding = 1) uniform Lights
{
	vec3 lightPos[LIGHTS_NUMBER];
};

out vec4 FragColor;

float diffuseLight(vec3 localLightPos)
{
	vec3 normal = normalize(outNormalVector);
	const vec3 lightDirection = normalize(localLightPos - fragPos);
	float result = max(dot(normal, lightDirection), 0.0f);
	return result * 0.5;
}

float specularLight(vec3 localLightPos)
{
	vec3 normal = normalize(outNormalVector);
	vec3 lightDir = normalize(localLightPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	const vec3 viewDir = normalize(vec3(0.0f, 0.0f, 0.0f) - fragPos);

	const vec3 halfDir = normalize(lightDir + viewDir);
	float result = pow(max(dot(halfDir, reflectDir), 0.0f), 2);
	return result;
}

void main()
{
	const float ambientLight = 0.025f;
	float diffuseLightComponent = 0.0f;
	float specularLightComponent = 0.0f;
	float lightDistanceComponent;

	float cacheDiffuse = 0.0f;
	float cacheSpecular = 0.0f;

	for (int i = 0; i < LIGHTS_NUMBER; i++)
	{
		lightDistanceComponent = 1.0f - min(abs(distance(fragPos, lightPos[i])) * 0.3, 1.0);

		cacheDiffuse = diffuseLight(lightPos[i]) * lightDistanceComponent;
		if (cacheDiffuse > diffuseLightComponent)
		{
			diffuseLightComponent = cacheDiffuse;
		}

		cacheSpecular = specularLight(lightPos[i]) * lightDistanceComponent;
		if (cacheSpecular > specularLightComponent)
		{
			specularLightComponent = cacheSpecular;
		}
	}
	FragColor = (ambientLight + diffuseLightComponent + specularLightComponent) * vec4(1.0f, 1.0f, 1.0f, 1.0f);
}