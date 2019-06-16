#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
    System::System(const SystemPriority p_priority, E_SystemExecutionModeFlags p_executionMode, const double p_updateInterval_ms) :
        m_priority(p_priority),
        m_updateInterval(p_updateInterval_ms),
        m_executionMode(p_executionMode),
        m_needsUpdate(true),
        m_enabled(true),
        m_reserved(0)
    {
    }

    System::~System()
    {}

    ssq::Class System::Expose()
    {
        ssq::Class cls = ScriptsHandler::Get().GetVirtualMachine().addAbstractClass<System>("System");

        cls.addFunc("OnEnable", &System::OnEnable);
        cls.addFunc("OnDisable", &System::OnDisable);

        cls.addFunc("Start", &System::Start);

        cls.addFunc("FixedUpdate", &System::FixedUpdate);
        cls.addFunc("PreUpdate", &System::PreUpdate);
        cls.addFunc("Update", &System::Update);
        cls.addFunc("PostUpdate", &System::PostUpdate);

        return cls;
    }

    bool System::Save(WriteStream& p_stream)
    {
        SerializeDouble(p_stream, m_timeSinceLastUpdate);
        SerializeDouble(p_stream, m_updateInterval);
        SerializeUnsignedInt(p_stream, m_priority, LOWEST_SYSTEM_PRIORITY, MAX_SYSTEM_PRIORITY);
        
        uint8_t execution   = m_executionMode;
        uint8_t needsUpdate = m_needsUpdate;
        uint8_t reserved    = m_reserved;

        SerializeBits(p_stream, execution,      2);
        SerializeBits(p_stream, needsUpdate,    1);
        SerializeBits(p_stream, reserved,       5);

        return true;
    }

    bool System::Load(ReadStream& p_stream)
    {
        SerializeDouble(p_stream, m_timeSinceLastUpdate);
        SerializeDouble(p_stream, m_updateInterval);
        SerializeUnsignedInt(p_stream, m_priority, LOWEST_SYSTEM_PRIORITY, MAX_SYSTEM_PRIORITY);

        uint8_t execution, needsUpdate, reserved;
        SerializeBits(p_stream, execution,      2);
        SerializeBits(p_stream, needsUpdate,    1);
        SerializeBits(p_stream, reserved,       5);

        m_executionMode = (E_SystemExecutionModeFlags)execution;
        m_needsUpdate   = needsUpdate;
        m_reserved      = reserved;
        return true;
    }

    void System::SetExecutionMode(E_SystemExecutionModeFlags p_mode) 
    {
        if (m_executionMode == DISABLED && p_mode != DISABLED)
        {
            m_executionMode = p_mode; 
            OnEnable();
        }
        else if (m_executionMode != DISABLED && p_mode == DISABLED)
        {
            OnDisable();
            m_executionMode = p_mode;
        }
        else
        {
            m_executionMode = p_mode;
        }
    }

}