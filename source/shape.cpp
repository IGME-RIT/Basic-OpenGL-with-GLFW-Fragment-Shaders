/*
Title: Fragment Shaders
File Name: shape.cpp
Copyright ? 2016
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

#include "../header/shape.h"

Shape::Shape(std::vector<Vertex2dColor> vertices, std::vector<unsigned int> indices)
{
	m_vertices = vertices;
	m_indices = indices;
	// Create the shape by setting up buffers

	// Set up vertex buffer.
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

	// Copy the vector into the buffer.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2dColor), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Set up index buffer same as above.
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Shape::~Shape()
{
	// Clear buffers for the shape object when done using them.
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
}



void Shape::Draw(glm::mat3 worldMatrix, GLuint uniformLocation)
{
	// Previously, we multiplied each vertex one by one, but now we just have to send the world matrix to the gpu.


	// Bind the vertex buffer and set the Vertex Attribute.
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

	// Now when we set Vertex Attribues, we need to set 2 of them.
	// The vertex position will be in layout location 0.
	// Each one is 2 floats.
	// It is NOT normalized.
	// The distance between each is the size of a whole Vertex2dColor.
	// The start location is at the beginning of the buffer.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2dColor), (void*)0);

	// The color will be in layout location 1.
	// Each on is 4 floats.
	// It's not normalized.
	// The distance between each is the size of a Vertex2dColor.
	// The start location is 2 floats past the beginning of the buffer, because the first 2 are positions.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2dColor), (void*)sizeof(glm::vec2));

	// Note:
	// In this example, the position and color data are interleaved.
	// It's also possible to store all position data, and then all color data, or even
	// store them in separate arrays, but in most cases, it's less efficient to do so.
	// By interleaving, we know that there are always an equal number of positions and colors.

	// Stop using the buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	// This function sets the uniform variable at the given LOCATION.
	// In this case it's a Matrix containing 3 vectors with 3 floats each.
	// 1 is the COUNT of matrices we are sending. (It's possible to send an entire array of matrices).
	// We are passing in FALSE into whether or not we want to transpose our matrices.
	// Last we give it a POINTER to the first float in our matrix.
	// It just interprets the data as an array of floats anyway.
	glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, &(worldMatrix[0][0]));




	// Bind index buffer to GL_ELEMENT_ARRAY_BUFFER, and enable vertex attribute
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Draw all indices in the index buffer
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0);

	// Disable vertex attribute and unbind index buffer.
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
