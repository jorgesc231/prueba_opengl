#pragma once

#include "Test.h"

#include <GL/glew.h>
#include <imgui/imgui.h>

struct TestClearColorState
{
	float clear_color[4];
};

void clear_color_delete_test(void* state)
{
	; // Deberia borrar la memoria
}

void clear_color_on_update(float dt)
{
	;
}

void clear_color_render(void* state)
{
	TestClearColorState* prueba = (TestClearColorState*)state;

	glClearColor(prueba->clear_color[0], prueba->clear_color[1], prueba->clear_color[2], prueba->clear_color[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void clear_color_imgui_render(void* state)
{
	TestClearColorState* prueba = (TestClearColorState*)state;
	ImGui::ColorEdit4("Clear Color", prueba->clear_color);
}

// Retorna un puntero al estado del test almacenado en el heap. Tiene que ser borrado con delete_test_clear_color
void* create_test_clear_color(Test* current_test)
{
	TestClearColorState* test_state = (TestClearColorState*)malloc(sizeof(TestClearColorState));

	if (test_state)
	{
		test_state->clear_color[0] = 0.2f;
		test_state->clear_color[1] = 0.3f;
		test_state->clear_color[2] = 0.8f;
		test_state->clear_color[3] = 1.0f;

		current_test->on_update = clear_color_on_update;
		current_test->on_render = clear_color_render;
		current_test->on_imgui_render = clear_color_imgui_render;

		current_test->delete_test = clear_color_delete_test;

		return (void*)test_state;
	}
	else {
		return NULL;
	}
}