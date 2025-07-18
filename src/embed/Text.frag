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
    // vec4 OutlineClr = fOutlColr;
    // vec4 GlowClr = fGlowColr;

    vec4 TexColor = texture(TextureUnit, fTexCoord);
    // BaseClr = TexColor;

    // float sd = median(TexColor.x, TexColor.y, TexColor.z);
    // float screenPxDistance = screenPxRange()*(sd - 0.5);
    // float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

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
    // BaseClr.a = dist < 0.0f? 1.0f:0.0f;

    // if (dist < 0.5f)
    // {
    //     Color =  BaseClr;
    // }
    // else
    // {
    //     discad
    // }
    

    // if (fOutlTres.x != fOutlTres.y && TexColor.a > fOutlTres.x && TexColor.a < fOutlTres.y)
    // {
    //     BaseClr = fOutlColr;
    //     BaseClr.a = 1.0f;
    // }
    // if (fGlowThes.x != fGlowThes.y && TexColor.a > fGlowThes.x && TexColor.a < fGlowThes.y)
    // {
    //     BaseClr = fGlowColr;
    //     BaseClr.a = smoothstep(fGlowThes.x, fGlowThes.y, TexColor.a);
    // }

    // BaseClr.a = step(0.5f, TexColor.a);

    // if (fOutlTres.w != fOutlTres.z && TexColor.a >= fOutlTres.w && TexColor.a <= fOutlTres.z)
    // {
    //     float PixelOpacity = 1.0f;
    //     if (TexColor.a <= fOutlTres.x)
    //     {
    //         PixelOpacity = smoothstep(fOutlTres.w, fOutlTres.x, TexColor.a);
    //     }
    //     else
    //     {
    //         PixelOpacity = smoothstep(fOutlTres.z, fOutlTres.y, TexColor.a);
    //     }
    //     BaseClr = mix(BaseClr, fOutlColr, PixelOpacity);
    // }

    // if (fSoftTres.x != fSoftTres.y)
    // {
    //     BaseClr.a *= smoothstep(fSoftTres.x, fSoftTres.y, TexColor.a);
    // }
    // else
    // {
        // BaseClr.a = step(0.5f, TexColor.a);
    // }

    // if (fGlowThes.x != fGlowThes.y)
    // {

    // }

    // if (fOutlTres.x != fOutlTres.y && TexColor.a > fOutlTres.x && TexColor.a < fOutlTres.y)
    // {
    //     FinalColor = fOutlColr;
    // }
    // if (fGlowThes.x != fGlowThes.y && TexColor.a > fGlowThes.x && TexColor.a < fGlowThes.y)
    // {
    //     FinalColor = fGlowColr;
    //     FinalColor.a = smoothstep(fGlowThes.x, fGlowThes.y, TexColor.a);
    // }
    // if (TexColor.a > 0.5f)
    // {
    //     FinalColor = fColor;
    //     FinalColor.a *= smoothstep(fSoftTres.x, fSoftTres.y, TexColor.a);
    // }
    // else
    // {
    //     discard;
    // }

    Color =  BaseClr;
    // Color =  vec4(1.0f,0.0f,0.0f,1.0f);
        //  Color = texture(TextureUnit, fTexCoord);

    if (Color.a < 0.1f)
    {
        discard;
        // Color = vec4(1.0f,0.0f,0.0f,1.0f);
    }
}