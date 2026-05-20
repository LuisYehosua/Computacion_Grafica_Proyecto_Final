#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(vec3(0.3, 1.0, 0.5));

    float ambient = 0.4;
    float diff    = max(dot(N, L), 0.0);

    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if (texColor.a < 0.01) texColor = vec4(0.8, 0.8, 0.8, 1.0);

    vec3 result = (ambient + diff * 0.7) * texColor.rgb;
    FragColor   = vec4(result, 1.0);
}