#pragma once

//#include "Renderer.h"

typedef struct VertexBuffer
{
	unsigned int renderer_id;   // ID que corresponde a OpenGL
	unsigned int size;
	void* data;
} vertex_buffer;

vertex_buffer init_vertex_buffer(void* data, unsigned int size, vertex_buffer* vb)
{
	glGenBuffers(1, &(vb->renderer_id));
	glBindBuffer(GL_ARRAY_BUFFER, vb->renderer_id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	vb->size = size;
	vb->data = data;

	return *vb;
}

// Esta funcion permite reservar la memoria en la GPU para enviar datos a ella despues
vertex_buffer init_dynamic_vertex_buffer(unsigned int size, vertex_buffer* vb)
{
	glGenBuffers(1, &(vb->renderer_id));
	glBindBuffer(GL_ARRAY_BUFFER, vb->renderer_id);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);

	vb->size = size;
	vb->data = NULL;

	return *vb;
}

void delete_vertex_buffer(vertex_buffer* vb)
{
	glDeleteBuffers(1, &(vb->renderer_id));
}

void bind_vertex_buffer(vertex_buffer* vb)
{
	glBindBuffer(GL_ARRAY_BUFFER, vb->renderer_id);
}

void unbind_vertex_buffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}