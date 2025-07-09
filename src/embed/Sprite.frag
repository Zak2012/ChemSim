#version 330 core

precision highp float;

out vec4 Color;

in vec4 fColor;
in vec2 fTexCoord;
in float fDepth;
in mat3 fTBN;

uniform sampler2D TextureUnit;

const vec3 lightColor = vec3(1.0f,1.0f,1.0f);
const float ambientStrength = 0.4f;
const float specularStrength = 0.0f;
const float diffuseStrength = 0.5f;

const vec3 lightDir = normalize(vec3(1.0f,2.0f,0.0f));

void main()
{
    vec3 Lighting = vec3(1.0f, 1.0f, 1.0f);

    if (fDepth > 0.0f)
    {
        vec3 Normal = fTBN * normalize(vec3(0.0f, 0.0f, 1.0f) + vec3(fTexCoord, 0.0f));
        
        vec3 ambient = ambientStrength * lightColor;

        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        vec3 viewDir = normalize(vec3(0.0f, -1.0f, 0.0f) - vec3(gl_FragCoord));
        vec3 reflectDir = reflect(lightDir, Normal);  

        float spec = pow(max(dot(-viewDir, reflectDir), 0.0), 128.0);
        vec3 specular = specularStrength * spec * lightColor;  
        Lighting = ambient + diffuse + specular;
        Color = vec4(Lighting, 1.0f) * fColor;

        // gl_FragDepth = gl_FragCoord.z - ((1-sqrt(dot(fTexCoord,fTexCoord)))* fDepth);
    }
    else
    {
         Color = texture(TextureUnit, fTexCoord) * fColor;
    }
    // Color = vec4(fTexCoord, 0.0f,1.0f);

    if (Color.a < 0.1f )
    {
        discard;
    }
}