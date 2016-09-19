/*
Title: Fragment Shaders
File Name: main.cpp
Copyright � 2016
Author: David Erbelding
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include "../header/shape.h"
#include "../header/transform2d.h"
#include "../header/shader.h"
#include <iostream>

Shape* square;

// The transform being used to draw our shape
Transform2D transform;

// Shaders.
Shader vertexShader;
Shader fragmentShader;

// GL index for the shader program
GLuint shaderProgram;

// The index of our world matrix within the shader program.
GLuint uniformLocation;

// Window resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}




int main(int argc, char **argv)
{
	// Initializes the GLFW library
	glfwInit();

	// Initialize window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Fragment Shaders", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	//set resize callback
	glfwSetFramebufferSizeCallback(window, resizeCallback);

	// Initializes the glew library
	glewInit();


	// Indices for square (-1, -1)[2] to (1, 1)[1]
	// [0]------[1]
	//	|		 |
	//	|		 |
	//	|		 |
	// [2]------[3]

	// Create square vertex data.
	std::vector<Vertex2dColor> vertices;
	// Let's create some vertices with color!
	vertices.push_back(
		Vertex2dColor(
			glm::vec2(-1, 1), // top left
			glm::vec4(1, 0, 0, 1) // red
			)
		);
	vertices.push_back(
		Vertex2dColor(
			glm::vec2(1, 1), // top right
			glm::vec4(0, 0, 1, 1) // blue
			)
		);
	vertices.push_back(
		Vertex2dColor(
			glm::vec2(-1, -1), // bottom left
			glm::vec4(0, 0, 1, 1) // blue
			)
		);
	vertices.push_back(
		Vertex2dColor(
			glm::vec2(1, -1), // bottom right
			glm::vec4(1, 0, 0, 1) // red
			)
		);

	// Our index buffer won't have to change!
	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(1);


	// Create shape object
	square = new Shape(vertices, indices);
	

	transform.SetScale(.25f);
	transform.SetPosition(glm::vec2(.25, .25));

	// SHADER CODE

	std::string vertexShaderCode =
		"#version 400 core \n"

		// Vertex attributes for position and color
		"layout(location = 0) in vec2 in_position;"
		"layout(location = 1) in vec4 in_color;"

		// uniform will contain the world matrix.
		"uniform mat3 worldMatrix;"

		// we can output extra data from the vertex shader with an out variable.
		"out vec4 color;"

		"void main(void)"
		"{"
			//transform the vector
			"vec3 transformed = worldMatrix * vec3(in_position, 1);"

			// output the transformed vector
			"gl_Position = vec4(transformed, 1);"

			// just send the same color through to the next step
			"color = in_color;"
		"}";


	std::string fragmentShaderCode =
		"#version 400 core \n"

		// The that we set in the vertex step comes in here!
		// Warning! This variable has to have the same name as the out variable in the vertex shader.
		"in vec4 color;"

		"void main(void)"
		"{"
			// Instead of returning a hard coded color, we can use the color given by the vertex step!
			"gl_FragColor = color;"
		"}";

	// Initialize the shaders using the shader files. (the strings from above also work)
	//vertexShader.InitFromString(vertexShaderCode, GL_VERTEX_SHADER);
	vertexShader.InitFromFile("../shaders/vertex.glsl", GL_VERTEX_SHADER);

	//fragmentShader.InitFromString(fragmentShaderCode, GL_FRAGMENT_SHADER);
	fragmentShader.InitFromFile("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);





	// Create a shader program.
	shaderProgram = glCreateProgram();
	
	// Attach the vertex and fragment shaders to our program.
	vertexShader.AttachTo(shaderProgram);
	fragmentShader.AttachTo(shaderProgram);

	// Tells gl to set up the connections between the shaders we have attached.
	// After this we should be ready to roll.
	glLinkProgram(shaderProgram);

	// After the program has been linked, we can ask it where it put our worldMatrix.
	// (Since there's only one uniform between our two shaders, ours should always end up at index 0)
	uniformLocation = glGetUniformLocation(shaderProgram, "worldMatrix");



	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
        // Calculate delta time.
        float dt = glfwGetTime();
        // Reset the timer.
        glfwSetTime(0);

		// Clear the screen.
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.0, 0.0);


		// rotate square
		transform.Rotate(dt);
		
		// Set the current shader program.
		glUseProgram(shaderProgram);

		
		// The drawing code in our shape class has to change significantly now.
		// Previously we had been multiplying each vertex manually, now we will let the gpu handle the dirty work.
		// We also need to give it the unifrom location for the world Matrix so it can send the matrix.
		square->Draw(transform.GetMatrix(), uniformLocation);



		// Stop using the shader program.
		glUseProgram(0);

		// Swap the backbuffer to the front.
		glfwSwapBuffers(window);

		// Poll input and window events.
		glfwPollEvents();

	}

	// Free memory from shader program and individual shaders
	glDeleteProgram(shaderProgram);


	// Free memory from shape object
	delete square;

	// Free GLFW memory.
	glfwTerminate();


	// End of Program.
	return 0;
}
