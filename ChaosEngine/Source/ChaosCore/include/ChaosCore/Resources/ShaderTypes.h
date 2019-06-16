#pragma once

#include <string>
#include <ChaosMaths.h>
#include "ChaosCore/Resources/ATexture.h"
#include "ChaosCore/Resources/ASkybox.h"


namespace Chaos
{
	enum class E_ShaderType
	{
		SHADER_TYPE_SAMPLERCUBE_IRR	        = 0,
		SHADER_TYPE_SAMPLERCUBE_PREFILTER	= 1,
		SHADER_TYPE_VEC3ARRAY				= 2,
		SHADER_TYPE_VEC4ARRAY				= 3,
		SHADER_TYPE_INT				        = 0x1404,
		SHADER_TYPE_UNSIGNED_INT	        = 0x1405,
		SHADER_TYPE_FLOAT			        = 0x1406,
		SHADER_TYPE_VEC2			        = 0x8B50,
		SHADER_TYPE_VEC3			        = 0x8B51,
		SHADER_TYPE_VEC4			        = 0x8B52,
		SHADER_TYPE_BOOL			        = 0x8B56,
		SHADER_TYPE_MAT3			        = 0x8B5B,
		SHADER_TYPE_MAT4			        = 0x8B5C,
		SHADER_TYPE_SAMPLER1D		        = 0x8B5D,
		SHADER_TYPE_SAMPLER2D		        = 0x8B5E,
		SHADER_TYPE_SAMPLER3D		        = 0x8B5F,
		SHADER_TYPE_SAMPLERCUBE		        = 0x8B60,
        SHADER_TYPE_MAX
	};

	inline constexpr bool IsSamplerType(E_ShaderType p_type)
	{
		return ((p_type == E_ShaderType::SHADER_TYPE_SAMPLER1D)	    ||
				(p_type == E_ShaderType::SHADER_TYPE_SAMPLER2D)	    ||
				(p_type == E_ShaderType::SHADER_TYPE_SAMPLER3D)	    ||
				(p_type == E_ShaderType::SHADER_TYPE_SAMPLERCUBE)   ||
				(p_type == E_ShaderType::SHADER_TYPE_SAMPLERCUBE_IRR));
	}

	enum class E_FaceCull
	{
		OFF = 0,
		FRONT = 0x0404,
		BACK = 0x0405,
	};

	enum class E_BlendMode : uint8_t
	{
		OFF,
		DEFAULT,		//SRC_ALPHA			1 - SRC_ALPHA
		PREMULTIPLY,	//1					1 - SRC_ALPHA
		ADDITIVE,		//1					1
		SOFT_ADDITIVE,	//1 - DST_COLOR		1
		MULTIPLY,		//DST_COLOR			0
		DARKER			//DST_COLOR			SRC_COLOR
	};


	struct Uniform
	{
		E_ShaderType    type;
		std::string     name;
		int             size;
		int             location;
	};

	struct UniformValue
	{
		E_ShaderType type;
		uint32_t     elementCount;
		Chaos::Memory::u_memoryPtr<int32_t> value;

		UniformValue() 
		{
			elementCount = 0;
			value.asUIntPtr = -1;
		}
		UniformValue(E_ShaderType p_type) : type(p_type) 
		{
			elementCount = 0;
            if(type == E_ShaderType::SHADER_TYPE_FLOAT)
                *reinterpret_cast<float*>(&value.asVoidPtr) = -1.0f;
            else
			    value.asUIntPtr = -1;
		}
	};

	struct UniformValueSampler
	{
		E_ShaderType	type;
        int		        slot {-1};

		union
		{
            Resources::TexturePtr	texture {nullptr};
			Resources::SkyboxPtr	cubeMap;
		};

        UniformValueSampler() {};
        UniformValueSampler(const UniformValueSampler& p_other) : type(p_other.type), slot(p_other.slot) , texture(p_other.texture) {}
        void operator=(const UniformValueSampler& p_other) { type = p_other.type; slot = p_other.slot; texture = p_other.texture; };
        ~UniformValueSampler() {};
		UniformValueSampler(E_ShaderType p_type, uint32_t p_slot) : type(p_type), slot(p_slot) {}
	};

	struct VertexAttribute
	{
		E_ShaderType  type;
		std::string  name;
		int          size;
		unsigned int location;
	};
}