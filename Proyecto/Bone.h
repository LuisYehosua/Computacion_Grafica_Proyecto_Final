#ifndef BONE_H
#define BONE_H

// Necesario para que GLM permita usar gtx/quaternion (slerp, etc.)
#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/scene.h>

struct KeyPosition { glm::vec3 position;    float timeStamp; };
struct KeyRotation { glm::quat orientation; float timeStamp; };
struct KeyScale    { glm::vec3 scale;       float timeStamp; };

// Un hueso guarda sus keyframes (posicion, rotacion, escala con tiempos)
// y sabe interpolar entre ellos segun el tiempo actual.
class Bone
{
private:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale>    m_Scales;
    int m_NumPositions, m_NumRotations, m_NumScalings;

    glm::mat4   m_LocalTransform;
    std::string m_Name;
    int         m_ID;

public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel)
        : m_Name(name), m_ID(ID), m_LocalTransform(1.0f)
    {
        m_NumPositions = channel->mNumPositionKeys;
        for (int i = 0; i < m_NumPositions; ++i) {
            aiVector3D p = channel->mPositionKeys[i].mValue;
            m_Positions.push_back({ glm::vec3(p.x, p.y, p.z),
                                    (float)channel->mPositionKeys[i].mTime });
        }
        m_NumRotations = channel->mNumRotationKeys;
        for (int i = 0; i < m_NumRotations; ++i) {
            aiQuaternion q = channel->mRotationKeys[i].mValue;
            m_Rotations.push_back({ glm::quat(q.w, q.x, q.y, q.z),
                                    (float)channel->mRotationKeys[i].mTime });
        }
        m_NumScalings = channel->mNumScalingKeys;
        for (int i = 0; i < m_NumScalings; ++i) {
            aiVector3D s = channel->mScalingKeys[i].mValue;
            m_Scales.push_back({ glm::vec3(s.x, s.y, s.z),
                                 (float)channel->mScalingKeys[i].mTime });
        }
    }

    void Update(float animationTime)
    {
        glm::mat4 t = InterpolatePosition(animationTime);
        glm::mat4 r = InterpolateRotation(animationTime);
        glm::mat4 s = InterpolateScaling(animationTime);
        m_LocalTransform = t * r * s;
    }

    glm::mat4   GetLocalTransform() { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int         GetBoneID()         { return m_ID; }

private:
    int GetPositionIndex(float t) {
        for (int i = 0; i < m_NumPositions - 1; ++i)
            if (t < m_Positions[i + 1].timeStamp) return i;
        return 0;
    }
    int GetRotationIndex(float t) {
        for (int i = 0; i < m_NumRotations - 1; ++i)
            if (t < m_Rotations[i + 1].timeStamp) return i;
        return 0;
    }
    int GetScaleIndex(float t) {
        for (int i = 0; i < m_NumScalings - 1; ++i)
            if (t < m_Scales[i + 1].timeStamp) return i;
        return 0;
    }

    float GetScaleFactor(float last, float next, float t) {
        return (t - last) / (next - last);
    }

    glm::mat4 InterpolatePosition(float t) {
        if (m_NumPositions == 1)
            return glm::translate(glm::mat4(1.0f), m_Positions[0].position);
        int p0 = GetPositionIndex(t), p1 = p0 + 1;
        float f = GetScaleFactor(m_Positions[p0].timeStamp, m_Positions[p1].timeStamp, t);
        glm::vec3 pos = glm::mix(m_Positions[p0].position, m_Positions[p1].position, f);
        return glm::translate(glm::mat4(1.0f), pos);
    }

    glm::mat4 InterpolateRotation(float t) {
        if (m_NumRotations == 1)
            return glm::toMat4(glm::normalize(m_Rotations[0].orientation));
        int p0 = GetRotationIndex(t), p1 = p0 + 1;
        float f = GetScaleFactor(m_Rotations[p0].timeStamp, m_Rotations[p1].timeStamp, t);
        // slerp para rotaciones suaves
        glm::quat q = glm::slerp(m_Rotations[p0].orientation, m_Rotations[p1].orientation, f);
        return glm::toMat4(glm::normalize(q));
    }

    glm::mat4 InterpolateScaling(float t) {
        if (m_NumScalings == 1)
            return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);
        int p0 = GetScaleIndex(t), p1 = p0 + 1;
        float f = GetScaleFactor(m_Scales[p0].timeStamp, m_Scales[p1].timeStamp, t);
        glm::vec3 s = glm::mix(m_Scales[p0].scale, m_Scales[p1].scale, f);
        return glm::scale(glm::mat4(1.0f), s);
    }
};

#endif
