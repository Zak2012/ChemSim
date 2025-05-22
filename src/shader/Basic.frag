//frag
#version 330 core

precision highp float;

out vec4 Color;

in vec4 fColor;
in mat3 fTBN;

void main()
{
    if (fColor.a < 0.1f )
    {
        discard;
    }

    Color = fColor;
}
