#pragma once

#include "ChaosCore/chaos_core_pch.h"

namespace Chaos::Functions
{
	class FunctionWrapper
	{
		struct FuncBase {
			virtual void Call() = 0;
			virtual ~FuncBase() = default;
		};

		template<typename F>
		struct FuncType : FuncBase
		{
			F f;
			FuncType(F&& f_) : f(std::move(f_)) {}
			void Call() override { f(); }
		};

	public:
		FunctionWrapper() = default;
			
		explicit FunctionWrapper(FunctionWrapper&& other) noexcept : m_func(std::move(other.m_func))
		{}
			
		template<typename F>
		FunctionWrapper(F&& f) : m_func(new FuncType<F>(std::move(f)))
		{}
			

		FunctionWrapper& operator=(FunctionWrapper&& other) noexcept
		{
			m_func = std::move(other.m_func);
			return *this;
		}

		void operator()() const
		{
			m_func->Call();
		}

		FunctionWrapper(const FunctionWrapper&) = delete;
		FunctionWrapper(const FunctionWrapper&&) = delete;
		FunctionWrapper& operator=(const FunctionWrapper&) = delete;

    private:
        std::unique_ptr<FuncBase> m_func;

	};
}