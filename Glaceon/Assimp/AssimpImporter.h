#ifndef ASSIMPIMPORTER_H
#define ASSIMPIMPORTER_H

struct aiScene;

namespace glaceon
{
    class AssimpImporter
    {
    public:
        static bool ImportObjectModel(const std::string& pFile);
        static bool DoSceneProcessing(const aiScene* scene);
    };
} // glaceon

#endif //ASSIMPIMPORTER_H
