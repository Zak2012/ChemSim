//frag
precision highp float;

#define PI radians(180.0)

out vec4 Color;

in vec4 fColor;
in vec2 fTexCoord;
in float fOutline;
in float fAngle;



void main()
{
    float Angle = atan(fTexCoord.y,fTexCoord.x) + PI; //Radians
    float Distance = sqrt(dot(fTexCoord, fTexCoord));
    // Color = fColor;
    if (fColor.a < 0.1f )
    {
        discard;
    }

    if (Angle < fAngle)
    {
        discard;
    }

    if (Distance > 1.0f)
    {
        discard;
    }

    if (Distance < fOutline)
    {
        discard;
    }

    Color = fColor;
    // Color = vec4(Angle/(2*PI),0.0f,0.0f,1.0f);
}