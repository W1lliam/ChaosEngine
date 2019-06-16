#pragma once
#include <ChaosMaths.h>
namespace Chaos::Resources
{
    struct UV
    {
        float U;
        float V;
        float W;

        UV() = default;

        UV(float u, float v, float w)
        {
            U = u;
            V = v;
            W = w;
        }

    };

    struct Color
    {
        float R {0.f};
        float G {0.f};
        float B {0.f};
        float A {1.f};

        Color() = default;
        Color(float r, float g, float b, float a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }
    };

    struct Vec3
    {
        float x;
        float y;
        float z;

        Vec3() = default;

        Vec3(float X, float Y, float Z)
        {
            x = X;
            y = Y;
            z = Z;
        }
    };
	struct Vec4
	{
		float x = -1.f;
		float y = -1.f;
		float z = -1.f;
		float w = -1.f;

		Vec4() = default;
		Vec4(float X, float Y, float Z, float W)
		{
			x = X;
			y = Y;
			z = Z;
			w = W;
		}
		float& operator[](uint32_t p_indice)
		{
			switch (p_indice)
			{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			default: return x;
			}
		}
		void insert(float p_weight)
		{
			if (x == -1.f)
				x = p_weight;
			else if (y == -1.f)
				y = p_weight;
			else if (z == -1.f)
				z = p_weight;
			else
				w = p_weight;
		}
	};
	struct IVec4
	{
		int32_t x = -1;
		int32_t y = -1;
		int32_t z = -1;
		int32_t w = -1;

		IVec4() = default;
		IVec4(int32_t X, int32_t Y, int32_t Z, int32_t W)
		{
			x = X;
			y = Y;
			z = Z;
			w = W;
		}
		int32_t& operator[](uint32_t p_indice)
		{
			switch (p_indice)
			{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			default: return x;
			}
		}
		void insert(uint32_t p_index)
		{
			if (x == -1)
				x = static_cast<int32_t>(p_index);
			else if (y == -1)
				y = static_cast<int32_t>(p_index);
			else if (z == -1)
				z = static_cast<int32_t>(p_index);
			else
				w = static_cast<int32_t>(p_index);
		}
	};
    struct Vertex
    {
        Vec3 position;
        Vec3 normal;
        Vec3 tangents;
        Vec3 bitangents;

        UV uv;
        Color color;

        Vertex() {}

        Vertex(const Vertex& p_other) : 
			position(p_other.position), 
			normal(p_other.normal),
			tangents(p_other.tangents), 
			bitangents(p_other.bitangents), 
			uv(p_other.uv), 
			color(p_other.color)
        {}

    };

	struct WeightedVertex
	{
		Vec3 position;
		Vec3 normal;
		Vec3 tangents;
		Vec3 bitangents;

		UV uv;
		Color color;
		IVec4 boneIndices;
		Vec4 boneWeights;
		WeightedVertex() {}

		WeightedVertex(const WeightedVertex& p_other) :
			position(p_other.position),
			normal(p_other.normal),
			tangents(p_other.tangents),
			bitangents(p_other.bitangents),
			uv(p_other.uv),
			color(p_other.color),
			boneIndices(p_other.boneIndices),
			boneWeights(p_other.boneWeights)
		{}

		void ResetBoneIndiciesNegativity()
		{
			for (uint32_t i = 0; i < 4; ++i)
			{
				if (boneIndices[i] == -1)
					boneIndices[i] = 0;
			}
		}
		void ResetBoneWeightsNegativity()
		{
			for (uint32_t i = 0; i < 4; ++i)
			{
				if (boneWeights[i] == -1.f)
					boneWeights[i] = 0.f;
			}
		}
	};
}