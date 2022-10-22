#pragma once

#include "Test.h"

#include <GL/glew.h>
#include <imgui/imgui.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"


struct TestBatchDynamicState
{
    float quad_position1[2];
    float quad_position2[2];

    vertex_array VAO;
    vertex_buffer VertexBuffer;
    index_buffer IndexBuffer;
    Shader shader;

    Texture texture1;
    Texture texture2;

    glm::mat4 proj;
    glm::mat4 view;;
};

struct Vec2
{
    float x, y;
};

struct Vec3
{
    float x, y, z;
};

struct Vec4
{
    float x, y, z, w;
};


struct Vertex
{
    Vec3 Position;
    Vec4 Color;
    Vec2 TexCoord;
    float TexID;
};

struct Quad
{
    Vertex vert1;
    Vertex vert2;
    Vertex vert3;
    Vertex vert4;
};

void batch_dynamic_delete_test(void* state)
{
    ; // TODO: Deberia borrar la memoria cuando se vuelve al menu
}

void batch_dynamic_on_update(float dt)
{
    ;
}

// Funcion que agrega 4 vertices al buffer target para hacer un nuevo Quad
// Retorna la posicion en el buffer en el que esta
Vertex* create_quad(Vertex* target, float x, float y, float textureID)
{
    float size = 1.0f;   // Quad de 1 x 1

    target->Position = { x, y, 0.0f };
    target->Color = { 0.18f, 0.6f, 0.96f, 1.0f };
    target->TexCoord = { 0.0f, 0.0f };
    target->TexID = textureID;
    target++;

    target->Position = { x + size, y, 0.0f };
    target->Color = { 0.18f, 0.6f, 0.96f, 1.0f };
    target->TexCoord = { 1.0f, 0.0f };
    target->TexID = textureID;
    target++;

    target->Position = { x + size, y + size,  0.0f };
    target->Color = { 0.18f, 0.6f, 0.96f, 1.0f };
    target->TexCoord = { 1.0f, 1.0f };
    target->TexID = textureID;
    target++;

    target->Position = { x, y + size, 0.0f };
    target->Color = { 0.18f, 0.6f, 0.96f, 1.0f };
    target->TexCoord = { 0.0f, 1.0f };
    target->TexID = textureID;
    target++;

    return target;
}


void batch_dynamic_render(void* state)
{
    TestBatchDynamicState* prueba = (TestBatchDynamicState*)state;

    // 3 * Vertex positions, 4 * color, 2 * texture coord, 1 * texture index
    /*float vertices[] = {
        -0.5, -0.5, 0.0f,  0.18f, 0.6f, 0.96f, 1.0f,  0.0f, 0.0f,  0.0f,   // 0
         0.5, -0.5, 0.0f,  0.18f, 0.6f, 0.96f, 1.0f,  1.0f, 0.0f,  0.0f,   // 1
         0.5, 0.5,  0.0f,  0.18f, 0.6f, 0.96f, 1.0f,  1.0f, 1.0f,  0.0f,   // 2
        -0.5,  0.5, 0.0f,  0.18f, 0.6f, 0.96f, 1.0f,  0.0f, 1.0f,  0.0f,   // 3

        0.5, -0.5, 0.0f,  1.0f, 0.93f, 0.24f, 1.0f,  0.0f, 0.0f,  1.0f,   // 4
        1.5, -0.5, 0.0f,  1.0f, 0.93f, 0.24f, 1.0f,  1.0f, 0.0f,  1.0f,   // 5
        1.5,  0.5, 0.0f,  1.0f, 0.93f, 0.24f, 1.0f,  1.0f, 1.0f,  1.0f,   // 6
        0.5,  0.5, 0.0f,  1.0f, 0.93f, 0.24f, 1.0f,  0.0f, 1.0f,  1.0f    // 7
    };*/


    uint32_t indexCount = 0;
    uint32_t vertexCount = 0;

    Vertex vertices[1000];
    Vertex* buffer = vertices;

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            buffer = create_quad(buffer, x, y, (x + y) % 2);
            indexCount += 6;
            vertexCount += 4;
        }
    }

    buffer = create_quad(buffer, prueba->quad_position1[0], prueba->quad_position1[1], 0.0f);
    indexCount += 6;
    vertexCount += 4;


    // Esto ya no es necesario porque los datos ya estan en el array vertices
    //Vertex vertices[8];
    //memcpy(vertices, (void *)&q0.vert1, sizeof(Vertex) * 4);
    //memcpy(vertices + 4, (void*)&q1.vert1, sizeof(Vertex) * 4);



    // Set dynamic vertex buffer
    bind_vertex_array(&prueba->VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), vertices);



    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    bind_texture(&prueba->texture1, 0);
    bind_texture(&prueba->texture2, 1);


    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = prueba->proj * prueba->view * model;
    bind_shader(&prueba->shader);
    set_uniform_mat4f(&prueba->shader, "u_MVP", mvp);

    prueba->IndexBuffer.count = indexCount;   // Esto deberia arreglarlo
    renderer_draw(&prueba->VAO, &prueba->IndexBuffer, &prueba->shader);
}

