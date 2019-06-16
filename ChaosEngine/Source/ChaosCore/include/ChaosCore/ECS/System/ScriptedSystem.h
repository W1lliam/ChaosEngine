#pragma once
#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Events/KeyEvents.h"

namespace Chaos
{

    class ScriptedSystem : public System, public Events::EventListener
    {
    public:
        static const SystemTypeID SYSTEM_TYPE_ID;

        ScriptedSystem(ssq::VM& p_vm);
        ~ScriptedSystem();

        virtual void Start()                                override;
        virtual void Stop()                                 override;
        virtual void FixedUpdate(double p_fixedDeltaTime)   override;
        virtual void PreUpdate(double p_dt)                 override;
        virtual void Update(double p_dt)                    override;
        virtual void PostUpdate(double p_dt)                override;

        virtual bool OnKeyPressed(const Events::KeyPressedEvent& p_event);
        virtual bool OnKeyReleased(const Events::KeyReleasedEvent& p_event);

        virtual inline const char* GetSystemTypeName() const override  
        {
            return m_typeName.c_str();
        }

        virtual inline SystemTypeID GetSystemTypeID() const override    
        {
            return m_typeID;
        }

        virtual bool        Save(WriteStream& p_stream);
        static System&      Load(ReadStream& p_stream, SystemTypeID p_systemTypeID, const std::string& p_systemName);

    private:
        ssq::Function*      FetchFunction(const std::string& p_functionName);

        SystemTypeID        m_typeID;
        std::string         m_typeName;
        ssq::VM&            m_vm;
        ssq::Instance       m_sqInstance;

        ssq::Function*      m_startFunc             {nullptr};
        ssq::Function*      m_stopFunc              {nullptr};

        ssq::Function*      m_fixedUpdateFunc       {nullptr};
        ssq::Function*      m_preUpdateFunc         {nullptr};
        ssq::Function*      m_updateFunc            {nullptr};
        ssq::Function*      m_postUpdateFunc        {nullptr};

        ssq::Function*      m_onKeyPressedFunc      {nullptr};
        ssq::Function*      m_onKeyReleasedFunc     {nullptr};
    };
}