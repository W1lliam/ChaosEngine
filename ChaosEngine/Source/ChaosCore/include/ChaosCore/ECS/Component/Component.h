#pragma once

#include "ChaosCore/ECS/Types.h"
#include "ChaosDebug.h"
#include "ChaosCore/Serialization/Serializer.h"
#include "ChaosCore/Utils/StringID.h"
#include <memory>

#define COMPONENT_INIT_PARAMS const ::Chaos::EntityID& p_ownerID, const ::Chaos::ComponentID& p_componentID
#define COMPONENT_INIT(type) ::Chaos::IComponent(p_ownerID, p_componentID)

namespace Chaos
{
	class IComponent
	{
		friend class ComponentManager;

	public:
        IComponent(const EntityID& p_owner_ID, const ComponentID& p_componentID);
        virtual ~IComponent() = default;

		virtual ComponentTypeID     GetComponentTypeID()    const = 0;
        virtual bool                Save(WriteStream& p_stream)   { return true; };
		
        inline  void                SetActive(bool p_active)      { m_enabled = p_active; }

		inline  const ComponentID&  GetComponentID()        const { return this->m_component_ID; }
		inline  const EntityID&     GetOwnerID()            const { return this->m_owner_ID; }
		inline  bool                IsActive()              const { return this->m_enabled; }


		inline bool operator==(const IComponent& p_other) const { return this->m_component_ID == p_other.m_component_ID; }
		inline bool operator!=(const IComponent& p_other) const { return this->m_component_ID != p_other.m_component_ID; }
		inline bool operator==(const IComponent* p_other) const { return this->m_component_ID == p_other->m_component_ID; }
		inline bool operator!=(const IComponent* p_other) const { return this->m_component_ID != p_other->m_component_ID; }

        static void Expose();


    protected:
        EntityID    m_owner_ID;
        ComponentID m_component_ID;
        bool        m_enabled;
	};

    using ComponentPtr = std::shared_ptr<IComponent>;

#define CHAOS_DEFINE_COMPONENT_TYPEID(Type) const ::Chaos::ComponentTypeID Type::COMPONENT_TYPE_ID = operator""_sid(typeid(Type).name(), strlen(typeid(Type).name()))

#define CHAOS_COMPONENT public:                                                                                     \
                        static const ::Chaos::ComponentTypeID COMPONENT_TYPE_ID;                                    \
                        ::Chaos::ComponentTypeID GetComponentTypeID() const override { return COMPONENT_TYPE_ID; }
}