#pragma once
#include "ChaosAssetsManager/chaos_assets_manager_pch.h"
#include "ChaosAssetsManager/export.h"

namespace Chaos::AssetsManager
{
    struct SubMeshNode;

	class CHAOS_ASSETS_MANAGER_API AssetsLoader
	{
	public:
		AssetsLoader() = delete;
		~AssetsLoader() = delete;

		static void LoadTexture(std::string p_path);
		static void LoadHDRTexture(std::string p_path, std::vector<float>& o_data, int& o_width, int& o_height, int& o_bpp);
		static void ExportTexture(void* p_image, std::string p_name, int32_t p_width, int32_t p_height, uint8_t p_bpp, E_TextureType p_textureType);
        static void ExportSkybox(std::string p_path, std::vector<float>& p_envMap, int32_t p_envSize, std::vector<float>& p_irrMap, int32_t p_irrSize, std::vector<float>& p_prefilterMap, int32_t p_prefilterSize);
		static void	VerifyAnimation(std::string path);
		static void LoadModel(const std::string& p_path, const aiScene* p_scene);
		static void LoadAnimation(const std::string& p_path, const aiScene* p_scene);

	private:

		static void ProcessNode(aiNode* p_node,
								const aiScene* p_scene,
								std::vector<aiNode*>& p_meshList);
		static void WriteFileHeader(std::ofstream& p_stream, std::string p_name, AssetType p_type);
		
		static void     ExportMesh(const std::string& p_name, const aiScene* p_scene, std::vector<aiNode*>& p_submeshes);

		static uint32_t ExportSubMesh(std::ofstream& output, const aiMesh& p_submesh);
		static void		ExportAnimation(const std::string& p_name, const aiScene* p_scene);
		//Variables
	private:
		static void     ExtractSkeletonHierarchy(aiMesh * mesh, aiNode* p_root, std::ofstream& p_output, uint32_t& p_fileSize);
		static void		ExtractAnimationMesh(const aiScene* p_scene, std::ofstream& p_output, uint32_t& p_fileSize);
		static void		ExtractAnimation(const aiScene* p_scene, std::ofstream& p_output, uint32_t& p_fileSize);
	};
}