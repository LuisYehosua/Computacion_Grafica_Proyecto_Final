#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <vector>
#include <glm/glm.hpp>
#include "Animation.h"

#define MAX_BONES_ANIM 200

class Animator
{
public:
    Animator(Animation* animation)
    {
        m_CurrentTime      = 0.0f;
        m_CurrentAnimation = animation;
        m_FinalBoneMatrices.reserve(MAX_BONES_ANIM);
        for (int i = 0; i < MAX_BONES_ANIM; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void UpdateAnimation(float dt)
    {
        if (m_CurrentAnimation) {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void PlayAnimation(Animation* anim)
    {
        m_CurrentAnimation = anim;
        m_CurrentTime = 0.0f;
    }

    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
    {
        std::string nodeName      = node->name;
        glm::mat4   nodeTransform = node->transformation;

        Bone* bone = m_CurrentAnimation->FindBone(nodeName);
        if (bone) {
            bone->Update(m_CurrentTime);
            nodeTransform = bone->GetLocalTransform();
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;

        auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap.at(nodeName).id;
            glm::mat4 offset = boneInfoMap.at(nodeName).offset;
            if (index < MAX_BONES_ANIM)
                m_FinalBoneMatrices[index] = globalTransform * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransform);
    }

    std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation*             m_CurrentAnimation;
    float                  m_CurrentTime;
};

#endif
