R"(
//vert

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vTangent;

out vec4 fColor;
out mat3 fTBN;

uniform float Flat;
uniform mat4 Matrix;

void main()
{
    gl_Position = Matrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0f);
    vec3 Bitangent = cross(vNormal, vTangent);
    fTBN = mat3(vTangent, Bitangent, vNormal);
    fColor = vColor;
}
)"