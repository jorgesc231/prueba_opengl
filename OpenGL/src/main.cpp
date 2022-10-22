#include <GL/glew.h>

#define GLEW_STATIC

#include <glfw3.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestBatchRendering.h"
#include "tests/TestBatchColor.h"
#include "tests/TestBatchTexture.h"
#include "tests/TestBatchDynamic.h"

#include "tests/BatchRenderer.h"


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Requerido por imgui
    //const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Primeras pruebas con OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // Activa el v-sync


    if (glewInit() != GLEW_OK)
        printf("ERROR: Inicializar GLEW\n");

    printf("%s\n", glGetString(GL_VERSION));

    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    


    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));


    TestMenu test_menu = { 0 };

    // Crea en memoria un objeto del test y guarda un puntero a el en las lista con su nombre...
    register_test_in_menu(&test_menu, "Clear Color", create_test_clear_color);
    register_test_in_menu(&test_menu, "2D Texture", create_test_texture);
    register_test_in_menu(&test_menu, "Introduccion Batch Rendering", create_test_batch);
    register_test_in_menu(&test_menu, "Batch Color", create_test_batch_color);
    register_test_in_menu(&test_menu, "Batch Texture", create_test_batch_texture);
    register_test_in_menu(&test_menu, "Batch Dynamic Geometry", create_test_batch_dynamic);
    
    //register_test_in_menu(&test_menu, "Batch Renderer", create_renderer);

    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        renderer_clear();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (test_menu.current_test)
        {
            test_menu.current_test->on_update(0.0f);
            test_menu.current_test->on_render(test_menu.current_test->test_state);

            ImGui::Begin("Test");

            if (ImGui::Button("<-"))
            {
                test_menu.current_test->delete_test(test_menu.current_test->test_state);
                test_menu.current_test = NULL;
            } else
                test_menu.current_test->on_imgui_render(test_menu.current_test->test_state);

            ImGui::End();
        }
        else {
            render_test_menu(&test_menu);
        }


        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (test_menu.current_test)
        test_menu.current_test->delete_test(test_menu.current_test->test_state);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}