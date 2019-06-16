#pragma once
#include "ChaosCore/Resources/Buffers/IFrameBuffer.h"
#include "ChaosCore/Resources/ATexture.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Events/ApplicationEvents.h"

#include <vector>

namespace Chaos::Graphics::Buffers
{
	class OpenGLFrameBuffer : public IFrameBuffer, Events::EventListener
	{
	public:

		OpenGLFrameBuffer(Chaos::Resources::TexturePtr p_texture);
		OpenGLFrameBuffer(std::vector<Chaos::Resources::TexturePtr> p_texture);

		~OpenGLFrameBuffer() override;
		// Inherited via IFrameBuffer
		virtual void										Bind()					const override;
		virtual void										BindForDrawing()		const override;
		virtual void										BindForReading()		const override;
		virtual void										BindTexture()			const override;
		virtual void										BindTextures()			const override;
		virtual bool										CheckCompleteStatus()	const override;
		virtual void										Unbind()				const override;

		virtual void										SetTexture(Chaos::Resources::TexturePtr p_texture)			 override;

		virtual Resources::TexturePtr						GetTexture()				  override;

		virtual std::vector<Chaos::Resources::TexturePtr>	GetTextures()				  override;

		bool												OnWindowResize(const Events::WindowResizeEvent& p_event);

	private:
		unsigned int m_rendererID	{ 0 };
		unsigned int m_depthID		{ 0 };
		unsigned int m_rboID		{ 0 };

		Chaos::Resources::TexturePtr m_texture	{ nullptr };
		
		std::vector<Chaos::Resources::TexturePtr> m_textures;
	};
}