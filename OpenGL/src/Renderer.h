#pragma once

#include <GL/glew.h>
#include <stdio.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if(!(x)) __debugbreak();

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        printf("[OpenGL Error] (%d): %s %s:%d\n", error, function, file, line);
        return false;
    }

    return true;
}

typedef struct _Renderer
{
    ;
} Renderer;

void renderer_clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_draw(vertex_array* va, index_buffer* ib, Shader* shader)
{
    bind_shader(shader);
    bind_vertex_array(va);
    bind_index_buffer(ib);

    glDrawElements(GL_TRIANGLES, ib->count, GL_UNSIGNED_INT, NULL);

}
