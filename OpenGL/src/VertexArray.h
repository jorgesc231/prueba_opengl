#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

typedef struct VertexArray
{
	unsigned int renderer_id;

} vertex_array;

void init_vertex_array(vertex_array* array)
{
	glGenVertexArrays(1, &(array->renderer_id));
	glBindVertexArray(array->renderer_id);
}

void delete_vertex_array(vertex_array* array)
{
	glDeleteVertexArrays(1, &(array->renderer_id));
}

void bind_vertex_array(vertex_array* array)
{
	glBindVertexArray(array->renderer_id);
}

void unbind_vertex_array(vertex_array* array)
{
	glBindVertexArray(0);
}

void add_buffer(vertex_array* array, vertex_buffer* vb, vertex_buffer_layout* layout)
{
	bind_vertex_array(array);
	bind_vertex_buffer(vb);

	vertex_buffer_element* elements = layout->elements;

	unsigned int offset = 0;

	for (int i = 0; i < layout->elements_size; i++)
	{
		vertex_buffer_element* element = &elements[i];

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element->count, element->type, element->normalized, layout->stride, (void*)offset);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

		offset += element->count * get_size_of_type(element->type);
	}
}