#pragma once
#include "ChaosCore/chaos_core_pch.h"


namespace Chaos::Resources 
{
	class ATexture;
	using TexturePtr = std::shared_ptr<ATexture>;
}

namespace Chaos::Graphics::Buffers
{
	class IFrameBuffer
	{
	public:
		virtual												~IFrameBuffer()															= default;
		virtual void										Bind()					const											= 0;
		virtual void										BindForDrawing()		const											= 0;
		virtual void										BindForReading()		const											= 0;
		virtual void										BindTexture()			const											= 0;
		virtual void										BindTextures()			const											= 0;
		virtual bool										CheckCompleteStatus()	const											= 0;
		virtual void										Unbind()				const											= 0;
		virtual Resources::TexturePtr						GetTexture()															= 0;
		virtual std::vector<Chaos::Resources::TexturePtr>	GetTextures()															= 0;

		virtual	void										SetTexture(Chaos::Resources::TexturePtr p_texture)						= 0;

		static void	            Bind(uint32_t p_frameBufferID);
		static IFrameBuffer*	Create(Chaos::Resources::TexturePtr p_texture);
		static IFrameBuffer*	Create(std::vector<Chaos::Resources::TexturePtr> p_textures);
	};
}