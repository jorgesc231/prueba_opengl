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

// TODO: Reemplazar con la consulta a los drivers de la cantidad maxima
#define MAX_TEXTURES 8 // 32

static const size_t MaxQuadCount = 10000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount = MaxQuadCount * 6;
static size_t MaxTextures = MAX_TEXTURES;


struct s_Vertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoords;
    float TexIndex;
};

struct RendererData
{
    GLuint WhiteTexture;
    uint32_t WhiteTextureSlot;
    
    // Cuantos indices tenemos que dibujar cuando hacemos un Flush
    uint32_t IndexCount;

    // Puntero al principio de buffer de Quads
    s_Vertex* QuadBuffer;
    s_Vertex* QuadBufferPtr;   // Indicar la ubicacion cuando copiamos datos

    // mapeo de texture slots a OpenGL Texture ID
    uint32_t TextureSlots[MAX_TEXTURES];
    uint32_t TextureSlotIndex;

    // Stats para mostrar en imgui
    uint32_t DrawCount;
    uint32_t QuadCount;
};

struct BatchRendererState
{
    glm::vec3 translation;

    vertex_array VAO;
    vertex_buffer VertexBuffer;
    index_buffer IndexBuffer;
    Shader shader;

    Texture green_tex;
    Texture blue_tex;

    RendererData renderer_data;

    glm::mat4 proj;
    glm::mat4 view;;
};


static void Init(RendererData* data, BatchRendererState *state)
{
    data->TextureSlotIndex = 1;

    data->QuadBuffer = (s_Vertex*)malloc(sizeof(s_Vertex) * MaxVertexCount);

    init_vertex_array(&state->VAO);

    init_dynamic_vertex_buffer(MaxVertexCount * sizeof(s_Vertex), &state->VertexBuffer);

    vertex_buffer_layout layout;
    init_vertex_buffer_layout(&layout);
    push_element_on_layout(&layout, 3, false, GL_FLOAT);
    push_element_on_layout(&layout, 4, false, GL_FLOAT);
    push_element_on_layout(&layout, 2, false, GL_FLOAT);
    push_element_on_layout(&layout, 1, false, GL_FLOAT);

    add_buffer(&state->VAO, &state->VertexBuffer, &layout);

    uint32_t indices[MaxIndexCount];
    uint32_t offset = 0;

    for (int i = 0; i < MaxIndexCount; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;

        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;

        offset += 4;
    }

    init_index_buffer(indices, MaxIndexCount, &state->IndexBuffer);

    // 1x1 white texture
    //glCreateTextures(GL_TEXTURE_2D, 1, &data->WhiteTexture);
    glGenTextures(1, &data->WhiteTexture);
    glBindTexture(GL_TEXTURE_2D, data->WhiteTexture);
    
    // Si no se establecen estos parametros, probablemnte salga una textura negra
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    uint32_t color = 0xffffffff;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
    
    data->TextureSlots[0] = data->WhiteTexture;
    
    for (size_t i = 1; i < MAX_TEXTURES; i++)
        data->TextureSlots[i] = 0;
}

static void ShutDown(BatchRendererState* estado)
{
    delete_vertex_array(&estado->VAO);
    delete_vertex_buffer(&estado->VertexBuffer);
    delete_index_buffer(&estado->IndexBuffer);

    glDeleteTextures(1, &estado->renderer_data.WhiteTexture);

    free(&estado->renderer_data.QuadBuffer);
}

static void BeginBatch(BatchRendererState *estado)
{
    estado->renderer_data.QuadBufferPtr = estado->renderer_data.QuadBuffer;
}

static void EndBatch(BatchRendererState *estado)
{
    GLsizeiptr size = (uint8_t*)estado->renderer_data.QuadBufferPtr - (uint8_t*)estado->renderer_data.QuadBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, estado->VertexBuffer.renderer_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, estado->renderer_data.QuadBuffer);
}

