#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "ChaosCore/ECS/System/ScriptedSystem.h"

namespace Chaos
{
    class ScriptsHandler : public Singleton<ScriptsHandler>
    {
    public:
        ScriptsHandler();
        ~ScriptsHandler();

        void CompileComponents();
        void CompileSystems();
        void CompileComponent(const std::string& p_componentName);
        ScriptedSystem* CompileSystem(const std::string& p_systemName);
        inline ssq::VM& GetVirtualMachine() { return *m_virtualMachine; }

    private:
        void CompileAndRunFolder(const filesystem::path& p_path, bool p_registerAsSystems = false);

        void ExposeLogger();
        void ExposeVector3();
        void ExposeKeyCodes();
        std::unique_ptr<ssq::VM> m_virtualMachine    {nullptr};
    };
}