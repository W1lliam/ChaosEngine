#pragma once
#include "ChaosCore/chaos_core_pch.h"
#include "AssetFileConstants.h"
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Resources/Mesh.h"

namespace Chaos
{
    namespace Resources
    {
        class AssetLoader
        {
        public:
            AssetLoader() = delete;
            ~AssetLoader() = delete;

            static AResource*   LoadAsset(const char* p_fileName);
            static bool         Validate(std::ifstream& p_fileStream, AssetType p_expectedType);

        private:

            static Mesh* LoadMesh(std::ifstream& p_input);

        };
    }
}