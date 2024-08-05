//frag
#version 330 core

out vec4 Color;

in vec4 fColor;
in vec2 fTexCoord;

uniform sampler2D TextureUnit;

void main()
{
    Color = texture(TextureUnit, fTexCoord) * fColor;
    // Color = vec4(fTexCoord, 0.0f,1.0f);

    if (Color.a < 0.1f )
    {
        discard;
    }
}