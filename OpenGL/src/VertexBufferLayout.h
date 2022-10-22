#pragma once

#include <GL/glew.h>
//#include "Renderer.h"
#include <stdlib.h>

#define ASSERT(x) if(!(x)) __debugbreak();

typedef struct VertexBufferElement
{
	unsigned int count;
	unsigned int type;
	//unsigned char normalized;
	bool normalized;
} vertex_buffer_element;

typedef struct VertexBufferLayout
{
	vertex_buffer_element* elements;
	int elements_size;            // Cantidad de elements en el array
	unsigned int stride;
} vertex_buffer_layout;


unsigned int get_size_of_type(unsigned int type)
{
	switch (type)
	{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
	}

	ASSERT(false);

	return 0;
}


void init_vertex_buffer_layout(vertex_buffer_layout* layout)
{
	layout->elements = 0;
	layout->elements_size = 0;
	layout->stride = 0;
}

void push_element_on_layout(vertex_buffer_layout* layout, unsigned int count, bool normalized, GLenum type)
{
	// Agregar un elemento al array de elementos usando realloc
	layout->elements = (vertex_buffer_element*)realloc(layout->elements, sizeof(vertex_buffer_element) * (layout->elements_size + 1));

	// TODO: Comprobar errores de realloc y demas.

	vertex_buffer_element* element = &layout->elements[layout->elements_size];

	if (type != GL_UNSIGNED_BYTE)
	{
		element->normalized = normalized;
	}
	else {
		element->normalized = false;
	}
	
	element->type = type;
	element->count = count;


	layout->stride += element->count * get_size_of_type(type);
	layout->elements_size += 1;
}