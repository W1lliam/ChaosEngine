#include "ChaosCore/Resources/AssetLoader.h"
#include "AssetFileConstants.h"
#include "ChaosDebug.h"



namespace Chaos::Resources
{
	AResource* AssetLoader::LoadAsset(const char* p_fileName)
	{
		if (!filesystem::is_directory("../../../ProcessedAssets"))
		{
			CHAOS_CORE_ERROR("Assets folder not found");
			return nullptr;
		}

		std::ifstream input(std::string("../../../ProcessedAssets/") + p_fileName + CHAOS_ASSET_EXTENSION, std::ios::binary | std::ios::in);
		
		if (!input.is_open())
		{
            CHAOS_CORE_ERROR("Couldn't open asset");
			input.close();
			return nullptr;
		}
		char sign[5];
		input.read(sign, 4);
		sign[4] = '\0';

		if (std::strcmp(sign, ".PNS") != 0)
		{
            CHAOS_CORE_ERROR("Asset not recognized");
			input.close();
			return nullptr;
		}

		uint8_t type;
		input.read(reinterpret_cast<char*>(&type), 1);

		uint32_t fileSize = 0;
		input.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
		uint32_t inputSize = 0;
		input.seekg(0, std::ios::end);
		inputSize = static_cast<uint32_t>(input.tellg());

		if (fileSize != inputSize)
		{
            CHAOS_CORE_ERROR("Asset is Corrupted");
			input.close();
			return nullptr;
		}

		input.seekg(9, std::ios::beg);									

        AResource* asset;

		switch (type)
		{
		case E_ASSETTYPE_MESH:
			asset = LoadMesh(input);											
			break;
		}

		input.close();

		return asset;
	}

    bool AssetLoader::Validate(std::ifstream& p_fileStream, AssetType p_expectedType)
    {
        CHAOS_CORE_ASSERT(p_fileStream.is_open(), "Asset file has not been opened");

        char sign[5];
        p_fileStream.read(sign, 4);
        sign[4] = '\0';

        if (std::strcmp(sign, ".PNS") != 0)
        {
            CHAOS_CORE_ERROR("Asset not recognized");
            p_fileStream.close();
            return false;
        }

        AssetType type;
        p_fileStream.read(reinterpret_cast<char*>(&type), 1);
        if (type != p_expectedType)
        {
            CHAOS_CORE_ERROR("Asset type is different from expected type");
            p_fileStream.close();
            return false;
        }

        uint32_t fileSize = 0;
        p_fileStream.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
        uint32_t inputSize = 0;
        p_fileStream.seekg(0, std::ios::end);
        inputSize = static_cast<uint32_t>(p_fileStream.tellg());

        // resume reading
        p_fileStream.seekg(9, std::ios::beg);									


        if (fileSize != inputSize)
        {
            CHAOS_CORE_ERROR("Asset is corrupted");
            p_fileStream.close();
            return false;
        }
        return true;
    }

    Mesh* AssetLoader::LoadMesh(std::ifstream& p_input)
	{
        return nullptr;
	}
}