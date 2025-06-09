//frag
#version 330 core

precision highp float;

#define PI radians(180.0)

out vec4 Color;

in vec4 fColor;
in vec2 fTexCoord;
in float fOutline;
in float fAngle;
// in float fFlat;
in float fDepth;
in mat3 fTBN;

const vec3 lightColor = vec3(1.0f,1.0f,1.0f);
const float ambientStrength = 0.4f;
const float specularStrength = 0.0f;
const float diffuseStrength = 0.5f;

const vec3 lightDir = normalize(vec3(1.0f,2.0f,0.0f));

void main()
{
    float Angle = atan(fTexCoord.y,fTexCoord.x) + PI; //Radians
    float Distance = sqrt(dot(fTexCoord, fTexCoord));

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

    vec3 Lighting = vec3(1.0f, 1.0f, 1.0f);
     vec3 Normal;

    if (fDepth > 0.0f)
    {
        Normal = fTBN * normalize(vec3(0.0f, 0.0f, 1.0f) + vec3(fTexCoord, 0.0f));
        
        vec3 ambient = ambientStrength * lightColor;

        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        vec3 viewDir = normalize(vec3(0.0f, -1.0f, 0.0f) - vec3(gl_FragCoord));
        vec3 reflectDir = reflect(lightDir, Normal);  

        float spec = pow(max(dot(-viewDir, reflectDir), 0.0), 128.0);
        vec3 specular = specularStrength * spec * lightColor;  
        Lighting = ambient + diffuse + specular;
        // gl_FragDepth = gl_FragCoord.z - ((1-Distance)* fDepth);
        // gl_FragDepth = ((1-Distance));
        // gl_FragDepth = -1;
    }


    Color = vec4(Lighting, 1.0f) * fColor;

    if (Distance > 1-fOutline)
    {
        Color = vec4(0,0,0, 1.0f);

    }
    // Color = vec4(gl_FragCoord.z - (1-Distance* fDepth),0,0, 1.0f);
}