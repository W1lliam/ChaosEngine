#pragma once
#include "ChaosCore/Resources/Buffers/IUniformBuffer.h"

namespace Chaos::Graphics::Buffers
{
	class OpenGLUniformBuffer: public IUniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint8_t p_slot, const size_t p_size);
		~OpenGLUniformBuffer() override;
		// Inherited via IUniformBuffer
		virtual void    Bind() const override;
		virtual void    Bind(uint8_t p_slot) override;
		virtual void    SetSlot(uint8_t p_slot) override;
		virtual void    Unbind() const override;
		virtual void    UpdateData(uint32_t p_padding, size_t p_dataSize, void* p_data) const override;
		

	private:
		unsigned int m_rendererID{ 0 };
		uint32_t m_slot;
	};
}