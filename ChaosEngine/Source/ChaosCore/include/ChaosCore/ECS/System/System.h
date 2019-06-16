#pragma once
#include <limits>
#include <memory>
#include "ChaosCore/ECS/Types.h"
#include "ChaosCore/Utils/StringID.h"
#include "ChaosCore/Serialization/Serializer.h"

namespace Chaos
{
	using SystemPriority    = uint16_t;

    enum E_SystemExecutionModeFlags : uint8_t
    {
        DISABLED    = (0 << 0),
        EDITOR      = (1 << 0),
        GAME        = (1 << 1),
        BOTH        = EDITOR | GAME,
    };

	class System
	{
	friend class SystemManager;
    public:
	    static const SystemPriority LOWEST_SYSTEM_PRIORITY = std::numeric_limits<SystemPriority>::min();

	    static const SystemPriority VERY_LOW_SYSTEM_PRIORITY    = 99;
	    static const SystemPriority LOW_SYSTEM_PRIORITY         = 100;
	    static const SystemPriority NORMAL_SYSTEM_PRIORITY      = 200;
	    static const SystemPriority MEDIUM_SYSTEM_PRIORITY      = 300;
	    static const SystemPriority HIGH_SYSTEM_PRIORITY        = 400;
	    static const SystemPriority VERY_HIGH_SYSTEM_PRIORITY   = 401;
	    static const SystemPriority MAX_SYSTEM_PRIORITY         = std::numeric_limits<SystemPriority>::max();

	public:
		virtual ~System();

		virtual inline SystemTypeID GetSystemTypeID()   const = 0;
		virtual inline const char* GetSystemTypeName()  const = 0;

        virtual void OnEnable()                             {};
        virtual void OnDisable()                            {};
        virtual void Start()                                {};
        virtual void Stop()                                 {};
        virtual void FixedUpdate(double p_fixedDeltaTime)   {};
        virtual void PreUpdate(double dt)                   {};
        virtual void Update(double dt)                      {};
        virtual void PostUpdate(double dt)                  {};


        template<class... Dependencies>
        void AddDependencies(Dependencies&&... dependencies)
        {
            this->m_systemManager->AddSystemDependency(this, std::forward<Dependencies>(dependencies)...);
        }

		inline E_SystemExecutionModeFlags GetExecutionMode() const { return m_executionMode; }

        static  ssq::Class  Expose();
        virtual bool        Save(WriteStream& p_stream);
                bool        Load(ReadStream& p_stream);

                void        SetExecutionMode(E_SystemExecutionModeFlags p_mode);
        inline void Enable()    { if (!m_enabled) { m_enabled = true;   OnEnable();         } };
        inline void Disable()   { if (m_enabled)  { OnDisable();        m_enabled = false;  } };

	protected:
		explicit System(SystemPriority p_priority = NORMAL_SYSTEM_PRIORITY, E_SystemExecutionModeFlags p_executionMode = E_SystemExecutionModeFlags::GAME, double p_updateInterval_ms = (-1.0));

        SystemManager*              m_systemManager {nullptr};
        E_SystemExecutionModeFlags	m_executionMode : 2;
        uint8_t	                    m_needsUpdate   : 1;
        uint8_t	                    m_enabled       : 1;
        uint8_t	                    m_reserved      : 4;
    private:
        double          m_timeSinceLastUpdate   {0};
        double          m_updateInterval;
        SystemPriority  m_priority;


	};
    using SystemPtr = std::shared_ptr<System>;

#define CHAOS_DEFINE_SYSTEM_TYPEID(Type) const ::Chaos::SystemTypeID Type::SYSTEM_TYPE_ID = operator""_sid(typeid(Type).name(), strlen(typeid(Type).name()))

#define CHAOS_SYSTEM(Type)      public:                                                         \
                                static const ::Chaos::SystemTypeID SYSTEM_TYPE_ID;              \
                                                                                                \
                                virtual inline const char* GetSystemTypeName() const override   \
                                {                                                               \
                                    static const char* SYSTEM_TYPE_NAME{ typeid(Type).name() }; \
                                    return SYSTEM_TYPE_NAME;                                    \
                                }                                                               \
                                virtual inline ::Chaos::SystemTypeID GetSystemTypeID() const override    \
                                {                                                               \
                                    return SYSTEM_TYPE_ID;                                      \
                                }                                                               \

}