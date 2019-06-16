#include "ChaosAssetsManager/chaos_assets_manager_pch.h"
#include "ChaosAssetsManager/AssetsLoader/AssetsLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>
#include <string>

namespace Chaos::AssetsManager
{
	void FindParentsName(aiString& p_outString, aiNode* p_node, aiMesh* p_mesh)
	{
		bool flag = false;
		if (p_node->mParent)
		{
			if (p_node->mParent->mName.length != 0)
			{
				for (uint32_t i = 0; i < p_mesh->mNumBones; ++i)
				{
					if (p_node->mParent->mName == p_mesh->mBones[i]->mName)
					{
						p_outString = p_node->mParent->mName;
						flag = true;
					}
				}
			}
			if (!flag)
			{
				FindParentsName(p_outString, p_node->mParent, p_mesh);
			}
		}
		else
		{
			p_outString = aiString("NULL");
		}
	}
}

namespace Chaos::AssetsManager
{
	
	void AssetsLoader::VerifyAnimation(std::string p_path)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(p_path, aiProcess_JoinIdenticalVertices |			// join identical vertices/ optimize indexing
			aiProcess_ValidateDataStructure |			// perform a full validation of the loader's output
			aiProcess_ImproveCacheLocality |			// improve the cache locality of the output vertices
			aiProcess_RemoveRedundantMaterials |			// remove redundant materials
			aiProcess_GenUVCoords |			// convert spherical, cylindrical, box and planar mapping to proper UVs
			aiProcess_TransformUVCoords |			// pre-process UV transformations (scaling, translation ...)
			
			aiProcess_LimitBoneWeights |			// limit bone weights to 4 per vertex
			aiProcess_OptimizeMeshes |			// join small submeshes, if possible;
		
			aiProcess_GenSmoothNormals |			// generate smooth normal vectors if not existing
			aiProcess_SplitLargeMeshes |			// split large, unrenderable submeshes into sub-submeshes
			aiProcess_Triangulate |			// triangulate polygons with more than 3 edges
			
			aiProcess_SortByPType |			// sorts entity by type
			aiProcess_CalcTangentSpace						// calculate tangent space for normal mapping
		);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		if (scene->HasAnimations())
		{
			LoadAnimation(p_path, scene);
		}
		else
		{
			LoadModel(p_path, scene);
		}
	}
	void AssetsLoader::LoadModel(const std::string& p_path, const aiScene* p_scene)
	{
		std::vector<aiNode*> submeshes{};
		ProcessNode(p_scene->mRootNode, p_scene, submeshes);

		// Write file
		ExportMesh(std::filesystem::path(p_path).filename().stem().string(), p_scene, submeshes);
	}

	void AssetsLoader::ProcessNode(aiNode* p_node, const aiScene* p_scene, std::vector<aiNode*>& p_meshList)
	{
		if (p_node->mNumMeshes)
		{
			p_meshList.push_back(p_node);
		}

		for (uint8_t i = 0; i < p_node->mNumChildren; i++)
		{
			ProcessNode(p_node->mChildren[i], p_scene, p_meshList);
		}
	}

	void AssetsLoader::WriteFileHeader(std::ofstream& output, std::string p_name, AssetType p_type)
	{
		if (!std::filesystem::is_directory("Assets"))
			std::filesystem::create_directory("Assets");

		output.open("Assets/" + p_name + CHAOS_ASSET_EXTENSION, std::ios::out | std::ios::binary);

		// HEADER //															// 9 byte
		output.write(".PNS", 4);												// signature byte
		output.put(p_type);														// asset type
		uint32_t fileSize = 0;
		output.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));	    // file size
	}

	void AssetsLoader::ExportMesh(const std::string& p_name, const aiScene* p_scene, std::vector<aiNode*>& p_submeshes)
	{
		// HEADER //
		std::ofstream output;
		WriteFileHeader(output, p_name, E_ASSETTYPE_MESH);
		uint32_t fileSize = 9;

		// DATA //
		output.write(p_name.c_str(), p_name.length() + 1);	// Mesh name
		fileSize += (uint32_t)p_name.length() + 1;

		uint32_t subMeshCount = 0;
		for (aiNode* node : p_submeshes)
			subMeshCount += node->mNumMeshes;
		output.write(reinterpret_cast<char*>(&subMeshCount), sizeof(subMeshCount));
		fileSize += sizeof(subMeshCount);

		for (aiNode* submeshNode : p_submeshes)
		{
			for (uint32_t i = 0; i < submeshNode->mNumMeshes; ++i)
			{
				output.put(E_ASSETTYPE_SUBMESH);

				uint32_t blockSize = 0;
				auto blockSizePos = output.tellp();

				// block size
				output.write(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
				blockSize = sizeof(E_ASSETTYPE_SUBMESH) + sizeof(blockSize);

				// submesh node name
				output.write(submeshNode->mName.C_Str(), submeshNode->mName.length + 1);
				blockSize += (uint32_t)submeshNode->mName.length + 1;

				// Find non-empty parent node
				aiNode* parentMesh = submeshNode->mParent;
				if (parentMesh)
				{
					while (parentMesh->mNumMeshes <= 0)
					{
						if (!parentMesh->mParent)
							break;
						parentMesh = parentMesh->mParent;
					}

					// parent node name
					output.write(parentMesh->mName.C_Str(), parentMesh->mName.length + 1);
					blockSize += (uint32_t)parentMesh->mName.length + 1;
				}
				else
				{
					// empty parent node name
					output.put('\0');
					blockSize += 1;
				}

				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.a1), sizeof(submeshNode->mParent->mTransformation.a1));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.a2), sizeof(submeshNode->mParent->mTransformation.a2));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.a3), sizeof(submeshNode->mParent->mTransformation.a3));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.a4), sizeof(submeshNode->mParent->mTransformation.a4));

				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.b1), sizeof(submeshNode->mParent->mTransformation.b1));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.b2), sizeof(submeshNode->mParent->mTransformation.b2));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.b3), sizeof(submeshNode->mParent->mTransformation.b3));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.b4), sizeof(submeshNode->mParent->mTransformation.b4));

				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.c1), sizeof(submeshNode->mParent->mTransformation.c1));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.c2), sizeof(submeshNode->mParent->mTransformation.c2));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.c3), sizeof(submeshNode->mParent->mTransformation.c3));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.c4), sizeof(submeshNode->mParent->mTransformation.c4));

				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.d1), sizeof(submeshNode->mParent->mTransformation.d1));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.d2), sizeof(submeshNode->mParent->mTransformation.d2));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.d3), sizeof(submeshNode->mParent->mTransformation.d3));
				output.write(reinterpret_cast<char*>(&submeshNode->mParent->mTransformation.d4), sizeof(submeshNode->mParent->mTransformation.d4));
				blockSize += 16 * sizeof(submeshNode->mParent->mTransformation.a1);

				blockSize += ExportSubMesh(output, *p_scene->mMeshes[submeshNode->mMeshes[i]]);

				output.seekp(blockSizePos);
				output.write(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
				output.seekp(0, std::ios::end);

				fileSize += blockSize;
			}
		}

		output.seekp(5, std::ios::beg);
		output.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
		output.close();
	}

	uint32_t AssetsLoader::ExportSubMesh(std::ofstream& output, const aiMesh& p_submesh)
	{
		uint32_t bytesWritten = 0;
		uint8_t flag = 0;
		if (p_submesh.HasPositions())
			flag |= E_VERTEXDATA_COORD;
		if (p_submesh.HasNormals())
			flag |= E_VERTEXDATA_NORM;
		if (p_submesh.HasTextureCoords(0))
			flag |= E_VERTEXDATA_UV;
		if (p_submesh.HasVertexColors(0))
			flag |= E_VERTEXDATA_COLOR;

		output.put(flag); // vertex data components
		bytesWritten += 1;

		uint8_t uvCount = p_submesh.GetNumUVChannels();
		if (flag & E_VERTEXDATA_UV)
		{
			output.write(reinterpret_cast<char*>(&uvCount), sizeof(uvCount));							// get number of
			bytesWritten += 1;
		}

		uint8_t colorCount = p_submesh.GetNumColorChannels();
		if (flag & E_VERTEXDATA_COLOR)
		{
			output.write(reinterpret_cast<char*>(&colorCount), sizeof(colorCount));
			bytesWritten += 1;
		}
		uint32_t numberOFVertex = p_submesh.mNumVertices;
		output.write(reinterpret_cast<const char*>(&numberOFVertex), sizeof(numberOFVertex));	// vertex count
		bytesWritten += sizeof(numberOFVertex);

		for (uint32_t i = 0; i < p_submesh.mNumVertices; ++i)
		{
			if (p_submesh.HasPositions())
			{
				output.write(reinterpret_cast<char*>(&p_submesh.mVertices[i].x), sizeof(p_submesh.mVertices[i].x));
				output.write(reinterpret_cast<char*>(&p_submesh.mVertices[i].y), sizeof(p_submesh.mVertices[i].y));
				output.write(reinterpret_cast<char*>(&p_submesh.mVertices[i].z), sizeof(p_submesh.mVertices[i].z));
				bytesWritten += sizeof(p_submesh.mVertices[i].x) + sizeof(p_submesh.mVertices[i].y) + sizeof(p_submesh.mVertices[i].z);
			}
			if (p_submesh.HasNormals())
			{
				output.write(reinterpret_cast<char*>(&p_submesh.mNormals[i].x), sizeof(p_submesh.mNormals[i].x));
				output.write(reinterpret_cast<char*>(&p_submesh.mNormals[i].y), sizeof(p_submesh.mNormals[i].y));
				output.write(reinterpret_cast<char*>(&p_submesh.mNormals[i].z), sizeof(p_submesh.mNormals[i].z));
				bytesWritten += 3 * sizeof(p_submesh.mNormals[i].x);

				output.write(reinterpret_cast<char*>(&p_submesh.mTangents[i].x), sizeof(p_submesh.mTangents[i].x));
				output.write(reinterpret_cast<char*>(&p_submesh.mTangents[i].y), sizeof(p_submesh.mTangents[i].y));
				output.write(reinterpret_cast<char*>(&p_submesh.mTangents[i].z), sizeof(p_submesh.mTangents[i].z));
				bytesWritten += 3 * sizeof(p_submesh.mTangents[i].x);

				output.write(reinterpret_cast<char*>(&p_submesh.mBitangents[i].x), sizeof(p_submesh.mBitangents[i].x));
				output.write(reinterpret_cast<char*>(&p_submesh.mBitangents[i].y), sizeof(p_submesh.mBitangents[i].y));
				output.write(reinterpret_cast<char*>(&p_submesh.mBitangents[i].z), sizeof(p_submesh.mBitangents[i].z));
				bytesWritten += 3 * sizeof(p_submesh.mBitangents[i].x);
			}
			for (uint8_t j = 0; j < uvCount; ++j)
			{
				uint8_t uvComponents = p_submesh.mNumUVComponents[j];
				output.write(reinterpret_cast<char*>(&uvComponents), sizeof(uvComponents));
				bytesWritten += sizeof(uvComponents);

				output.write(reinterpret_cast<char*>(&p_submesh.mTextureCoords[j][i].x), sizeof(p_submesh.mTextureCoords[j][i].x));
				output.write(reinterpret_cast<char*>(&p_submesh.mTextureCoords[j][i].y), sizeof(p_submesh.mTextureCoords[j][i].y));
				bytesWritten += 2 * sizeof(p_submesh.mTextureCoords[j][i].x);

				if (uvComponents > 2)
				{
					output.write(reinterpret_cast<char*>(&p_submesh.mTextureCoords[j][i].z), sizeof(p_submesh.mTextureCoords[j][i].z));
					bytesWritten += sizeof(p_submesh.mTextureCoords[j][i].z);
				}

			}
			for (uint8_t j = 0; j < colorCount; ++j)
			{
				output.write(reinterpret_cast<char*>(&p_submesh.mColors[j][i].r), sizeof(p_submesh.mColors[j][i].r));
				output.write(reinterpret_cast<char*>(&p_submesh.mColors[j][i].g), sizeof(p_submesh.mColors[j][i].g));
				output.write(reinterpret_cast<char*>(&p_submesh.mColors[j][i].b), sizeof(p_submesh.mColors[j][i].b));
				output.write(reinterpret_cast<char*>(&p_submesh.mColors[j][i].a), sizeof(p_submesh.mColors[j][i].a));
				bytesWritten += 4 * sizeof(p_submesh.mColors[j][i].r);
			}
		}

		output.write(reinterpret_cast<const char*>(&p_submesh.mNumFaces), sizeof(p_submesh.mNumFaces));
		bytesWritten += sizeof(p_submesh.mNumFaces);
		for (uint32_t i = 0; i < p_submesh.mNumFaces; i++)
		{
			aiFace face = p_submesh.mFaces[i];
			if (face.mNumIndices != 3)
			{
				std::cerr << "ERROR: Face: " << i << " is not a triangle" << '\n';
				continue;
			}
			for (uint32_t j = 0; j < face.mNumIndices; j++)
			{
				output.write(reinterpret_cast<char*>(&face.mIndices[j]), sizeof(face.mIndices[j]));				// write indices
				bytesWritten += sizeof(face.mIndices[j]);
			}
		}
		return bytesWritten;
	}

	void AssetsLoader::LoadTexture(std::string p_path)
	{
		int32_t width, height, bpp;

		stbi_set_flip_vertically_on_load(1);
		uint8_t* image = stbi_load(p_path.c_str(), &width, &height, &bpp, STBI_rgb_alpha);

		if (!image)
			std::cout << stbi_failure_reason() << std::endl;
		auto first = p_path.find_last_of('/') + 1;
		std::string name = p_path.substr(first, p_path.find_last_of('.') - first);

		ExportTexture(image, name, width, height, 4, E_TextureType::RGBA8);
		stbi_image_free(image);
	}

	void AssetsLoader::LoadAnimation(const std::string& p_path, const aiScene* p_scene)
	{
		ExportAnimation(std::filesystem::path(p_path).filename().stem().string(), p_scene);
	}

    void AssetsLoader::LoadHDRTexture(std::string p_path, std::vector<float>& o_data, int& o_width, int& o_height, int& o_bpp)
    {
        stbi_set_flip_vertically_on_load(1);
        float* image = stbi_loadf(p_path.c_str(), &o_width, &o_height, &o_bpp, 0);

        if (!image)
            std::cout << stbi_failure_reason() << std::endl;

        o_data.resize(o_width * o_height * o_bpp);
        memcpy(&o_data[0], image, o_width * o_height * o_bpp * sizeof(float));
        stbi_image_free(image);
    }


	void AssetsLoader::ExportTexture(void* p_image, std::string p_name, int32_t p_width, int32_t p_height, uint8_t p_bpp, E_TextureType p_textureType)
	{
		// HEADER //
		std::ofstream output;
		WriteFileHeader(output, p_name, E_ASSETTYPE_TEXTURE);
		uint32_t fileSize = 9;

		output.write(p_name.c_str(), p_name.length() + 1);									// texture name
		fileSize += (uint32_t)p_name.length() + 1;

		output.write(reinterpret_cast<char*>(&p_width), sizeof(p_width));					// texture width
		fileSize += sizeof(p_width);
		output.write(reinterpret_cast<char*>(&p_height), sizeof(p_width));					// texture height
		fileSize += sizeof(p_height);
		output.write(reinterpret_cast<char*>(&p_bpp), sizeof(p_bpp));						// texture bpp
		fileSize += sizeof(p_bpp);
		output.write(reinterpret_cast<char*>(&p_textureType), sizeof(p_textureType));				// texture dataType
		fileSize += sizeof(p_textureType);

		int dataByteCount = p_width * p_height * p_bpp;
		if (p_textureType != E_TextureType::RGBA8)
			dataByteCount *= sizeof(float);
		output.write(reinterpret_cast<char*>(p_image), dataByteCount);
		fileSize += dataByteCount;

		output.seekp(5, std::ios::beg);
		output.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
		output.close();
	}
	void AssetsLoader::ExportSkybox(std::string p_path,
		std::vector<float>& p_envMap, int32_t p_envSize,
		std::vector<float>& p_irrMap, int32_t p_irrSize,
		std::vector<float>& p_prefilterMap, int32_t p_prefilterSize)
	{
		auto first = p_path.find_last_of('/') + 1;
		std::string name = p_path.substr(first, p_path.find_last_of('.') - first);

		// HEADER //
		std::ofstream output;
		WriteFileHeader(output, name, E_ASSETTYPE_SKYBOX);
		uint32_t fileSize = 9;

		// DATA //
		output.write(name.c_str(), name.length() + 1);									                        // texture name
		fileSize += (uint32_t)name.length() + 1;

		output.write(reinterpret_cast<char*>(&p_envSize), sizeof(p_envSize));				                    // environment size
		fileSize += sizeof(p_envSize);
		output.write(reinterpret_cast<char*>(&p_irrSize), sizeof(p_irrSize));				                    // irradiance size
		fileSize += sizeof(p_irrSize);
		output.write(reinterpret_cast<char*>(&p_prefilterSize), sizeof(p_prefilterSize));				        // prefilter size
		fileSize += sizeof(p_prefilterSize);

		output.write(reinterpret_cast<char*>(&p_envMap[0]), p_envMap.size() * sizeof(float));           // environment data
		fileSize += static_cast<uint32_t>(p_envMap.size() * sizeof(float));
		output.write(reinterpret_cast<char*>(&p_irrMap[0]), p_irrMap.size() * sizeof(float));           // irradiance data
		fileSize += static_cast<uint32_t>(p_irrMap.size() * sizeof(float));
		output.write(reinterpret_cast<char*>(&p_prefilterMap[0]), p_prefilterMap.size() * sizeof(float));     // prefilter data
		fileSize += static_cast<uint32_t>(p_prefilterMap.size() * sizeof(float));


		// CLOSE FILE //
		output.seekp(5, std::ios::beg);
		output.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
		output.close();
	}

	void AssetsLoader::ExportAnimation(const std::string & p_name, const aiScene * p_scene)
	{
		std::ofstream output;
		WriteFileHeader(output, p_name, E_ASSETTYPE_SKELETALMESH);
		uint32_t fileSize = 9;

		//Data
		output.write(p_name.c_str(), p_name.length() + 1);	// Animation name
		fileSize += (uint32_t)p_name.length() + 1;
		aiNode* p_root = p_scene->mRootNode;
		
		//Skeleton hierarchy first
		uint32_t SkeletalhierarchyCount = 0;

		aiMesh* ToReturn;
		for (size_t i = 0; i < p_scene->mNumMeshes; ++i)
		{
			aiMesh * mesh = p_scene->mMeshes[i];
			if (SkeletalhierarchyCount < mesh->mNumBones)
			{
				SkeletalhierarchyCount = mesh->mNumBones;
				ToReturn = mesh;
			}
		}

		output.write(reinterpret_cast<char*>(&SkeletalhierarchyCount), sizeof(SkeletalhierarchyCount));
		fileSize += sizeof(SkeletalhierarchyCount);

		ExtractSkeletonHierarchy(ToReturn, p_root, output, fileSize);
		ExtractAnimationMesh(p_scene, output, fileSize);
		ExtractAnimation(p_scene, output, fileSize);

		output.seekp(5, std::ios::beg);
		output.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
		output.close();
	}

	void AssetsLoader::ExtractSkeletonHierarchy(aiMesh*  p_node, aiNode* p_root, std::ofstream & p_output, uint32_t & p_fileSize)
	{
		for (uint16_t i = 0; i < p_node->mNumBones; ++i)
		{
			aiBone* bone = p_node->mBones[i];
			p_output.put(E_ASSETTYPE_BONE);
			uint32_t blockSize = 0;
			auto blockSizePos = p_output.tellp();

			// block size
			p_output.write(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
			blockSize = sizeof(E_ASSETTYPE_BONE) + sizeof(blockSize);

			// Bone Name
			p_output.write(bone->mName.C_Str(), bone->mName.length + 1);
			blockSize += (uint32_t)bone->mName.length + 1;
	
			aiNode* boneNode = p_root->FindNode(bone->mName);
			aiString ParentName;
			FindParentsName(ParentName, boneNode, p_node);

			// ParentName Name
			p_output.write(ParentName.C_Str(), ParentName.length + 1);
			blockSize += (uint32_t)ParentName.length + 1;

			aiQuaternion rotation;
			aiVector3D position;
			bone->mOffsetMatrix.Inverse().DecomposeNoScaling(rotation, position);

			p_output.write(reinterpret_cast<char*>(&rotation.x), sizeof(rotation.x));
			p_output.write(reinterpret_cast<char*>(&rotation.y), sizeof(rotation.y));
			p_output.write(reinterpret_cast<char*>(&rotation.z), sizeof(rotation.z));
			p_output.write(reinterpret_cast<char*>(&rotation.w), sizeof(rotation.w));
			blockSize += 4 * sizeof(rotation.x);

			p_output.write(reinterpret_cast<char*>(&position.x), sizeof(position.x));
			p_output.write(reinterpret_cast<char*>(&position.y), sizeof(position.y));
			p_output.write(reinterpret_cast<char*>(&position.z), sizeof(position.z));
			blockSize += 3 * sizeof(position.x);

			p_output.seekp(blockSizePos);
			p_output.write(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
			p_output.seekp(0, std::ios::end);

			p_fileSize += blockSize;
		}
	}

	void AssetsLoader::ExtractAnimationMesh(const aiScene * p_scene, std::ofstream & p_output, uint32_t & p_fileSize)
	{
		uint32_t subMeshCount = p_scene->mNumMeshes;
		p_output.write(reinterpret_cast<char*>(&subMeshCount), sizeof(subMeshCount));
		p_fileSize += sizeof(subMeshCount);

		for (size_t i = 0; i < p_scene->mNumMeshes; ++i)
		{
			p_output.put(E_ASSETTYPE_ANIMATIONMESH);

			uint32_t blockSize = 0;
			auto blockSizePos = p_output.tellp();

			// block size
			p_output.write(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
			blockSize = sizeof(E_ASSETTYPE_ANIMATIONMESH) + sizeof(blockSize);
			aiMesh* pMesh = p_scene->mMeshes[i];

			p_output.write(pMesh->mName.C_Str(), pMesh->mName.length + 1);	// Animation name
			blockSize += (uint32_t)pMesh->mName.length + 1;

			blockSize += ExportSubMesh(p_output, *pMesh);

			uint32_t boneSize = pMesh->mNumBones;
			p_output.write(reinterpret_cast<char*>(&boneSize), sizeof(boneSize));
			blockSize += sizeof(boneSize);

			for (uint32_t i = 0; i < pMesh->mNumBones; ++i)
			{
				aiBone* bone = pMesh->mBones[i];

				p_output.write(bone->mName.C_Str(), bone->mName.length + 1);
				blockSize += (uint32_t)bone->mName.length + 1;

				uint32_t weightSize = bone->mNumWeights;
				p_output.write(reinterpret_cast<char*>(&weightSize), sizeof(weightSize));
				blockSize += sizeof(weightSize);
				for (size_t i = 0; i < bone->mNumWeights; ++i)
				{
					uint32_t vertexId = bone->mWeights[i].mVertexId;
					p_output.write(reinterpret_cast<char*>(&vertexId), sizeof(vertexId));
					blockSize += sizeof(vertexId);
				
					float weight = bone->mWeights[i].mWeight;
					p_output.write(reinterpret_cast<char*>(&weight), sizeof(weight));
					blockSize += sizeof(weight);
				}
			}
			p_output.seekp(blockSizePos);
			p_output.write(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
			p_output.seekp(0, std::ios::end);
			p_fileSize += blockSize;
		}
	}

	void Chaos::AssetsManager::AssetsLoader::ExtractAnimation(const aiScene * p_scene, std::ofstream & p_output, uint32_t & p_fileSize)
	{
		if (p_scene->mNumAnimations > 0)
		{
			uint32_t animCount = p_scene->mNumAnimations;
			p_output.write(reinterpret_cast<char*>(&animCount), sizeof(animCount));
			p_fileSize += sizeof(animCount);
			for (size_t i = 0; i < animCount; ++i)
			{
				p_output.put(E_ASSETTYPE_ANIMATION);

				uint32_t blockSize = 0;
				auto blockSizePos = p_output.tellp();

				// block size
				p_output.write(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
				blockSize = sizeof(E_ASSETTYPE_ANIMATION) + sizeof(blockSize);

				aiAnimation* pAnimation = p_scene->mAnimations[i];
				//Animation name
				p_output.write(pAnimation->mName.C_Str(), pAnimation->mName.length + 1);
				blockSize += (uint32_t)pAnimation->mName.length + 1;

				//Animation Duration
				double AnimDuration = pAnimation->mDuration;
				p_output.write(reinterpret_cast<char*>(&AnimDuration), sizeof(AnimDuration));
				blockSize += sizeof(AnimDuration);

				//Animation TicksPerSeconds
				p_output.write(reinterpret_cast<char*>(&pAnimation->mTicksPerSecond), sizeof(pAnimation->mTicksPerSecond));
				blockSize += sizeof(pAnimation->mTicksPerSecond);

				//size of total poses
				uint32_t chanelSize = pAnimation->mNumChannels;
				p_output.write(reinterpret_cast<char*>(&chanelSize), sizeof(chanelSize));
				blockSize += sizeof(chanelSize);

				for (size_t i = 0; i < pAnimation->mNumChannels; ++i)
				{
					aiNodeAnim* pChannel = pAnimation->mChannels[i];
					p_output.write(pChannel->mNodeName.C_Str(), pChannel->mNodeName.length + 1);
					blockSize += (uint32_t)pChannel->mNodeName.length + 1;

					uint32_t channel = pChannel->mNumPositionKeys;
					p_output.write(reinterpret_cast<char*>(&channel), sizeof(channel));
					blockSize += sizeof(channel);
					for (unsigned int i = 0; i < pChannel->mNumPositionKeys; ++i)
					{
						p_output.write(reinterpret_cast<char*>(&pChannel->mPositionKeys[i].mTime), sizeof(pChannel->mPositionKeys[i].mTime));
						blockSize += sizeof(pChannel->mPositionKeys[i].mTime);

						p_output.write(reinterpret_cast<char*>(&pChannel->mPositionKeys[i].mValue.x), sizeof(pChannel->mPositionKeys[i].mValue.x));
						p_output.write(reinterpret_cast<char*>(&pChannel->mPositionKeys[i].mValue.y), sizeof(pChannel->mPositionKeys[i].mValue.y));
						p_output.write(reinterpret_cast<char*>(&pChannel->mPositionKeys[i].mValue.z), sizeof(pChannel->mPositionKeys[i].mValue.z));
						blockSize += 3 * sizeof(pChannel->mPositionKeys[i].mValue.x);
					}
					uint32_t channelRot = pChannel->mNumRotationKeys;
					p_output.write(reinterpret_cast<char*>(&channelRot), sizeof(channelRot));
					blockSize += sizeof(channelRot);
					for (unsigned int i = 0; i < pChannel->mNumRotationKeys; ++i)
					{
						p_output.write(reinterpret_cast<char*>(&pChannel->mRotationKeys[i].mTime), sizeof(pChannel->mRotationKeys[i].mTime));
						blockSize += sizeof(pChannel->mPositionKeys[i].mTime);

						p_output.write(reinterpret_cast<char*>(&pChannel->mRotationKeys[i].mValue.x), sizeof(pChannel->mRotationKeys[i].mValue.x));
						p_output.write(reinterpret_cast<char*>(&pChannel->mRotationKeys[i].mValue.y), sizeof(pChannel->mRotationKeys[i].mValue.y));
						p_output.write(reinterpret_cast<char*>(&pChannel->mRotationKeys[i].mValue.z), sizeof(pChannel->mRotationKeys[i].mValue.z));
						p_output.write(reinterpret_cast<char*>(&pChannel->mRotationKeys[i].mValue.w), sizeof(pChannel->mRotationKeys[i].mValue.w));
						blockSize += 4 * sizeof(pChannel->mPositionKeys[i].mValue.x);
					}
				}
				p_output.seekp(blockSizePos);
				p_output.write(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
				p_output.seekp(0, std::ios::end);
				p_fileSize += blockSize;
			}
		}
	}
}