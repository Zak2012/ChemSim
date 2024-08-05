//vert
#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vTexCoord;

out vec4 fColor;
out vec2 fTexCoord;

uniform mat4 Matrix;

void main()
{
    gl_Position = Matrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

    fColor = vColor;
    fTexCoord = vTexCoord;
}