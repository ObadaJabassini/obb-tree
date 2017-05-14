#include <ModelLoader/Loader.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

using namespace std;

namespace ModelLoader {

    auto Loader::Load( std::string filePath ) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile( filePath.c_str(),
                                                  aiProcess_CalcTangentSpace |
                                                  aiProcess_Triangulate |
                                                  aiProcess_JoinIdenticalVertices |
                                                  aiProcess_SortByPType );
        if ( !scene ) {
            cout << "error!";
            throw exception();
        }
        for ( int i = 0; i < scene->mNumMeshes; ++i ) {
            auto mesh = scene->mMeshes[i];
            for(int j = 0; j < mesh->mNumFaces;++j)
            {
                const aiFace& face = mesh->mFaces[j];
                aiVector3D pos = mesh->mVertices[face.mIndices[0]];
                Point p1(pos.x, pos.y, pos.z);
                pos = mesh->mVertices[face.mIndices[1]];
                Point p2(pos.x, pos.y, pos.z);
                pos = mesh->mVertices[face.mIndices[2]];
                Point p3(pos.x, pos.y, pos.z);
                this->_triangles.push_back(Triangle(p1, p2, p3));
            }
        }
    }

    vector<Triangle>& Loader::Triangles() {
        return _triangles;
    }
}