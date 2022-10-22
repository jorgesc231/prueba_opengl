#shader vertex

#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 a_Color;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 u_MVP;

void main()
{
   v_TexCoord = texCoord;
   v_Color = a_Color;
   gl_Position = u_MVP * position;
}


#shader fragment

#version 330

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform sampler2D u_Texture;

void main()
{
    vec4 TexColor = texture(u_Texture, v_TexCoord);
    //color = TexColor;
    color = v_Color;
}