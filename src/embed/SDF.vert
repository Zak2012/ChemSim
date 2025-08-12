#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec2 vTexCoord;
// layout(location = 5) in vec2 vGlowThes;
// layout(location = 6) in vec4 vGlowColr;
// layout(location = 7) in vec2 vOutlTres;
// layout(location = 8) in vec4 vOutlColr;
// layout(location = 9) in vec2 vSoftTres;

out vec4 fColor;
out vec2 fTexCoord;
// out vec2 fGlowThes;
// out vec4 fGlowColr;
// out vec2 fOutlTres;
// out vec4 fOutlColr;
out mat3 fTBN;

uniform float Flat;
uniform mat4 Matrix;

void main()
{
    gl_Position = Matrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

    vec3 Bitangent = cross(vNormal, vTangent);
    fTBN = mat3(vTangent, Bitangent, vNormal);
    fColor = vColor;
    fTexCoord = vTexCoord;
    // fGlowThes = vGlowThes;
    // fGlowColr = vGlowColr;
    // fOutlTres = vOutlTres;
    // fOutlColr = vOutlColr;
}
