#include "ChaosCore/Systems/LightSystem.h"
#include "ChaosCore/Components/LightComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Systems/SceneSystem.h"

#include "ChaosCore/Systems/CameraSystem.h"
#include "ChaosCore/Resources/Managers/ShaderManager.h"
#include "ChaosCore/Systems/SceneSystem.h"

namespace Chaos
{
    CHAOS_DEFINE_SYSTEM_TYPEID(LightSystem);

	const std::map<float, Attenuation> LightSystem::ATTENUATION
	{
		{	7.0f,		{	0.7f,    1.8f		}	},
		{	13.0f,		{	0.35f,   0.44f		}	},
		{	20.0f,		{	0.22f,   0.20f		}	},
		{	32.0f,		{	0.14f,   0.07f		}	},
		{	50.0f,		{	0.09f,   0.032f		}	},
		{	65.0f,		{	0.07f,   0.017f		}	},
		{	100.0f,		{	0.045f,  0.0075f	}	},
		{	160.0f,		{	0.027f,  0.0028f	}	},
		{	200.0f,		{	0.022f,  0.0019f	}	},
		{	325.0f,		{	0.014f,  0.0007f	}	},
		{	600.0f,		{	0.007f,  0.0002f	}	},
		{	3250.0f,	{	0.0014f, 0.000007f	}	}
	};

	Attenuation LightSystem::GetAttenuation(float p_range)
	{
		auto& it = ATTENUATION.lower_bound(p_range);

		if (it == ATTENUATION.begin())
		{
			return it->second;
		}

		if (it == ATTENUATION.end())
		{
			return ATTENUATION.rbegin()->second;
		}

		auto prev = std::prev(it, 1);

		float diff = it->first - prev->first;
		float ratio = (p_range - prev->first) / diff;

		return { prev->second.linear + ratio * (it->second.linear - prev->second.linear), prev->second.quadratic + ratio * (it->second.quadratic - prev->second.quadratic) };
	}

    System& LightSystem::Load(ReadStream & p_stream)
    {
        LightSystem* system =  SystemManager::Get().AddSystem<LightSystem>();
        static_cast<System*>(system)->Load(p_stream);
        return *system;


    }

	LightSystem::LightSystem() : System(System::NORMAL_SYSTEM_PRIORITY, E_SystemExecutionModeFlags::BOTH)
	{
		if (!ShaderManager::Get().lightBuffer)
			ShaderManager::Get().lightBuffer	= Graphics::Buffers::IUniformBuffer::Create(1, sizeof(uint32_t) + MAX_LIGHTS * (4 * sizeof(Maths::Vector4))); // Position  Type | Color  Intensity | Direction | Linear  Quadratic  Angle
	}

	void LightSystem::Update(double p_dt)
	{
		uint32_t index{ 0 };
        auto scene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
        for (auto lightIt = scene->begin<LightComponent>(); lightIt != scene->end<LightComponent>(); lightIt++)
		{
			LightComponent*     lightComp       = static_cast<LightComponent*>(lightIt->second);
            TransformComponent* transformComp   = ComponentManager::Get().GetComponent<TransformComponent>(lightComp->GetOwnerID());
            if (transformComp)
            {
                Transform&          transform = transformComp->AsTransform();

                if (lightComp->IsDirty() || transform.IsDirty())
                {

					Maths::Matrix4  worldTransform = SceneSystem::GetWorldTransform(lightComp->GetOwnerID());
					
					Maths::Vector3 pos;

					GameObject* cam = SystemManager::Get().GetSystem<CameraSystem>()->GetMainCameraGameObject();

					if (lightComp->GetType() == E_LightType::DIRECTIONAL && cam)
					{
						Maths::Matrix4  camTransform = SceneSystem::GetWorldTransform(SystemManager::Get().GetSystem<CameraSystem>()->GetMainCameraGameObject()->GetEntityID()); // getcamera;
						pos = { camTransform.m03, camTransform.m13, camTransform.m23 };
						pos -= cam->GetComponent<TransformComponent>()->AsTransform().Forward() * 10.0f;
					}
					else
					{
						pos = { worldTransform.m03, worldTransform.m13, worldTransform.m23 };
					}

                    worldTransform.m03 = 0;
                    worldTransform.m13 = 0;
                    worldTransform.m23 = 0;
                    Maths::Vector3 lightDir = transform.GetRotationQuaternion().RotateAntiClockWise(Maths::Vector3(0, 0, 1));
                    lightDir.Normalize();

                    uint32_t offset = index * (4 * sizeof(Maths::Vector4));

                    Maths::Vector3 posType{ pos };
                    posType.empty = static_cast<float>(lightComp->GetType());
                    ShaderManager::Get().lightBuffer->UpdateData(offset + 0, sizeof(Maths::Vector4), posType.elements);
                    ShaderManager::Get().lightBuffer->UpdateData(offset + sizeof(Maths::Vector4), sizeof(Maths::Vector4), lightComp->GetColor().elements);

                    lightDir.empty = static_cast<float>(lightComp->CastShadows());
                    ShaderManager::Get().lightBuffer->UpdateData(offset + 2 * sizeof(Maths::Vector4), sizeof(Maths::Vector4), lightDir.elements);

                    Maths::Vector4 LQAF{ LightSystem::GetAttenuation(lightComp->GetRange()).linear,  LightSystem::GetAttenuation(lightComp->GetRange()).quadratic, lightComp->GetAngle(), lightComp->GetFalloff() };
                    ShaderManager::Get().lightBuffer->UpdateData(offset + 3 * sizeof(Maths::Vector4), sizeof(Maths::Vector4), LQAF.elements);
                    lightComp->SetDirty(false);

                    if (lightComp->CastShadows())
                    {
                        switch (lightComp->GetType())
                        {
                        case E_LightType::SPOT:
                            if (lightComp->GetAngle() < 180)
                            {
                                lightComp->SetShadowInfo(Maths::Matrix4::CreatePerspective(45.0f, 1.0f, 0.1f, lightComp->GetRange()),
                                    Maths::Matrix4::CreateLookAt(posType.x, posType.y, posType.z, lightDir.x, lightDir.y, lightDir.z, 0.0f, 1.0f, 0.0f));
                            }
                            else
                            {

                            }
                            break;
                        case E_LightType::DIRECTIONAL:
                        default:
                            lightComp->SetShadowInfo(Maths::Matrix4::CreateOrthographic(20.0f, 1.0f, 0.1f, 100),
                                Maths::Matrix4::CreateLookAt(posType.x, posType.y, posType.z, lightDir.x, lightDir.y, lightDir.z, 0.0f, 1.0f, 0.0f));
                            break;
                        }
                    }
                }
            }
			index++;
			index = std::min(index, (uint32_t)MAX_LIGHTS - 1);
			lightComp->SetDirty(true);
		};
		ShaderManager::Get().lightBuffer->UpdateData(MAX_LIGHTS * (4 * sizeof(Maths::Vector4)), sizeof(uint32_t), &index);
	}
}