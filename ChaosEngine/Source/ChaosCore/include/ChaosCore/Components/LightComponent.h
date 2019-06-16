#pragma once
#include "ChaosCore/ECS/Component/Component.h"
#include <ChaosMaths.h>

#ifdef CHAOS_EDITOR
namespace ChaosEditor{ class LightComponentGUI; }
#endif

namespace Chaos
{
    //uint32_t for UBO std140 alignment
	enum class E_LightType : uint32_t
	{
		DIRECTIONAL,
		SPOT,
        TYPE_COUNT
	};

    struct ShadowInfo
    {
        Maths::Matrix4 projection   {};
        Maths::Matrix4 view         {};

        ShadowInfo() = default;
        ShadowInfo(const Maths::Matrix4& p_projection, const Maths::Matrix4& p_view) : projection(p_projection), view(p_view) {}
        ShadowInfo(const ShadowInfo& p_other) : projection(p_other.projection), view(p_other.view) {}
    };

	class LightComponent : public IComponent
	{
        CHAOS_COMPONENT
#ifdef CHAOS_EDITOR
        friend class ::ChaosEditor::LightComponentGUI;
#endif
#pragma region Methods
	public:
		LightComponent(COMPONENT_INIT_PARAMS);
		
        inline  void				SetShadowInfo(const Maths::Matrix4& p_proj, 
                                                  const Maths::Matrix4& p_view) { m_shadowInfo.projection = p_proj; m_shadowInfo.view = p_view;             }
        inline  void				SetShadowInfo(const ShadowInfo& p_shadow)   { m_shadowInfo  = p_shadow;                                                 }
        inline  void				SetType(E_LightType p_type)                 { m_type        = p_type;                               m_isDirty = true;   }
		inline  void				SetIntensity(float p_intensity)             { m_intensity   = p_intensity;                          m_isDirty = true;   }
		inline  void			    SetColor(Maths::Vector4 p_color)            { m_color       = p_color;                              m_isDirty = true;   }
		inline  void			    SetAngle(float p_angle)                     { m_angle       = std::clamp(p_angle, 0.0f, 180.0f);    m_isDirty = true;   }
		inline  void			    SetFalloff(float p_falloff)                 { m_falloff     = std::clamp(p_falloff, 1.0f, 25.0f);   m_isDirty = true;   }
		inline  void 			    SetRange(float p_range)                     { m_range       = p_range;                              m_isDirty = true;   }
        inline  void			    SetCastShadows(bool p_castShadows)          { m_castShadows = p_castShadows;                        m_isDirty = true;   }
		inline  void			    SetDirty(bool p_dirty)                      { m_isDirty     = p_dirty;                                                  }

		inline  const ShadowInfo&	GetShadowInfo()                     const   { return m_shadowInfo;                                                      }
		inline  const E_LightType&	GetType()                           const   { return m_type;                                                            }
		inline  float				GetIntensity()                      const   { return m_intensity;                                                       }
		inline  Maths::Vector4	    GetColor()                          const   { return Maths::Vector4(m_color.x, m_color.y, m_color.z, m_intensity);      }
		inline  float			    GetAngle()                          const   { return m_angle;                                                           }
		inline  float			    GetFalloff()                        const   { return std::clamp(m_angle + m_falloff, 0.0f, 180.0f);                     }
		inline  float			    GetRange()                          const   { return m_range;                                                           }
		inline  bool			    CastShadows()                       const   { return m_castShadows;                                                     }
		inline  bool			    IsDirty()                           const   { return m_isDirty;                                                         }

        virtual bool        Save(WriteStream& p_stream) override;
        static IComponent&  Load(ReadStream& p_stream, const EntityID& p_entityID);

        static void Expose();

#pragma endregion

#pragma region Variables
	private:
        ShadowInfo      m_shadowInfo    {};
		E_LightType		m_type			{ E_LightType::DIRECTIONAL					};
		float			m_intensity		{ 1.0f										};
		Maths::Vector4	m_color			{ Maths::Vector4(1.0f, 0.874f, 0.0f, 1.0f)	};
		float			m_angle			{ 180.0f									};
		float			m_falloff		{ 5.0f										};
		float			m_range			{ 10.0f										};
		bool			m_castShadows	{ false										};
		bool			m_isDirty		{ true										};

#pragma endregion
	};
}