static void Flush(BatchRendererState *estado)
{
    //for (uint32_t i = 0; i < estado->renderer_data.TextureSlotIndex; i++)
        //glBindTextureUnit(i, estado->renderer_data.TextureSlots[i]);

    bind_vertex_array(&estado->VAO);
    glDrawElements(GL_TRIANGLES, estado->renderer_data.IndexCount, GL_UNSIGNED_INT, NULL);
    estado->renderer_data.DrawCount;

    estado->renderer_data.IndexCount = 0;
    estado->renderer_data.TextureSlotIndex = 1;
}

static void DrawQuad(BatchRendererState* estado, glm::vec2* position, glm::vec2* size, glm::vec4* color)
{
    if (estado->renderer_data.IndexCount >= MaxIndexCount)
    {
        EndBatch(estado);
        Flush(estado);
        BeginBatch(estado);
    }

    RendererData* datos = &estado->renderer_data;

    float textureIndex = 0.0f;

    datos->QuadBufferPtr->Position = {position->x, position->y, 0.0f};
    datos->QuadBufferPtr->Color = *color;
    datos->QuadBufferPtr->TexCoords = {0.0f, 0.0f};
    datos->QuadBufferPtr->TexIndex = textureIndex;
    datos->QuadBufferPtr++;

    datos->QuadBufferPtr->Position = { position->x + size->x, position->y, 0.0f };
    datos->QuadBufferPtr->Color = *color;
    datos->QuadBufferPtr->TexCoords = { 1.0f, 1.0f };
    datos->QuadBufferPtr->TexIndex = textureIndex;
    datos->QuadBufferPtr++;

    datos->QuadBufferPtr->Position = { position->x + size->x, position->y + size->y, 0.0f };
    datos->QuadBufferPtr->Color = *color;
    datos->QuadBufferPtr->TexCoords = { 1.0f, 1.0f };
    datos->QuadBufferPtr->TexIndex = textureIndex;
    datos->QuadBufferPtr++;

    datos->QuadBufferPtr->Position = { position->x, position->y + size->y, 0.0f };
    datos->QuadBufferPtr->Color = *color;
    datos->QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
    datos->QuadBufferPtr->TexIndex = textureIndex;
    datos->QuadBufferPtr++;

    datos->IndexCount += 6;
    datos->QuadCount++;
}

static void DrawQuad(BatchRendererState* estado, glm::vec2* position, glm::vec2* size, GLuint textureID)
{
    if (estado->renderer_data.IndexCount >= MaxIndexCount || estado->renderer_data.TextureSlotIndex > 31)
    {
        EndBatch(estado);
        Flush(estado);
        BeginBatch(estado);
    }

    glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

    float texture_index = 0.0f;

    // Comprueba si la textura se ha usado en otro quad
    for (uint32_t i = 1; i < estado->renderer_data.TextureSlotIndex; i++)
    {
        if (estado->renderer_data.TextureSlots[i] == textureID)
        {
            texture_index = (float)i;
            break;
        }
    }

    // Si no se uso antes
    if (texture_index == 0.0f)
    {
        texture_index = (float)estado->renderer_data.TextureSlotIndex;
        estado->renderer_data.TextureSlots[estado->renderer_data.TextureSlotIndex] = textureID;
        estado->renderer_data.TextureSlotIndex++;
    }

    estado->renderer_data.QuadBufferPtr->Position = { position->x, position->y, 0.0f };
    estado->renderer_data.QuadBufferPtr->Color = color;
    estado->renderer_data.QuadBufferPtr->TexCoords = { 0.0f, 0.0f };
    estado->renderer_data.QuadBufferPtr->TexIndex = texture_index;
    estado->renderer_data.QuadBufferPtr++;

    estado->renderer_data.QuadBufferPtr->Position = { position->x + size->x, position->y, 0.0f };
    estado->renderer_data.QuadBufferPtr->Color = color;
    estado->renderer_data.QuadBufferPtr->TexCoords = { 1.0f, 0.0f };
    estado->renderer_data.QuadBufferPtr->TexIndex = texture_index;
    estado->renderer_data.QuadBufferPtr++;

    estado->renderer_data.QuadBufferPtr->Position = { position->x + size->x, position->y + size->y, 0.0f };
    estado->renderer_data.QuadBufferPtr->Color = color;
    estado->renderer_data.QuadBufferPtr->TexCoords = { 1.0f, 1.0f };
    estado->renderer_data.QuadBufferPtr->TexIndex = texture_index;
    estado->renderer_data.QuadBufferPtr++;

    estado->renderer_data.QuadBufferPtr->Position = { position->x, position->y + size->y, 0.0f };
    estado->renderer_data.QuadBufferPtr->Color = color;
    estado->renderer_data.QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
    estado->renderer_data.QuadBufferPtr->TexIndex = texture_index;
    estado->renderer_data.QuadBufferPtr++;

    estado->renderer_data.IndexCount += 6;
    estado->renderer_data.QuadCount++;


}

