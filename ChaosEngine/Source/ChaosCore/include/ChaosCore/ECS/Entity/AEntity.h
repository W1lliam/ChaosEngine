#pragma once

#include "ChaosCore/ECS/Types.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/Serialization/Serializer.h"

namespace Chaos
{
	class AEntity : public ISerializable
	{
		friend class EntityManager;
		friend class Scene;

	public:
        AEntity(const EntityID& p_id);

        virtual ~AEntity()
        {
        };

        virtual void OnEnable() {}
        virtual void OnDisable() {}

        virtual bool Save(WriteStream& p_stream) override;
        virtual bool Load(ReadStream& p_stream) override;

                void SetActive(const bool p_active);

        inline IComponent&  LoadComponent(ReadStream& p_stream, const ComponentTypeID& p_typeID) { return ComponentManager::Get().LoadComponent(p_stream, this->m_entityID, p_typeID); }

		template<typename T, typename ...Args>
		T& AddComponent(Args&&... p_params)                         { return ComponentManager::Get().AddComponent<T>(this->m_entityID, std::forward<Args>(p_params)...); }
        
        template<typename ...Args>
		ScriptedComponent& AddScriptedComponent(const std::string& p_componentTypeName, Args&&... p_params)     { return ComponentManager::Get().AddScriptedComponent(this->m_entityID, p_componentTypeName, std::forward<Args>(p_params)...); }
        
		template<typename T>
		void RemoveComponent()                                      { return ComponentManager::Get().RemoveComponent<T>(this->m_entityID);}
		inline void RemoveComponent(ComponentTypeID p_type)			{ return ComponentManager::Get().RemoveComponent(this->m_entityID, p_type);}
        
        template<typename T>
        T* GetComponent() const                                     { return ComponentManager::Get().GetComponent<T>(this->m_entityID); }

        inline ScriptedComponent* GetScriptedComponent(const std::string& p_componentTypeName) const { return ComponentManager::Get().GetScriptedComponent(this->m_entityID, p_componentTypeName); }
        
        inline IComponent* GetComponent(const ComponentID& p_componentID) const { return ComponentManager::Get().GetComponent(this->m_entityID, p_componentID); }
        
        void Rename(const std::string& p_newName);
        void Rename(const EntityID& p_newName);

        inline std::unordered_map<ComponentTypeID, ComponentID>&    GetAllComponents()  const   { return ComponentManager::Get().GetAllComponents(this->m_entityID);}
		inline  const EntityID&                                     GetEntityID()       const   { return m_entityID; }
		inline  bool                                                IsActive()          const   { return m_active; }
        inline std::string                                          GetFullName()       const   { return m_entityID.string; }

		inline  bool operator==(const AEntity& p_other)     const   { return this->m_entityID == p_other.m_entityID; }
		inline  bool operator!=(const AEntity& p_other)     const   { return this->m_entityID != p_other.m_entityID; }
		inline  bool operator==(const AEntity* p_other)     const   { return this->m_entityID == p_other->m_entityID; }
		inline  bool operator!=(const AEntity* p_other)     const   { return this->m_entityID != p_other->m_entityID; }

        static void Expose();

	protected:
		EntityID    m_entityID;
		bool        m_active;
	};
    using EntityPtr = std::shared_ptr<AEntity>;
}