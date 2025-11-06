#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec2 vTexCoord;
layout(location = 5) in float vOutline;
layout(location = 6) in float vDepth;
// layout(location = 6) in float vAngle;

out vec4 fColor;
out vec2 fTexCoord;
out float fOutline;
out float fDepth;
out mat3 fTBN;

uniform mat4 Matrix;

void main()
{
    gl_Position = Matrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

    if (vDepth > 0.0f)
    {
        vec3 Bitangent = cross(vNormal, vTangent);
        fTBN = mat3(vTangent, Bitangent, vNormal);
    }
    fColor = vColor;
    fTexCoord = vTexCoord;
    // fAngle = vAngle;
    fOutline = vOutline;
    // vec4 temp = Matrix * vec4(0.0f, 0.0f, vDepth, 1.0f);
    fDepth = vDepth / 10.0f;
}