//frag
#version 330 core

#define PI radians(180.0f)
out vec4 Color;

in vec4 fColor;
in vec2 fTexCoord;
in float fOutline;
in float fAngle;

float Distance = sqrt(dot(fTexCoord, fTexCoord));
float Angle = atan(fTexCoord.y,fTexCoord.x) + PI; //Radians


void main()
{
    // Color = fColor;
    if (fColor.a < 0.1f )
    {
        discard;
    }

    if (Angle < fAngle)
    {
        discard;
    }

    if (Distance > 1)
    {
        discard;
    }

    if (Distance < fOutline)
    {
        discard;
    }

    Color = fColor;
    // Color = vec4(Angle/(2*PI),0,0,1);
}