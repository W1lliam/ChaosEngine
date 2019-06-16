#pragma once
#include "ChaosCore/chaos_core_pch.h"


namespace Chaos::Graphics::Buffers
{
	class IUniformBuffer
	{
	public:
		virtual ~IUniformBuffer()				= default;
		virtual void Bind() const				= 0;
		virtual void Bind(uint8_t p_slot)		= 0;
		virtual void Unbind() const				= 0;
		virtual void SetSlot(uint8_t p_slot)	= 0;

		virtual void UpdateData(uint32_t p_padding, size_t p_dataSize, void* p_data) const = 0;


		static IUniformBuffer* Create(uint8_t p_slot, const size_t p_size);
	};
}