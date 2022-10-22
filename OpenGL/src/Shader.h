#pragma once

#include <GL/glew.h>
#include <stdio.h>
#include <malloc.h>

#include <Windows.h>

#include "glm/glm.hpp"

#include "hash_table.h"

typedef struct _Shader
{
	unsigned int renderer_id;
	const char* file_path;
	
    // TODO: Caching for uniforms in a hash map
    ht_hash_table* uniform_location_cache;

} Shader;

typedef struct ShaderProgramSource
{
    char* file_buffer;
    char* vertex_source;
    char* fragment_source;
} shader_program_source;


shader_program_source parse_shader(const char* file_path)
{
    shader_program_source source = { 0 };

    HANDLE file = CreateFileA((LPCSTR)file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (file == INVALID_HANDLE_VALUE) exit(-1);

    LARGE_INTEGER file_size;
    DWORD bytes_read = 0;

    if (!GetFileSizeEx(file, &file_size)) exit(-1);

    if (source.file_buffer == NULL)
    {
        source.file_buffer = (char*)malloc(sizeof(char) * file_size.QuadPart);
    }

    if (ReadFile(file, source.file_buffer, file_size.QuadPart, &bytes_read, NULL) && file_size.QuadPart == bytes_read)
    {
        source.file_buffer[bytes_read] = '\0';     // Para que no lea mas alla del string

        // Revisa si existen los #shader
        source.vertex_source = strstr(source.file_buffer, "#shader vertex");
        if (source.vertex_source)
        {
            source.vertex_source += 14; // Para saltarse el #shader vertex
        }

        source.fragment_source = strstr(source.file_buffer, "#shader fragment");
        if (source.fragment_source)
        {
            *source.fragment_source = '\0';  // evita que el puntero a vertex siga hasta el final del archivo
            source.fragment_source += 16;    // Para saltarse el #shader fragment
        }
    }

    CloseHandle(file);

    return source;
}




unsigned int compile_shader(unsigned int type, const char* source)
{
    unsigned int id = glCreateShader(type);

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        if (type == GL_FRAGMENT_SHADER)
            printf("Failed to compile Fragment Shader\n");
        else printf("Failed to compile Vertex Shader\n");

        printf("%s\n", message);

        glDeleteShader(id);

        return 0;
    }

    return id;
}

// Toma como argumentos el codigo de los shaders
unsigned int create_shader(const char* vertex, const char* fragment)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex);
    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}



void init_shader(Shader* shader, const char* filepath)
{
	shader->file_path = filepath;

    shader_program_source source = parse_shader(filepath);
    shader->renderer_id = create_shader(source.vertex_source, source.fragment_source);

    // Inicializa la hash table
    shader->uniform_location_cache = ht_new();

}

void delete_shader(Shader* shader)
{
    glDeleteProgram(shader->renderer_id);
    ht_del_hash_table(shader->uniform_location_cache);   // Elimina la hash table con su memoria al borrar el shader
}

void bind_shader(Shader* shader)
{
    glUseProgram(shader->renderer_id);
}

void unbind_shader(Shader* shader)
{
    glUseProgram(0);
}

// Set uniforms

int get_uniform_location(Shader* shader, const char* name)
{
    int location = -1;

    // Si esta en la hash table
    location = ht_search(shader->uniform_location_cache, name);
    if (location > -1)
        return location;

    // Si no esta lo agrega
    location = glGetUniformLocation(shader->renderer_id, name);
    ht_insert(shader->uniform_location_cache, name, location);    // Agrega la location a la hash table
    
    if (location == -1)
    {
        printf("Warning: Uniform %s doesn't exist!\n", name);
    }

    return location;
}


void set_uniform_4f(Shader* shader, const char* name, float v0, float v1, float v2, float v3)
{
    glUniform4f(get_uniform_location(shader, name), v0, v1, v2, v3);
}

void set_uniform_mat4f(Shader* shader, const char* name, const glm::mat4 matrix)
{
    glUniformMatrix4fv(get_uniform_location(shader, name), 1, GL_FALSE, &matrix[0][0]);
    // glm::value_ptr()
}

void set_uniform_1iv(Shader* shader, const char* name, int count, int* value)
{
    glUniform1iv(get_uniform_location(shader, name), count, value);
}

void set_uniform_1i(Shader* shader, const char* name, int value)
{
    glUniform1i(get_uniform_location(shader, name), value);
}