void batch_dynamic_imgui_render(void* state)
{
    TestBatchDynamicState* prueba = (TestBatchDynamicState*)state;

    ImGui::DragFloat2("Quad 1 Position", prueba->quad_position1, 0.1f);


    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

// Retorna un puntero al estado del test almacenado en el heap. Tiene que ser borrado con delete_test_clear_color
void* create_test_batch_dynamic(Test* current_test)
{
    TestBatchDynamicState* test_state = (TestBatchDynamicState*)malloc(sizeof(TestBatchDynamicState));

    if (test_state)
    {
        test_state->quad_position1[0] = -1.5f;
        test_state->quad_position1[1] = -0.5f;
        
        test_state->quad_position2[0] = 0.5f;
        test_state->quad_position2[1] = -0.5f;


        init_shader(&test_state->shader, "res/shaders/batch_dynamic.shader");
        bind_shader(&test_state->shader);

        int samplers[2] = { 0, 1 };
        set_uniform_1iv(&test_state->shader, "u_Textures", 2, samplers);

        // Sirve para saber el tamano del vertex buffer e index buffer de antemano
        const size_t MaxQuadCount = 1000;   // Cantidad maxima de quad que quiero dibujar por drawcall
        const size_t MaxVertexCount = MaxQuadCount * 4;
        const size_t MaxIndexCount = MaxQuadCount * 6;

        // Index Buffer
        //unsigned int indices[] = {
        //    0, 1, 2, 2, 3, 0,  // Quad 1
        //    4, 5, 6, 6, 7, 4
        //};

        // Crea los indices de forma automatica
        uint32_t indices[MaxIndexCount];
        uint32_t offset = 0;
        
        for (size_t i = 0; i < MaxIndexCount; i += 6)
        {
            indices[i + 0] = 0 + offset;
            indices[i + 1] = 1 + offset;
            indices[i + 2] = 2 + offset;

            indices[i + 3] = 2 + offset;
            indices[i + 4] = 3 + offset;
            indices[i + 5] = 0 + offset;

            offset += 4;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Inicializa el vertex array
        init_vertex_array(&test_state->VAO);

        // Inicializa el vertex buffer
        init_dynamic_vertex_buffer(MaxVertexCount * sizeof(Vertex), &test_state->VertexBuffer);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord));
        
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexID));

        // Inicializa el idex buffer
        init_index_buffer(indices, MaxIndexCount, &test_state->IndexBuffer);

        test_state->proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        test_state->view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));


        init_texture(&test_state->texture1, "res/textures/tank_green.png");
        init_texture(&test_state->texture2, "res/textures/tank_blue.png");


        current_test->on_update = batch_dynamic_on_update;
        current_test->on_render = batch_dynamic_render;
        current_test->on_imgui_render = batch_dynamic_imgui_render;
        current_test->delete_test = batch_dynamic_delete_test;

        return (void*)test_state;
    }
    else {
        return NULL;
    }
}
