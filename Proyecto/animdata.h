#ifndef ANIMDATA_H
#define ANIMDATA_H

#include <glm/glm.hpp>

struct BoneInfo
{
    int id;             // indice del hueso en el arreglo final del shader
    glm::mat4 offset;   // inverse bind pose: lleva el vertice del espacio del modelo al espacio del hueso
};

#endif
