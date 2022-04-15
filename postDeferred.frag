#version 450 core

#define LIGHTS_NUMBER 20

layout (location = 1) in vec2 outTextureCoords;

layout (location = 2) uniform sampler2D fragPosMap;
layout (location = 3) uniform sampler2D diffuseMap;
layout (location = 4) uniform sampler2D normalMap;

layout (std140, binding = 1) uniform Lights
{
	vec3 lightPos[LIGHTS_NUMBER];
};

out vec4 FragColor;


float diffuseLight(vec3 localLightPos, vec3 sampledFragPos, vec3 sampledNormal)
{
	const vec3 lightDirection = normalize(localLightPos - sampledFragPos);
	float result = max(dot(sampledNormal, lightDirection), 0.0f);
	return result * 0.5;
}

float specularLight(vec3 localLightPos, vec3 sampledFragPos, vec3 sampledNormal)
{
	vec3 lightDir = normalize(localLightPos - sampledFragPos);
	vec3 reflectDir = reflect(-lightDir, sampledNormal);
	const vec3 viewDir = normalize(vec3(0.0f, 0.0f, 0.0f) - sampledFragPos);
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

	vec3 sampledFragPos = texture(fragPosMap, outTextureCoords).rgb;
	vec3 sampledNormal = texture(normalMap, outTextureCoords).rgb;
	vec4 sampledColor = texture(diffuseMap, outTextureCoords);

	for (int i = 0; i < LIGHTS_NUMBER; i++)
	{
		lightDistanceComponent = 1.0f - min(abs(distance(sampledFragPos, lightPos[i])) * 0.3, 1.0);

		cacheDiffuse = diffuseLight(lightPos[i], sampledFragPos, sampledNormal) * lightDistanceComponent;
		if (cacheDiffuse > diffuseLightComponent)
		{
			diffuseLightComponent = cacheDiffuse;
		}

		cacheSpecular = specularLight(lightPos[i], sampledFragPos, sampledNormal) * lightDistanceComponent;
		if (cacheSpecular > specularLightComponent)
		{
			specularLightComponent = cacheSpecular;
		}
	}

	FragColor = (ambientLight + diffuseLightComponent + specularLightComponent) * sampledColor;
//	FragColor = vec4(sampledNormal, 1.0f);
}