#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <string>
#include <map>
#include <iostream>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Bone.h"
#include "animdata.h"
#include "AnimatedModel.h"

struct AssimpNodeData
{
    glm::mat4                   transformation;
    std::string                 name;
    int                         childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:
    Animation() = default;

    // Carga animacion por INDICE (0 = primera, 1 = segunda, etc.)
    Animation(const std::string& animationPath, AnimatedModel* model, int animationIndex = 0)
    {
        loadInternal(animationPath, model, animationIndex, "");
    }

    // Carga animacion por NOMBRE
    // Para tu Adventurer.fbx: Animation("...", &model, "CharacterArmature|Run")
    Animation(const std::string& animationPath, AnimatedModel* model,
              const std::string& animationName)
    {
        loadInternal(animationPath, model, -1, animationName);
    }

    Bone* FindBone(const std::string& name)
    {
        for (auto& b : m_Bones)
            if (b.GetBoneName() == name) return &b;
        return nullptr;
    }

    float                          GetTicksPerSecond() { return m_TicksPerSecond; }
    float                          GetDuration()       { return m_Duration; }
    const AssimpNodeData&          GetRootNode()       { return m_RootNode; }
    const std::map<std::string,BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

private:
    float                            m_Duration = 0.0f;
    float                            m_TicksPerSecond = 25.0f;
    std::vector<Bone>                m_Bones;
    AssimpNodeData                   m_RootNode;
    std::map<std::string, BoneInfo>  m_BoneInfoMap;

    void loadInternal(const std::string& path, AnimatedModel* model,
                      int index, const std::string& name)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
        if (!scene || !scene->mRootNode) {
            std::cout << "ERROR cargando animacion: " << path << std::endl;
            return;
        }
        if (scene->mNumAnimations == 0) {
            std::cout << "El archivo no contiene animaciones: " << path << std::endl;
            return;
        }

        // Resolver indice
        int finalIndex = 0;
        if (!name.empty()) {
            // buscar por nombre
            for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
                if (name == scene->mAnimations[i]->mName.C_Str()) {
                    finalIndex = i;
                    break;
                }
            }
            std::cout << "Buscando animacion '" << name << "' -> indice " << finalIndex << "\n";
        } else if (index >= 0 && index < (int)scene->mNumAnimations) {
            finalIndex = index;
        }

        aiAnimation* animation = scene->mAnimations[finalIndex];
        m_Duration       = (float)animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond != 0
                            ? (float)animation->mTicksPerSecond : 25.0f;

        ReadHierarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);

        std::cout << "Animacion '" << animation->mName.C_Str()
                  << "' cargada. Duracion: " << m_Duration
                  << " ticks, " << m_TicksPerSecond << " tps, "
                  << animation->mNumChannels << " canales\n";
    }

    void ReadMissingBones(const aiAnimation* animation, AnimatedModel& model)
    {
        int size = animation->mNumChannels;
        auto& boneInfoMap = model.GetBoneInfoMap();
        int&  boneCount   = model.GetBoneCount();

        for (int i = 0; i < size; i++) {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;
            if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            m_Bones.push_back(Bone(boneName, boneInfoMap[boneName].id, channel));
        }
        m_BoneInfoMap = boneInfoMap;
    }

    void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
    {
        dest.name           = src->mName.data;
        dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount  = src->mNumChildren;
        for (unsigned int i = 0; i < src->mNumChildren; i++) {
            AssimpNodeData child;
            ReadHierarchyData(child, src->mChildren[i]);
            dest.children.push_back(child);
        }
    }
};

#endif
