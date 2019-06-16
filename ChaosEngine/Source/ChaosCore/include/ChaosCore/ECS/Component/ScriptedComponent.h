#pragma once
#include <ChaosMaths.h>
#include "ChaosCore/ECS/Component/Component.h"


namespace Chaos
{
    class ScriptedComponent : public IComponent
    {
    public:
        static const ComponentTypeID COMPONENT_TYPE_ID;

        ScriptedComponent(COMPONENT_INIT_PARAMS, const std::string& p_typeName);
        ~ScriptedComponent();

        template<typename ...Args>
        void Init(Args&&... p_params)
        {
            m_sqInstance = m_vm.newInstance(m_vm.findClass(m_typeName.c_str()), (IComponent*)(this), std::forward<Args>(p_params)...);
        };


        virtual bool        Save(WriteStream& p_stream) override;
        static  IComponent& Load(ReadStream& p_stream, const EntityID& p_entityID);

        ComponentTypeID     GetComponentTypeID() const override { return m_typeID; }
        ssq::Instance&      GetSqInstance()                     { return m_sqInstance; }

        static void         Expose();

    private:
        ComponentTypeID     m_typeID;
        std::string         m_typeName;
        ssq::VM&            m_vm;
        ssq::Instance       m_sqInstance;

        ssq::Function*      m_saveFunc      {nullptr};
        ssq::Function*      m_loadFunc      {nullptr};
    };
}