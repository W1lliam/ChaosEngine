#pragma once
#include <ChaosMaths.h>
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/Resources/AShader.h"
#include "ChaosCore/Resources/ATexture.h"
#include "ChaosCore/Resources/Material.h"

#ifdef CHAOS_EDITOR
namespace ChaosEditor { class MaterialComponentGUI; }
#endif

namespace Chaos
{
    class MaterialComponent : public IComponent
    {
        CHAOS_COMPONENT
#ifdef CHAOS_EDITOR
        friend class ::ChaosEditor::MaterialComponentGUI;
#endif
    public:
        MaterialComponent(COMPONENT_INIT_PARAMS, Resources::MaterialPtr p_material = nullptr);
        ~MaterialComponent() = default;

		inline void							SetMaterial(Resources::MaterialPtr p_material) { m_material = p_material; }

		inline const Resources::MaterialPtr GetMaterial()	const { return m_material; }
		inline const Resources::MaterialPtr operator->()	const { return m_material; }

        virtual bool        Save(WriteStream& p_stream) override;
        static  IComponent& Load(ReadStream& p_stream, const EntityID& p_entityID);
        static  void        Expose();

	private:
		Resources::MaterialPtr	m_material;
    };
}