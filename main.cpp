#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <stb/stb_image.h>

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <time.h>

#include <own/ShaderClass.hpp>
#include <own/window.hpp>
#include "Models OBJ Loader.hpp"
#include "objects.hpp"
#include "AppIntervalLib.hpp"

#define RENDER_MODE_FORWARD 0
#define RENDER_MODE_DEFERRED 1
#define RENDER_MODE_DISABLED 2

#define LIGHTS_NUMBER 20

void init(uint32_t* lightsBuffer)
{
	srand(time(NULL));

	uint32_t UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(window::width) / float(window::height), 0.1f, 10.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(view));
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);



	glGenBuffers(1, lightsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, *lightsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3) * LIGHTS_NUMBER, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, *lightsBuffer);
}


int main()
{
	bool renderMode = RENDER_MODE_DEFERRED;
	// initalize GLFW library and OpenGL version
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4);
	window::width = 1920;
	window::height = 1080;
	window::window = glfwCreateWindow(window::width, window::height, "Deferred Shading", NULL, NULL);
	glfwMakeContextCurrent(window::window);
	glfwSwapInterval(0);

	gladLoadGL();
	glViewport(0, 0, window::width, window::height);	

	glm::vec3 lights[LIGHTS_NUMBER] = { glm::vec3(1.0f, 1.0f, 1.0f) };
	uint32_t lightsBuffer;

	init(&lightsBuffer);

	glBindBuffer(GL_UNIFORM_BUFFER, lightsBuffer);
	for (uint16_t i = 0, offset = 0; i < LIGHTS_NUMBER; i++, offset += 12)
	{
		lights[i].x = (rand() % 10 + 0) - 5.0f;
		lights[i].y = (rand() % 10 + 0) - 5.0f;
		lights[i].z = (rand() % 3 + 0) - 3.0f;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, glm::value_ptr(lights[i]));
	}


	OBJ_Loader::Loader SwordLoader(new OBJ_Loader::Storage_MSDO);
	SwordLoader.loadMaterials("sword");
	SwordLoader.loadOBJFile("sword");
	const uint32_t verticesCount = *SwordLoader.modelDataStoragePtr->verticesCount;



	glm::vec3* VerticesLocal = new glm::vec3[verticesCount];
	SwordLoader.modelDataStoragePtr->streamMeshVerticesPositionData(VerticesLocal);

	glm::vec3* NormalsLocal = new glm::vec3[verticesCount];
	SwordLoader.modelDataStoragePtr->streamMeshNormalVectorsData(NormalsLocal);

	uint32_t SwordVAO, SwordVertices, SwordNormals, SwordTextureCoordinates;
	glGenVertexArrays(1, &SwordVAO);
	glBindVertexArray(SwordVAO);
	glGenBuffers(1, &SwordVertices);
	glBindBuffer(GL_ARRAY_BUFFER, SwordVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verticesCount, VerticesLocal, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &SwordNormals);
	glBindBuffer(GL_ARRAY_BUFFER, SwordNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verticesCount, NormalsLocal, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);
	glEnableVertexAttribArray(1);
	delete[] VerticesLocal;
	delete[] NormalsLocal;


	Swords Obj;


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glCullFace(GL_FRONT);

	AppIntervalLib::AppIntervalTimer Timer(FPS_LOCK::_30);


	Shaders::BasicPipeline ForwardShading("forward.vert", "forward.frag", true);
	Shaders::BasicPipeline preDeferredShading("preDeferred.vert", "preDeferred.frag", true);
	Shaders::BasicPipeline postDeferredShading("postDeferred.vert", "postDeferred.frag", true);



	uint32_t positionMap, diffuseMap, normalMap;
	glGenTextures(1, &positionMap);
	glGenTextures(1, &diffuseMap);
	glGenTextures(1, &normalMap);


	uint32_t deferredFrame;
	glGenFramebuffers(1, &deferredFrame);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFrame);

	glBindTexture(GL_TEXTURE_2D, positionMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, window::width, window::height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionMap, 0);

	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, window::width, window::height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diffuseMap, 0);

	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, window::width, window::height, 0, GL_RGB, GL_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, normalMap, 0);

	uint32_t attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glNamedFramebufferDrawBuffers(deferredFrame, 3, attachments);

	uint32_t deferredVAO, deferredVBO;
	glGenVertexArrays(1, &deferredVAO);
	glBindVertexArray(deferredVAO);

	float deferredPlane[24]
	{
		-1.0f, -1.0f,		0.0f, 0.0f,
		1.0f, -1.0f,		1.0f, 0.0f,
		1.0f, 1.0f,			1.0f, 1.0f,

		1.0f, 1.0f,			1.0f, 1.0f,
		-1.0f, 1.0f,		0.0f, 1.0f,
		-1.0f, -1.0f,		0.0f, 0.0f
	};

	glGenBuffers(1, &deferredVBO);
	glBindBuffer(GL_ARRAY_BUFFER, deferredVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(deferredPlane), deferredPlane, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);
	glEnableVertexAttribArray(1);



	// main app loop
	while (!glfwWindowShouldClose(window::window))
	{
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (renderMode != RENDER_MODE_DISABLED)
		{
			if (renderMode == RENDER_MODE_FORWARD)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glUseProgram(*ForwardShading.program);
				glBindVertexArray(SwordVAO);
				glDrawArraysInstanced(GL_TRIANGLES, 0, verticesCount, 30);
			}
			else if (renderMode == RENDER_MODE_DEFERRED)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, deferredFrame);
				glUseProgram(*preDeferredShading.program);
				glBindVertexArray(SwordVAO);				
				glDrawArraysInstanced(GL_TRIANGLES, 0, verticesCount, 30);


				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glUseProgram(*postDeferredShading.program);
				glBindVertexArray(deferredVAO);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, positionMap);
				glUniform1i(2, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, diffuseMap);
				glUniform1i(3, 1);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, normalMap);
				glUniform1i(4, 2);

				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}

		if (Timer.timerFlag)
		{
			if (glfwGetKey(window::window, GLFW_KEY_LEFT))
			{
				lights[0].x -= 0.05f;
			}
			else if (glfwGetKey(window::window, GLFW_KEY_RIGHT))
			{
				lights[0].x += 0.05f;
			}
			if (glfwGetKey(window::window, GLFW_KEY_UP))
			{
				lights[0].y += 0.025f;
			}
			else if (glfwGetKey(window::window, GLFW_KEY_DOWN))
			{
				lights[0].y -= 0.025f;
			}
			if (glfwGetKey(window::window, GLFW_KEY_1))
			{
				renderMode = RENDER_MODE_FORWARD;
			}
			else if (glfwGetKey(window::window, GLFW_KEY_2))
			{
				renderMode = RENDER_MODE_DEFERRED;
			}
			glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, glm::value_ptr(lights[0]));
		}
		
		glfwSwapBuffers(window::window);
	}
}