#version 330 core

precision highp float;

out vec4 Color;

in vec4 fColor;
in vec2 fTexCoord;
// in vec2 fGlowThes;
// in vec4 fGlowColr;
// in vec2 fOutlTres;
// in vec4 fOutlColr;
in mat3 fTBN;

uniform sampler2D TextureUnit;

float GetPixelOpacity(float Border, float Distance)
{
    return (1.0/Border) * Distance;
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{

    vec4 FinalClr;
    vec4 BaseClr = fColor;

    vec4 TexColor = texture(TextureUnit, fTexCoord);

    //https://drewcassidy.me/2020/06/26/sdf-antialiasing/
    // sdf distance from edge (scalar)
    float dist = 0.5f - median(TexColor.r, TexColor.g, TexColor.b);
    // float dist = (TexColor.r);

    // sdf distance per pixel (gradient vector)
    vec2 ddist = vec2(dFdx(dist), dFdy(dist));

    // distance to edge in pixels (scalar)
    float pixelDist = dist / length(ddist);

    float opacity = clamp(0.5f - pixelDist, 0.0f, 1.0f); 
    BaseClr.a = opacity;
    Color =  BaseClr;

    if (Color.a < 0.1f)
    {
        discard;
        // Color = vec4(1.0f,0.0f,0.0f,1.0f);
    }
}