#shader vertex

#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float a_TexIndex;

out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;

uniform mat4 u_MVP;

void main()
{
   v_TexCoord = texCoord;
   v_Color = a_Color;
   v_TexIndex = a_TexIndex;
   gl_Position = u_MVP * position;
}


#shader fragment

#version 330

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexIndex;

uniform sampler2D u_Textures[2];

void main()
{
    int index = int(v_TexIndex);   // Para poder usarlo como un indice en un array tiene que ser int

    // Asignamos distintos texture slots dependiendo del v_TexIndex
    color = texture(u_Textures[index], v_TexCoord);
}