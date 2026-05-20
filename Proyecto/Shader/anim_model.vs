#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal   = vec3(0.0);
    float totalWeight  = 0.0;

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (boneIds[i] < 0 || boneIds[i] >= MAX_BONES) continue;

        vec4 localPos = finalBonesMatrices[boneIds[i]] * vec4(pos, 1.0);
        totalPosition += localPos * weights[i];

        vec3 localNorm = mat3(finalBonesMatrices[boneIds[i]]) * norm;
        totalNormal += localNorm * weights[i];

        totalWeight += weights[i];
    }

    if (totalWeight > 0.01) {
        totalPosition /= totalWeight;
        totalNormal   /= totalWeight;
    } else {
        totalPosition = vec4(pos, 1.0);
        totalNormal   = norm;
    }

    gl_Position = projection * view * model * totalPosition;
    FragPos     = vec3(model * totalPosition);
    Normal      = mat3(transpose(inverse(model))) * totalNormal;
    TexCoords   = tex;
}
