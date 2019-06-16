#pragma once
#include <cstdint>

namespace Chaos
{

	/*******************
	*  FILE EXTENSION  *
	********************/
	#define CHAOS_ASSET_EXTENSION ".chaosasset"
	#define CHAOS_SCENE_EXTENSION ".chaosscene"

	/***************
	*  ASSET TYPE  *
	****************/
	enum AssetType : uint8_t
	{
		E_ASSETTYPE_MESH = 0x00,
		E_ASSETTYPE_TEXTURE,
		E_ASSETTYPE_MATERIAL,
		E_ASSETTYPE_AUDIO,
		E_ASSETTYPE_SOUND,
		E_ASSETTYPE_SHADER,
		E_ASSETTYPE_SCENE,
		E_ASSETTYPE_MODEL,
		E_ASSETTYPE_SKELETON,
		E_ASSETTYPE_SUBMESH,
		E_ASSETTYPE_SKYBOX,
		E_ASSETTYPE_BONE,
		E_ASSETTYPE_SKELETALMESH,
		E_ASSETTYPE_ANIMATIONMESH,
		E_ASSETTYPE_ANIMATION,
        E_ASSETTYPE_COUNT,
        E_ASSETTYPE_INVALID     = 0xFF
	};

	/****************
	*  VERTEX DATA  *
	*****************/
	enum VertexData : uint8_t
	{
		E_VERTEXDATA_COORD		= 0x01,
		E_VERTEXDATA_NORM		= 0x02,
		E_VERTEXDATA_UV			= 0x04,
		E_VERTEXDATA_COLOR		= 0x08
	};

	/*****************
	*  TEXTURE DATA  *
	******************/
    enum E_TextureType : uint32_t
    {
        RGB         = 0x1907,
        RG16F       = 0x822F,
        RG32F       = 0x8230,
        RGB16F      = 0x881B,
        RGB32F      = 0x8815,
        RGBA8       = 0x8058,
        DEPTH       = 0x1902,
        DEPTH16     = 0x81A5,
        DEPTH24     = 0x81A6
    };
}
