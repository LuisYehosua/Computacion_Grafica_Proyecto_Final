#ifndef ANIMATED_MODEL_H
#define ANIMATED_MODEL_H

#include <vector>
#include <string>
#include <map>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SOIL2/SOIL2.h"
#include "AnimMesh.h"
#include "animdata.h"

class AssimpGLMHelpers
{
public:
    static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
    {
        glm::mat4 to;
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }
    static glm::vec3 GetGLMVec(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }
    static glm::quat GetGLMQuat(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
};

class AnimatedModel
{
public:
    std::vector<AnimTexture> textures_loaded;
    std::vector<AnimMesh>    meshes;
    std::string              directory;

    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    AnimatedModel(const std::string& path) { loadModel(path); }

    void Draw(Shader& shader)
    {
        for (auto& m : meshes) m.Draw(shader);
    }

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

private:
    void loadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs | aiProcess_LimitBoneWeights);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR ASSIMP :: " << importer.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        // Recorrer la jerarquia y aplicar las transformaciones nodales
        // a los vertices de cada malla. Esto pone TODOS los vertices
        // en el mismo espacio (model space) sin importar en que nodo vivan.
        processNode(scene->mRootNode, scene, glm::mat4(1.0f));

        std::cout << "Modelo cargado: " << meshes.size() << " mallas, "
            << m_BoneCounter << " huesos\n";
    }

    void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform)
    {
        // Acumular transformacion del nodo
        glm::mat4 nodeMatrix = AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);
        glm::mat4 globalNodeTransform = parentTransform * nodeMatrix;

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, globalNodeTransform));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene, globalNodeTransform);
    }

    void SetVertexBoneDataToDefault(AnimVertex& v)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            v.m_BoneIDs[i] = -1;
            v.m_Weights[i] = 0.0f;
        }
    }

    void SetVertexBoneData(AnimVertex& v, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            if (v.m_BoneIDs[i] < 0) {
                v.m_BoneIDs[i] = boneID;
                v.m_Weights[i] = weight;
                return;
            }
        }
    }

    void ExtractBoneWeightForVertices(std::vector<AnimVertex>& vertices,
        aiMesh* mesh, const aiScene*,
        const glm::mat4& nodeTransform)
    {
        // Necesitamos ajustar los offsets de los huesos para que tomen en cuenta
        // la transformacion nodal aplicada a los vertices.
        // Si vertex_nuevo = nodeTransform * vertex_viejo,
        // entonces offset_nuevo = offset_viejo * inverse(nodeTransform)
        glm::mat4 invNodeTransform = glm::inverse(nodeTransform);

        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

            if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
                BoneInfo info;
                info.id = m_BoneCounter;
                glm::mat4 originalOffset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
                    mesh->mBones[boneIndex]->mOffsetMatrix);
                // Ajustar el offset por la transformacion del nodo
                info.offset = originalOffset * invNodeTransform;
                m_BoneInfoMap[boneName] = info;
                boneID = m_BoneCounter;
                m_BoneCounter++;
            }
            else {
                boneID = m_BoneInfoMap[boneName].id;
            }

            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;
            for (int wi = 0; wi < numWeights; ++wi) {
                int vertexId = weights[wi].mVertexId;
                float weight = weights[wi].mWeight;
                if (vertexId < (int)vertices.size())
                    SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }

    AnimMesh processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 nodeTransform)
    {
        std::vector<AnimVertex>   vertices;
        std::vector<unsigned int> indices;
        std::vector<AnimTexture>  textures;

        // Para transformar normales correctamente
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(nodeTransform)));

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            AnimVertex v;
            SetVertexBoneDataToDefault(v);

            // CLAVE: pre-aplicar la transformacion del nodo al vertice
            glm::vec3 posOriginal = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
            glm::vec4 posTransformed = nodeTransform * glm::vec4(posOriginal, 1.0f);
            v.Position = glm::vec3(posTransformed);

            // Aplicar tambien a las normales
            glm::vec3 normOriginal = mesh->HasNormals()
                ? AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i])
                : glm::vec3(0.0f);
            v.Normal = glm::normalize(normalMatrix * normOriginal);

            if (mesh->mTextureCoords[0])
                v.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y);
            else
                v.TexCoords = glm::vec2(0.0f);

            vertices.push_back(v);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

            auto diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            if (diffuseMaps.empty()) {
                aiColor3D color(0.8f, 0.8f, 0.8f);
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

                GLuint texID;
                glGenTextures(1, &texID);
                glBindTexture(GL_TEXTURE_2D, texID);

                unsigned char colorData[4] = {
                    (unsigned char)(color.r * 255),
                    (unsigned char)(color.g * 255),
                    (unsigned char)(color.b * 255),
                    255
                };
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, colorData);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);

                AnimTexture solid;
                solid.id = texID;
                solid.type = "texture_diffuse";
                solid.path = "";
                textures.push_back(solid);
            }
        }

        // Pasar el nodeTransform para ajustar los offsets de huesos
        ExtractBoneWeightForVertices(vertices, mesh, scene, nodeTransform);
        return AnimMesh(vertices, indices, textures);
    }

    std::vector<AnimTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        const std::string& typeName)
    {
        std::vector<AnimTexture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip = false;
            for (auto& t : textures_loaded) {
                if (t.path == str.C_Str()) {
                    textures.push_back(t);
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                AnimTexture t;
                std::string filename = directory + "/" + std::string(str.C_Str());
                t.id = SOIL_load_OGL_texture(filename.c_str(),
                    SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
                t.type = typeName;
                t.path = str.C_Str();
                textures.push_back(t);
                textures_loaded.push_back(t);
            }
        }
        return textures;
    }
};

#endif