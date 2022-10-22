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


struct TestTexture2DState
{
    glm::vec3 translationA;
    glm::vec3 translationB;

    vertex_array VAO;
    vertex_buffer VertexBuffer;
    index_buffer IndexBuffer;
    Shader shader;

    Texture texture;

    glm::mat4 proj;
    glm::mat4 view;;
};

void texture2d_delete_test(void* state)
{
	; // TODO: Deberia borrar la memoria cuando se vuelve al menu
}

void texture2d_on_update(float dt)
{
	;
}

void texture2d_render(void* state)
{
	TestTexture2DState* prueba = (TestTexture2DState*)state;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    bind_texture(&prueba->texture, 0);

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), prueba->translationA);
        glm::mat4 mvp = prueba->proj * prueba->view * model;
        bind_shader(&prueba->shader);
        set_uniform_mat4f(&prueba->shader, "u_MVP", mvp);

        renderer_draw(&prueba->VAO, &prueba->IndexBuffer, &prueba->shader);
    }

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), prueba->translationB);
        glm::mat4 mvp = prueba->proj * prueba->view * model;
        bind_shader(&prueba->shader);
        set_uniform_mat4f(&prueba->shader, "u_MVP", mvp);

        renderer_draw(&prueba->VAO, &prueba->IndexBuffer, &prueba->shader);
    }
}

void texture_imgui_render(void* state)
{
	TestTexture2DState* prueba = (TestTexture2DState*)state;
	
    ImGui::SliderFloat3("Translation A", &prueba->translationA.x, -4.0f, 4.0f);
    ImGui::SliderFloat3("Translation B", &prueba->translationB.x, -4.0f, 4.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

// Retorna un puntero al estado del test almacenado en el heap. Tiene que ser borrado con delete_test_clear_color
void* create_test_texture(Test* current_test)
{
	TestTexture2DState* test_state = (TestTexture2DState*)malloc(sizeof(TestTexture2DState));

	if (test_state)
	{
        test_state->translationA = glm::vec3(0.0f, 0.0f, 0.0f);
        test_state->translationB = glm::vec3(1.0f, 0.0f, 0.0f);

        // Vertex positions
        float positions[] = {
            -0.5, -0.5, 0.0f, 0.0f,  // 0
             0.5, -0.5, 1.0f, 0.0f,  // 1
             0.5, 0.5,  1.0f, 1.0f,  // 2
             -0.5, 0.5, 0.0f, 1.0f  // 3
        };

        // Index Buffer
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Inicializa el vertex array
        init_vertex_array(&test_state->VAO);

        // Inicializa el vertex buffer
        init_vertex_buffer(positions, 4 * 4 * sizeof(float), &test_state->VertexBuffer);

        vertex_buffer_layout layout;
        init_vertex_buffer_layout(&layout);
        push_element_on_layout(&layout, 2, false, GL_FLOAT);
        push_element_on_layout(&layout, 2, false, GL_FLOAT);

        add_buffer(&test_state->VAO, &test_state->VertexBuffer, &layout);

        // Inicializa el idex buffer
        init_index_buffer(indices, 6, &test_state->IndexBuffer);

        test_state->proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        test_state->view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));


        init_shader(&test_state->shader, "res/shaders/basic.shader");
        bind_shader(&test_state->shader);


        init_texture(&test_state->texture, "res/textures/tank_green.png");
        set_uniform_1i(&test_state->shader, "u_Texture", 0);


        current_test->on_update = texture2d_on_update;
        current_test->on_render = texture2d_render;
        current_test->on_imgui_render = texture_imgui_render;
        current_test->delete_test = texture2d_delete_test;

		return (void*)test_state;
	}
	else {
		return NULL;
	}
}