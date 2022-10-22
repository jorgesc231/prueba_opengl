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


struct TestBatchTextureState
{
    glm::vec3 translation;

    vertex_array VAO;
    vertex_buffer VertexBuffer;
    index_buffer IndexBuffer;
    Shader shader;

    Texture texture1;
    Texture texture2;

    glm::mat4 proj;
    glm::mat4 view;;
};

void batch_texture_delete_test(void* state)
{
    ; // TODO: Deberia borrar la memoria cuando se vuelve al menu
}

void batch_texture_on_update(float dt)
{
    ;
}

void batch_texture_render(void* state)
{
    TestBatchTextureState* prueba = (TestBatchTextureState*)state;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    bind_texture(&prueba->texture1, 0);
    bind_texture(&prueba->texture2, 1);


    glm::mat4 model = glm::translate(glm::mat4(1.0f), prueba->translation);
    glm::mat4 mvp = prueba->proj * prueba->view * model;
    bind_shader(&prueba->shader);
    set_uniform_mat4f(&prueba->shader, "u_MVP", mvp);

    renderer_draw(&prueba->VAO, &prueba->IndexBuffer, &prueba->shader);
}

void batch_texture_imgui_render(void* state)
{
    TestBatchTextureState* prueba = (TestBatchTextureState*)state;

    ImGui::SliderFloat3("Translation", &prueba->translation.x, -4.0f, 4.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

// Retorna un puntero al estado del test almacenado en el heap. Tiene que ser borrado con delete_test_clear_color
void* create_test_batch_texture(Test* current_test)
{
    TestBatchTextureState* test_state = (TestBatchTextureState*)malloc(sizeof(TestBatchTextureState));

    if (test_state)
    {
        test_state->translation = glm::vec3(0.0f, 0.0f, 0.0f);

        // 2 * Vertex positions, 2 * texture coord, 4 * color, 1 * texture index
        float positions[] = {
           -0.5, -0.5, 0.0f, 0.0f,  0.18f, 0.6f, 0.96f, 1.0f,  0.0f,   // 0
            0.5, -0.5, 1.0f, 0.0f,  0.18f, 0.6f, 0.96f, 1.0f,  0.0f,   // 1
            0.5,  0.5, 1.0f, 1.0f,  0.18f, 0.6f, 0.96f, 1.0f,  0.0f,   // 2
           -0.5,  0.5, 0.0f, 1.0f,  0.18f, 0.6f, 0.96f, 1.0f,  0.0f,   // 3

            0.5, -0.5, 0.0f, 0.0f,  1.0f, 0.93f, 0.24f, 1.0f,  1.0f,   // 4
            1.5, -0.5, 1.0f, 0.0f,  1.0f, 0.93f, 0.24f, 1.0f,  1.0f,   // 5
            1.5,  0.5, 1.0f, 1.0f,  1.0f, 0.93f, 0.24f, 1.0f,  1.0f,   // 6
            0.5,  0.5, 0.0f, 1.0f,  1.0f, 0.93f, 0.24f, 1.0f,  1.0f    // 7
        };

        // Index Buffer
        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,  // Quad 1
            4, 5, 6, 6, 7, 4
        };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Inicializa el vertex array
        init_vertex_array(&test_state->VAO);

        // Inicializa el vertex buffer
        init_vertex_buffer(positions, 9 * 8 * sizeof(float), &test_state->VertexBuffer);

        vertex_buffer_layout layout;
        init_vertex_buffer_layout(&layout);
        push_element_on_layout(&layout, 2, false, GL_FLOAT);
        push_element_on_layout(&layout, 2, false, GL_FLOAT);
        push_element_on_layout(&layout, 4, false, GL_FLOAT);
        push_element_on_layout(&layout, 1, false, GL_FLOAT);

        add_buffer(&test_state->VAO, &test_state->VertexBuffer, &layout);

        // Inicializa el idex buffer
        init_index_buffer(indices, 12, &test_state->IndexBuffer);

        test_state->proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        test_state->view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));


        init_shader(&test_state->shader, "res/shaders/batch_texture.shader");
        bind_shader(&test_state->shader);


        init_texture(&test_state->texture1, "res/textures/tank_green.png");
        init_texture(&test_state->texture2, "res/textures/tank_blue.png");

        int samplers[2] = {0, 1};
        set_uniform_1iv(&test_state->shader, "u_Textures", 2, samplers);


        current_test->on_update = batch_texture_on_update;
        current_test->on_render = batch_texture_render;
        current_test->on_imgui_render = batch_texture_imgui_render;
        current_test->delete_test = batch_texture_delete_test;

        return (void*)test_state;
    }
    else {
        return NULL;
    }
}