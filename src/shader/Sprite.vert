R"(
//vert

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec3 vNormal;
layout(location = 4) in vec3 vTangent;

out vec4 fColor;
out vec2 fTexCoord;
out float fFlat;
out mat3 fTBN;

uniform mat4 Matrix;
uniform float Flat;

void main()
{
    gl_Position = Matrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

    vec3 Bitangent = cross(vNormal, vTangent);
    fTBN = mat3(vTangent, Bitangent, vNormal);
    fColor = vColor;
    fTexCoord = vTexCoord;
    fFlat = Flat;
}
)"
