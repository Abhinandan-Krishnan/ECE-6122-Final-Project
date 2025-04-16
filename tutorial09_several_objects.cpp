// Include standard headers

/*

Author: Abhinandan Krishnan
Class: ECE6122
Last Date Modified: 12/02/2023

Description:
C++ OpenGl application that renders 4 pumpkin objects and mimic random movements in a 3D space. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <omp.h>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include<stb/stb_image.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1920, 1080, "Tutorial 09 - Rendering several models", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1920 / 2, 1080 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");

	GLuint Texture = loadDDS("uvmap.DDS");


	// Load the texture

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	GLint light = glGetUniformLocation(programID, "light");
	glUniform1f(light, 10.0f);
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("texture/pumpkin.obj", vertices, uvs, normals);

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VBO
	//GLuint Texture = loadDDS("uvmap.DDS");

	/////////////////////////////SECOND OBJECT/////////////////////////////////
	std::vector<glm::vec3> cubeVertices;
	std::vector<glm::vec2> cubeUvs;
	std::vector<glm::vec3> cubeNormals;
	bool cubeRes = loadOBJ("suzanne.obj", cubeVertices, cubeUvs, cubeNormals);

	std::vector<unsigned short> cubeIndices;
	std::vector<glm::vec3> indexedCubeVertices;
	std::vector<glm::vec2> indexedCubeUvs;
	std::vector<glm::vec3> indexedCubeNormals;
	indexVBO(cubeVertices, cubeUvs, cubeNormals, cubeIndices, indexedCubeVertices, indexedCubeUvs, indexedCubeNormals);


	GLuint cubeVertexBuffer, cubeUvBuffer, cubeNormalBuffer, cubeElementBuffer;
	glGenBuffers(1, &cubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, indexedCubeVertices.size() * sizeof(glm::vec3), &indexedCubeVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &cubeUvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeUvBuffer);
	glBufferData(GL_ARRAY_BUFFER, indexedCubeUvs.size() * sizeof(glm::vec2), &indexedCubeUvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &cubeNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, indexedCubeNormals.size() * sizeof(glm::vec3), &indexedCubeNormals[0], GL_STATIC_DRAW);


	glGenBuffers(1, &cubeElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned short), &cubeIndices[0], GL_STATIC_DRAW);

	/////////////////////////////////////////////////////////////////////////////////////
	
	///////////// TEXTURE STUFF ////////////////////

	
	GLfloat size(30.00f);
	GLfloat verticesFloor[] =
	{
		-size,-size,0.0f,  0.0f,0.0f,
		-size,size,0.0f,  0.0f,10.0f,
		size,size,0.0f,  10.0f,10.0f,
		size,-size,0.0f,  10.0f,0.0f,
	};

	GLuint indicesEnv[] =
	{
		0, 2, 1,
		0, 3, 2
	};

	

	//  TEXTURE LOADING FOR FLOOR
	
	int widthFloor, heightFloor, numColFloor;
	unsigned char* floorBytes = stbi_load("texture/backwall1.bmp", &widthFloor, &heightFloor, &numColFloor, 0);

	if (!floorBytes)
	{
		fprintf(stderr, "Failed to load floor texture.\n");
		// You can add more details about the error using stbi_failure_reason()
		fprintf(stderr, "Reason: %s\n", stbi_failure_reason());
		getchar();  // Pause the program so you can see the error message
		glfwTerminate();
		return -1;
	}
	GLuint texture;

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);



	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthFloor, heightFloor, 0, GL_RGB, GL_UNSIGNED_BYTE, floorBytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	//stbi_image_free(floorBytes);
	//glBindTexture(GL_TEXTURE_2D, 0);

	////////////////// backwall ////////////////////////////////////

	int widthwall, heightwall, numColwall;
	unsigned char* wallBytes = stbi_load("texture/floor.bmp", &widthFloor, &heightFloor, &numColFloor, 0);

	GLuint texturewall;

	glGenTextures(1, &texturewall);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturewall);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthFloor, heightFloor, 0, GL_RGB, GL_UNSIGNED_BYTE, wallBytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	/////////////////////////////////////////////////////////////////

	

	



	///////////////////////////////////////
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);


	///////////// TEXTURE STUFF ////////////////////
	
	GLuint textureFloor;
	glGenBuffers(1, &textureFloor);
	glBindBuffer(GL_ARRAY_BUFFER, textureFloor);
	glBufferData(GL_ARRAY_BUFFER,sizeof(verticesFloor),verticesFloor,GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesEnv), indicesEnv, GL_STATIC_DRAW);
	
	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	float step = 0.0f;
	int firstrender = 0;
	int counter = 0;
	bool move = 0;//

	int flag = 1;

	glm::vec3 translations[4] = {
	glm::vec3(0.0f, -1.8f, 1.0f),
	glm::vec3(1.8f, 0.0f, 1.0f),
	glm::vec3(0.0f, 1.8f, 1.0f),
	glm::vec3(-1.8f, 0.0f, 1.0f)
	};

	glm::vec3 cubetranslations[4] =
	{
	glm::vec3(0.0f, -5.8f, 1.0f),
	glm::vec3(5.8f, 0.0f, 1.0f),
	glm::vec3(0.0f, 5.8f, 1.0f),
	glm::vec3(-5.8f, 0.0f, 1.0f)
	};

	glm::vec3 rotationAxis[4] =
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	};

	float dirx = 1.0f;
	float diry = 1.0f;
	float dirz = 1.0f;

	do {

		if (counter%100==0)
		{
			float randomFloat = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (60.0f - 10.0f)));
			glUniform1f(light, randomFloat);
		}

		counter++;
		//printf("%d\n", counter);
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0)
		{ 
			//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();


		////// Start of the rendering of the first object //////

		// Use our shader
		glUseProgram(programID);


		

		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);


		float angles[4] = { 90.0f,90.0f,90.0f,90.0f };



		
		///////////// TEXTURE STUFF ////////////////////
		glm::mat4 MVP;
		// Enable the attributes for the rectangle
		glm::mat4 ModelMatrix = glm::mat4(1.0);


		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);//
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);//

		glBindBuffer(GL_ARRAY_BUFFER, textureFloor);
		glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(vertexUVID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(TextureID, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		




		////////////////////////// WALL TEXTURE//////////////////////////////////

		
		// Enable the attributes for the rectangle
		

		glm::mat4 ModelMatrix1_rot = glm::mat4(1.0);//
		ModelMatrix1_rot = glm::rotate(ModelMatrix1_rot, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));//

		glm::mat4 ModelMatrix1_trans = glm::mat4(1.0); 
		ModelMatrix1_trans = glm::translate(ModelMatrix1_trans, glm::vec3(-30.0f, 0.0f, 5.0f));

		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix1_trans*ModelMatrix1_rot;


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);//
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1_rot[0][0]);//
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1_trans[0][0]);//

		glBindBuffer(GL_ARRAY_BUFFER, textureFloor);
		glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(vertexUVID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturewall);
		glUniform1i(TextureID, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);


		//////////////////////////////////////////////////////////

		
		//#pragma omp parallel for num_threads(4)	 
		for (int i = 0;i < 4;i++)
		{ 
			float angleInRadians_1 = glm::radians(angles[i]); //
			glm::mat4 ModelMatrix1_rot = glm::mat4(1.0);//
			ModelMatrix1_rot = glm::rotate(ModelMatrix1_rot, angleInRadians_1, rotationAxis[i]);//
			glm::mat4 MVP1;
			glm::mat4 ModelMatrix1_trans = glm::mat4(1.0); //
			ModelMatrix1_trans = glm::translate(ModelMatrix1_trans, translations[i]);
				

			if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
			{
				printf("G key pressed, Starting movement!\n");
				move = 1;
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				move = 0;
			}

			if (firstrender<=4 || (move==0))
			{
				if (i == 0)
				{
					//printf("translation %f %f %f\n", translations[i].x, translations[i].y, translations[i].z);
					MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1_trans * ModelMatrix1_rot; //
				}
				if (i == 1)
				{
					float angleInRadians_1 = glm::radians(90.0f); //
					glm::mat4 ModelMatrix1_rot_1 = glm::mat4(1.0);//
					ModelMatrix1_rot_1 = glm::rotate(ModelMatrix1_rot_1, angleInRadians_1, glm::vec3(0.0f, 0.0f, 1.0f));//
					
					MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1_trans * ModelMatrix1_rot_1 * ModelMatrix1_rot;//*ModelMatrix1_rot_1 ; //	
				}
				if (i == 2)
				{
					float angleInRadians_1 = glm::radians(180.0f); //
					glm::mat4 ModelMatrix1_rot_2 = glm::mat4(1.0);//
					ModelMatrix1_rot_2 = glm::rotate(ModelMatrix1_rot_2, angleInRadians_1, glm::vec3(0.0f, 0.0f, 1.0f));//

					MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1_trans * ModelMatrix1_rot_2 * ModelMatrix1_rot;//
				}

				if (i == 3)
				{
					float angleInRadians_1 = glm::radians(-90.0f); //
					glm::mat4 ModelMatrix1_rot_2 = glm::mat4(1.0);//
					ModelMatrix1_rot_2 = glm::rotate(ModelMatrix1_rot_2, angleInRadians_1, glm::vec3(0.0f, 0.0f, 1.0f));//

					MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1_trans * ModelMatrix1_rot_2 * ModelMatrix1_rot;//
				}
				firstrender++;
			
			}
			
			
			else if(firstrender>4 && move ==1)
			{

				if (i == 0)
				{
					if (counter % 2500 == 0)
					{
					
						dirx =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2) - 1.0f;
						diry =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2) - 1.0f;
						dirz =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
						
					}
					step = (rand() % 500) / 10000000.0f + 0.0001f;
					translations[i] += glm::vec3(dirx * step, diry * step, dirz * step);

					
					//printf("Angle= %f\n", (rand() % 500) / 10000.0f + 0.01f);

					//printf("translation %f %f %f\n", translations[i].x, translations[i].y, translations[i].z);
					//translations[i] = translations[i] + glm::vec3(-1 * step, step, step);
					ModelMatrix1_trans = glm::translate(ModelMatrix1_trans, translations[i]);
					
					
					MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1_trans * ModelMatrix1_rot; //
				}
				if (i == 1)
				{
					
					if (counter%1500 ==0)
					{
						
						dirx =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2) - 1.0f;
						diry =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2) - 1.0f;
						dirz =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
						
					}
					step = (rand() % 500) / 10000000.0f + 0.0001f;
					translations[i] += glm::vec3(dirx * step, diry* step, dirz * step);
					//printf("Object Location: (%f, %f, %f)\n", translations[i].x, translations[i].y, translations[i].z);
					glm::mat4 ModelMatrix1_rot_1 = glm::mat4(1.0);
					ModelMatrix1_rot_1 = glm::rotate(ModelMatrix1_rot_1, angleInRadians_1, glm::vec3(0.0f, 0.0f, 1.0f));
					
					ModelMatrix1_trans = glm::translate(ModelMatrix1_trans, translations[i]);
					MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1_trans * ModelMatrix1_rot_1 * ModelMatrix1_rot;//*ModelMatrix1_rot_1 ; //	
				}
				if (i == 2)
				{
					if (counter % 2000 == 0)
					{
						
						dirx =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2) - 1.0f;
						diry =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2) - 1.0f;
						dirz =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
					}
					step = (rand() % 100) / 10000000.0f + 0.0001f;
					translations[i] += glm::vec3(dirx * step, diry * step, dirz * step);

					float angleInRadians_1 = glm::radians(120.0f); //
					glm::mat4 ModelMatrix1_rot_2 = glm::mat4(1.0);//

					
					ModelMatrix1_trans = glm::translate(ModelMatrix1_trans, translations[i]);
					ModelMatrix1_rot_2 = glm::rotate(ModelMatrix1_rot_2, angleInRadians_1, glm::vec3(0.0f, 0.0f, 1.0f));//

					MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1_trans * ModelMatrix1_rot_2 * ModelMatrix1_rot;//
				}

				if (i == 3)
				{
					if (counter % 3000 == 0)
					{
						
						dirx =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2) - 1.0f;
						diry =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2) - 1.0f;
						dirz =  static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
						
					}
					step = (rand() % 800) / 1000000.0f + 0.0001f;
					translations[i] += glm::vec3(dirx * step, diry * step, dirz * step);

					float angleInRadians_1 = glm::radians(-80.0f); //
					glm::mat4 ModelMatrix1_rot_2 = glm::mat4(1.0);//
					ModelMatrix1_rot_2 = glm::rotate(ModelMatrix1_rot_2, angleInRadians_1, glm::vec3(0.0f, 0.0f, 1.0f));//
					
					ModelMatrix1_trans = glm::translate(ModelMatrix1_trans, translations[i]);
					MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1_trans * ModelMatrix1_rot_2 * ModelMatrix1_rot;//
				}


			}

			
			//printf("First render: %d\n",firstrender);

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);//
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1_rot[0][0]);//
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1_trans[0][0]);//

			glEnableVertexAttribArray(vertexPosition_modelspaceID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);//
			glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//

			glEnableVertexAttribArray(vertexUVID);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);//
			glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);//

			glEnableVertexAttribArray(vertexNormal_modelspaceID);
			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);//
			glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);//

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);//

		}

		
	

		////// End of rendering of the first object //////
		
		//glUniform1f(light, 50.0f);
		/////////////// SECOND OBJECT ////////////////////////
		glm::mat4 cubeModelMatrix = glm::mat4(1.0);
		// Set the cube's translation, rotation, etc.

		glm::mat4 cubeMVP = ProjectionMatrix * ViewMatrix * cubeModelMatrix;

		for (int j = 0;j < 4; j++)
		{
			float angleInRadians_1 = glm::radians(angles[j]); //
			glm::mat4 ModelMatrix1_rot = glm::mat4(1.0);//
			ModelMatrix1_rot = glm::rotate(ModelMatrix1_rot, angleInRadians_1, rotationAxis[j]);//
			glm::mat4 MVP1;
			glm::mat4 CubeModelMatrix1_trans = glm::mat4(1.0); //
			CubeModelMatrix1_trans = glm::translate(CubeModelMatrix1_trans, cubetranslations[j]);

			if (j == 0)
			{
				//printf("translation %f %f %f\n", translations[i].x, translations[i].y, translations[i].z);
				cubeMVP = ProjectionMatrix * ViewMatrix * CubeModelMatrix1_trans * ModelMatrix1_rot; //
			}
			if (j == 1)
			{
				float angleInRadians_1 = glm::radians(90.0f); //
				glm::mat4 ModelMatrix1_rot_1 = glm::mat4(1.0);//
				ModelMatrix1_rot_1 = glm::rotate(ModelMatrix1_rot_1, angleInRadians_1, glm::vec3(0.0f, 0.0f, 1.0f));//

				cubeMVP = ProjectionMatrix * ViewMatrix * CubeModelMatrix1_trans * ModelMatrix1_rot_1 * ModelMatrix1_rot;//*ModelMatrix1_rot_1 ; //	
			}
			if (j == 2)
			{
				float angleInRadians_1 = glm::radians(180.0f); //
				glm::mat4 ModelMatrix1_rot_2 = glm::mat4(1.0);//
				ModelMatrix1_rot_2 = glm::rotate(ModelMatrix1_rot_2, angleInRadians_1, glm::vec3(0.0f, 0.0f, 1.0f));//

				cubeMVP = ProjectionMatrix * ViewMatrix * CubeModelMatrix1_trans * ModelMatrix1_rot_2 * ModelMatrix1_rot;//
			}

			if (j == 3)
			{
				float angleInRadians_1 = glm::radians(-90.0f); //
				glm::mat4 ModelMatrix1_rot_2 = glm::mat4(1.0);//
				ModelMatrix1_rot_2 = glm::rotate(ModelMatrix1_rot_2, angleInRadians_1, glm::vec3(0.0f, 0.0f, 1.0f));//

				cubeMVP = ProjectionMatrix * ViewMatrix * CubeModelMatrix1_trans * ModelMatrix1_rot_2 * ModelMatrix1_rot;//
			}
			

		


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &cubeMVP[0][0]);  


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);


		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &cubeModelMatrix[0][0]);//

		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);//
		glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//

		glEnableVertexAttribArray(vertexUVID);
		glBindBuffer(GL_ARRAY_BUFFER, cubeUvBuffer);//
		glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);//

		glEnableVertexAttribArray(vertexNormal_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, cubeNormalBuffer);//
		glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeElementBuffer);
		glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_SHORT, 0);

		}


		////////////////////////////////////////////////////////

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);
		glDisableVertexAttribArray(vertexNormal_modelspaceID);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	

	// Cleanup VBO and shader
	printf("CLearing data!");
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteBuffers(1, &textureFloor);
	glDeleteBuffers(1, &ebo);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteTextures(1, &texture);




	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

