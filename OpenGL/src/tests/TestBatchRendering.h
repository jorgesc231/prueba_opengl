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


struct TestBatchState
{
    glm::vec3 translation;

    vertex_array VAO;
    vertex_buffer VertexBuffer;
    index_buffer IndexBuffer;
    Shader shader;

    Texture texture;

    glm::mat4 proj;
    glm::mat4 view;;
};

void batch_delete_test(void* state)
{
    ; // TODO: Deberia borrar la memoria cuando se vuelve al menu
}

void batch_on_update(float dt)
{
    ;
}

void batch_render(void* state)
{
    TestBatchState* prueba = (TestBatchState*)state;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    bind_texture(&prueba->texture, 0);

    
    glm::mat4 model = glm::translate(glm::mat4(1.0f), prueba->translation);
    glm::mat4 mvp = prueba->proj * prueba->view * model;
    bind_shader(&prueba->shader);
    set_uniform_mat4f(&prueba->shader, "u_MVP", mvp);

    renderer_draw(&prueba->VAO, &prueba->IndexBuffer, &prueba->shader);
}

void batch_imgui_render(void* state)
{
    TestBatchState* prueba = (TestBatchState*)state;

    ImGui::SliderFloat3("Translation", &prueba->translation.x, -4.0f, 4.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

// Retorna un puntero al estado del test almacenado en el heap. Tiene que ser borrado con delete_test_clear_color
void* create_test_batch(Test* current_test)
{
    TestBatchState* test_state = (TestBatchState*)malloc(sizeof(TestBatchState));

    if (test_state)
    {
        test_state->translation = glm::vec3(0.0f, 0.0f, 0.0f);

        // Vertex positions
        float positions[] = {
            -0.5, -0.5, 0.0f, 0.0f,  // 0
             0.5, -0.5, 1.0f, 0.0f,  // 1
             0.5, 0.5,  1.0f, 1.0f,  // 2
             -0.5, 0.5, 0.0f, 1.0f,   // 3

            0.5, -0.5, 0.0f, 0.0f,  // 4
            1.5, -0.5, 1.0f, 0.0f,  // 5
            1.5,  0.5, 1.0f, 1.0f,  // 6
            0.5,  0.5, 0.0f, 1.0f   // 7
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
        init_vertex_buffer(positions, 4 * 8 * sizeof(float), &test_state->VertexBuffer);

        vertex_buffer_layout layout;
        init_vertex_buffer_layout(&layout);
        push_element_on_layout(&layout, 2, false, GL_FLOAT);
        push_element_on_layout(&layout, 2, false, GL_FLOAT);

        add_buffer(&test_state->VAO, &test_state->VertexBuffer, &layout);

        // Inicializa el idex buffer
        init_index_buffer(indices, 12, &test_state->IndexBuffer);

        test_state->proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        test_state->view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));


        init_shader(&test_state->shader, "res/shaders/basic.shader");
        bind_shader(&test_state->shader);


        init_texture(&test_state->texture, "res/textures/tank_green.png");
        set_uniform_1i(&test_state->shader, "u_Texture", 0);


        current_test->on_update = batch_on_update;
        current_test->on_render = batch_render;
        current_test->on_imgui_render = batch_imgui_render;
        current_test->delete_test = batch_delete_test;

        return (void*)test_state;
    }
    else {
        return NULL;
    }
}