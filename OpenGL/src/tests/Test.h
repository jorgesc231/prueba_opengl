#pragma once

struct Test {
	const char *name;
	void* test_state;

	void *(*create_test)(Test* current_test);              // Constructor
	void (*delete_test)(void* state);               // Destructor
	void (*on_update)(float dt);         // OnUpdate (delta_time)
	void (*on_render)(void *state);                 // OnRender
	void (*on_imgui_render)(void* state);           // OnImGuiRender
};



struct TestMenu
{
	Test* current_test;   // Modificable por el menu para cambiarlo a otro test

	Test* tests;
	int test_count;
};


void register_test_in_menu(TestMenu* menu, const char* name, void*(*create_test)(Test*))
{
	menu->tests = (Test*)realloc(menu->tests, sizeof(Test) * (menu->test_count + 1));
	
	if (menu->tests)
	{
		menu->tests[menu->test_count].name = name;

		menu->tests[menu->test_count].create_test = create_test;
		menu->tests[menu->test_count].on_update = NULL;
		menu->tests[menu->test_count].on_render = NULL;
		menu->tests[menu->test_count].on_imgui_render = NULL;

		menu->test_count += 1;
	}
}

void delete_test_menu(TestMenu* menu)
{
	; // TODO: Deberia borrar la memoria de las lista de tests...
}

void render_test_menu(TestMenu* menu)
{
	for (int i = 0; i < menu->test_count; i++)
	{
		if (ImGui::Button(menu->tests[i].name))
		{
			menu->current_test = &menu->tests[i];
			menu->current_test->test_state = menu->current_test->create_test(menu->current_test);
		}
	}
}


