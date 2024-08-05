//vert
#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;

out vec4 fColor;

uniform mat4 Matrix;

void main()
{
    gl_Position = Matrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

    fColor = vColor;
}