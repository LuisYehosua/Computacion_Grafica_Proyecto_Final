#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    // Truco: forzar z = w para que despues de la division perspectiva quede z=1
    // (siempre al fondo del depth buffer)
    gl_Position = pos.xyww;
}
