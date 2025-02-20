//frag
precision highp float;

out vec4 Color;

in vec4 fColor;
in vec2 fTexCoord;
in vec2 fGlowThes;
in vec4 fGlowColr;
in vec2 fOutlTres;
in vec4 fOutlColr;

uniform sampler2D TextureUnit;

float GetPixelOpacity(float Border, float Distance)
{
    return (1.0/Border) * Distance;
}

void main()
{

    vec4 FinalClr;
    vec4 BaseClr = fColor;
    vec4 OutlineClr = fOutlColr;
    vec4 GlowClr = fGlowColr;

    vec4 TexColor = texture(TextureUnit, fTexCoord);
    BaseClr.a = TexColor.r;
    // BaseClr = TexColor;

    //https://drewcassidy.me/2020/06/26/sdf-antialiasing/
    // sdf distance from edge (scalar)
    float dist = (0.5f - TexColor.r);

    // sdf distance per pixel (gradient vector)
    vec2 ddist = vec2(dFdx(dist), dFdy(dist));

    // distance to edge in pixels (scalar)
    float pixelDist = dist / length(ddist);

    BaseClr.a = clamp(0.5f - pixelDist, 0.0f, 1.0f); 
    

    if (fOutlTres.x != fOutlTres.y && TexColor.r > fOutlTres.x && TexColor.r < fOutlTres.y)
    {
        BaseClr = fOutlColr;
        BaseClr.a = 1.0f;
    }
    if (fGlowThes.x != fGlowThes.y && TexColor.r > fGlowThes.x && TexColor.r < fGlowThes.y)
    {
        BaseClr = fGlowColr;
        BaseClr.a = smoothstep(fGlowThes.x, fGlowThes.y, TexColor.r);
    }

    // BaseClr.a = step(0.5f, TexColor.r);

    // if (fOutlTres.w != fOutlTres.z && TexColor.r >= fOutlTres.w && TexColor.r <= fOutlTres.z)
    // {
    //     float PixelOpacity = 1.0f;
    //     if (TexColor.r <= fOutlTres.x)
    //     {
    //         PixelOpacity = smoothstep(fOutlTres.w, fOutlTres.x, TexColor.r);
    //     }
    //     else
    //     {
    //         PixelOpacity = smoothstep(fOutlTres.z, fOutlTres.y, TexColor.r);
    //     }
    //     BaseClr = mix(BaseClr, fOutlColr, PixelOpacity);
    // }

    // if (fSoftTres.x != fSoftTres.y)
    // {
    //     BaseClr.a *= smoothstep(fSoftTres.x, fSoftTres.y, TexColor.r);
    // }
    // else
    // {
        // BaseClr.a = step(0.5f, TexColor.r);
    // }

    // if (fGlowThes.x != fGlowThes.y)
    // {

    // }

    // if (fOutlTres.x != fOutlTres.y && TexColor.r > fOutlTres.x && TexColor.r < fOutlTres.y)
    // {
    //     FinalColor = fOutlColr;
    // }
    // if (fGlowThes.x != fGlowThes.y && TexColor.r > fGlowThes.x && TexColor.r < fGlowThes.y)
    // {
    //     FinalColor = fGlowColr;
    //     FinalColor.a = smoothstep(fGlowThes.x, fGlowThes.y, TexColor.r);
    // }
    // if (TexColor.r > 0.5f)
    // {
    //     FinalColor = fColor;
    //     FinalColor.a *= smoothstep(fSoftTres.x, fSoftTres.y, TexColor.r);
    // }
    // else
    // {
    //     discard;
    // }

    Color = BaseClr;
    if (Color.a < 0.1f)
    {
        discard;
        // Color = vec4(1.0f,0.0f,0.0f,1.0f);
    }
}