void renderer_delete_test(void* state)
{
    BatchRendererState* estado = (BatchRendererState*)state;
    ShutDown(estado);
}

void renderer_on_update(float dt)
{
    ;
}

void renderer_render(void* state)
{
    BatchRendererState* prueba = (BatchRendererState*)state;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    bind_shader(&prueba->shader);
    //bind_texture(&prueba->green_tex, 0);

    // Reset stats
    prueba->renderer_data.DrawCount = 0;
    prueba->renderer_data.QuadCount = 0;
    
    BeginBatch(prueba);
    for (float y = -10.0f; y < 10.0f; y += 0.25f)
    {
        for (float x = -10.0f; x < 10.0f; x += 0.25f)
        {
            glm::vec4 color = { (x + 10) / 20.0f, 0.2f, (y + 10) / 20.0f, 1.0f };
            glm::vec2 position = {x, y};
            glm::vec2 size = { 0.2f, 0.2f };
            //DrawQuad({ x, y }, { 0.25f, 0.25f }, color);
            DrawQuad(prueba, &position, &size, &color);
        }
    }

#if 1
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            GLuint tex = (x + y) % 2 == 0 ? prueba->blue_tex.renderer_id : prueba->green_tex.renderer_id;
            
            glm::vec2 position = { x, y };
            glm::vec2 size = { 1.0f, 1.0f };
            DrawQuad(prueba, &position, &size, tex);
        }
    }
#endif

    //DrawQuad(m_QuadPosition, {1.0f, 1.0f}, m_ChernoTex);

    EndBatch(prueba);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), prueba->translation);
    glm::mat4 mvp = prueba->proj * prueba->view * model;
    bind_shader(&prueba->shader);
    //set_uniform_mat4f(&prueba->shader, "u_MVP", mvp);

    Flush(prueba);
}

void renderer_imgui_render(void* state)
{
    BatchRendererState* prueba = (BatchRendererState*)state;

    ImGui::SliderFloat3("Translation", &prueba->translation.x, -4.0f, 4.0f);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

// Retorna un puntero al estado del test almacenado en el heap. 
// Tiene que ser borrado con delete_test_clear_color
void* create_renderer(Test* current_test)
{
    BatchRendererState* test_state = (BatchRendererState*)malloc(sizeof(BatchRendererState));

    if (test_state)
    {
        test_state->translation = glm::vec3(0.0f, 0.0f, 0.0f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        test_state->proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        test_state->view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));


        init_shader(&test_state->shader, "res/shaders/batch_renderer.shader");
        bind_shader(&test_state->shader);

        int samplers[MAX_TEXTURES];
        for (int i = 0; i < MAX_TEXTURES; i++)
            samplers[i] = i;

        set_uniform_1iv(&test_state->shader, "u_Textures", MAX_TEXTURES, samplers);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        // Inicializa
        Init(&test_state->renderer_data, test_state);

        init_texture(&test_state->green_tex, "res/textures/tank_green.png");
        init_texture(&test_state->blue_tex, "res/textures/tank_blue.png");
        


        current_test->on_update = renderer_on_update;
        current_test->on_render = renderer_render;
        current_test->on_imgui_render = renderer_imgui_render;
        current_test->delete_test = renderer_delete_test;

        return (void*)test_state;
    }
    else {
        return NULL;
    }
}