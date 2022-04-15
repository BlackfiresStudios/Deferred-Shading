#pragma once


class Swords
{
public:
	Swords()
	{

		const uint32_t swordsCount = 30;
		glm::mat4* transformTable = new glm::mat4[swordsCount];
		glm::mat4* rotationTable = new glm::mat4[swordsCount];


		glm::vec3 cacheVector = glm::vec3(-10.0f, 0.0f, -4.0f);
		float angle = 0.0f;
		for (uint16_t i = 0; i < swordsCount; i++)
		{
			transformTable[i] = glm::translate(glm::mat4(1.0f), cacheVector);
			cacheVector.x += 0.5f;
			cacheVector.y = (rand() % 4 + 0) - 2.0f;
			cacheVector.z = (rand() % 3 + 0) * -1.0f;

			angle = rand() % 180 + 0;
			angle -= 90.0f;
			rotationTable[i] = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		uint32_t rotationAttrib;
		glGenBuffers(1, &rotationAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, rotationAttrib);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * swordsCount, rotationTable, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 64, (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribDivisor(2, 1);

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 64, (void*)16);
		glEnableVertexAttribArray(3);
		glVertexAttribDivisor(3, 1);

		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 64, (void*)32);
		glEnableVertexAttribArray(4);
		glVertexAttribDivisor(4, 1);

		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 64, (void*)48);
		glEnableVertexAttribArray(5);
		glVertexAttribDivisor(5, 1);



		uint32_t translateAttrib;
		glGenBuffers(1, &translateAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, translateAttrib);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * swordsCount, transformTable, GL_STATIC_DRAW);

		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 64, (void*)0);
		glEnableVertexAttribArray(6);
		glVertexAttribDivisor(6, 1);

		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 64, (void*)16);
		glEnableVertexAttribArray(7);
		glVertexAttribDivisor(7, 1);

		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 64, (void*)32);
		glEnableVertexAttribArray(8);
		glVertexAttribDivisor(8, 1);

		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 64, (void*)48);
		glEnableVertexAttribArray(9);
		glVertexAttribDivisor(9, 1);
	}
};