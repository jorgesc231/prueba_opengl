#pragma once

#define ASSERT(x) if(!(x)) __debugbreak();

typedef struct IndexBuffer
{
	unsigned int renderer_id;   // ID que corresponde a OpenGL
	unsigned int count;         // Cantidad de indices, podria usarse short
	void* data;
} index_buffer;

bool init_index_buffer(unsigned int* data, unsigned int count, index_buffer* ib)
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	glGenBuffers(1, &(ib->renderer_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->renderer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);

	ib->count = count;
	ib->data = data;

	return true;
}

void delete_index_buffer(index_buffer* ib)
{
	glDeleteBuffers(1, &(ib->renderer_id));
	// TODO: Deberia liberar la memoria de data? (es solo un puntero para leer esos datos, no deberia)
}

void bind_index_buffer(index_buffer* ib)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->renderer_id);
}

void unbind_index_buffer()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}