//frag
#version 330 core

out vec4 Color;

in vec4 fColor;
void main()
{
    if (fColor.a < 0.1f )
    {
        discard;
    }

    Color = fColor;
}