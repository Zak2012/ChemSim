//vert
#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in float vOutline;
layout(location = 4) in float vAngle;

out vec4 fColor;
out vec2 fTexCoord;
out float fOutline;
out float fAngle;

uniform mat4 Matrix;

void main()
{
    gl_Position = Matrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

    fColor = vColor;
    fTexCoord = vTexCoord;
    fAngle = vAngle;
    fOutline = vOutline;
}