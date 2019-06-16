/*************************
 *      INTERFACES       *
 *************************/
#include "ChaosCore/Context/IWindow.h"
#include "ChaosCore/Rendering/IRenderer.h"
 
 /*************************
 *      CONTAINERS       *
 *************************/
#include "ChaosCore/Containers/RHHashTable.h"
 
 /*************************
 *        MUTEXES        *
 *************************/
#include "ChaosCore/Multithreading/Mutexes/Spinlock.h"
#include "ChaosCore/Multithreading/Mutexes/SharedSpinlock.h"

/*************************
 *         LOCKS         *
 *************************/
#include "ChaosCore/Multithreading/Locks/LockGuard.h"
#include "ChaosCore/Multithreading/Locks/SharedLock.h"
#include "ChaosCore/Multithreading/Locks/UniqueLock.h"

/*************************
 * THREADSAFE CONTAINERS *
 *************************/
#include "ChaosCore/Multithreading/Containers/ThreadSafeArray.h"
#include "ChaosCore/Multithreading/Containers/ThreadSafeList.h"
#include "ChaosCore/Multithreading/Containers/ThreadSafeMap.h"

/*************************
 *  LOCKFREE CONTAINERS  *
 *************************/
#include "ChaosCore/Multithreading/Containers/LockFree/LFQueue.h"
#include "ChaosCore/Multithreading/Containers/LockFree/LFStack.h"

/*************************
 *        THREADS        *
 *************************/
#include "ChaosCore/Multithreading/Threads/ThreadPool.h"
#include "ChaosCore/Multithreading/Threads/ThreadsJoiner.h"

/*************************
 *      ALLOCATORS       *
 *************************/
#include "ChaosCore/Memory/Allocators/AAllocator.h"
#include "ChaosCore/Memory/Allocators/LinearAllocator.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"
#include "ChaosCore/Memory/Allocators/StackAllocator.h"
#include "ChaosCore/Memory/Allocators/DoubleBufferedAllocator.h"
#include "ChaosCore/Memory/Allocators/TLSFAllocator.h"

 /*************************
  *         UTILS         *
  *************************/
#include "ChaosCore/Utils/StringID.h"
#include "ChaosCore/Utils/Singleton.h"
#include "ChaosCore/Utils/algorithm.h"
#include "ChaosCore/Utils/Timer.h"

 /*************************
  *        EVENTS         *
  *************************/
#include "ChaosCore/Events/AEvent.h"
#include "ChaosCore/Events/ApplicationEvents.h"
#include "ChaosCore/Events/EventDispatcher.h"
#include "ChaosCore/Events/KeyEvents.h"
#include "ChaosCore/Events/SceneEvents.h"
#include "ChaosCore/Events/MouseEvents.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Events/EventHandler.h"

  /************************
   *        INPUTS       *
   ***********************/
#include "ChaosCore/Inputs/Input.h"

  /************************
   *      RESOURCES       *
   ************************/
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Resources/Managers/AnimationManager.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"
#include "ChaosCore/Resources/Managers/MeshManager.h"
#include "ChaosCore/Resources/Managers/TextureManager.h"
#include "ChaosCore/Resources/Managers/SkyboxManager.h"
#include "ChaosCore/Resources/Managers/ShaderManager.h"
#include "ChaosCore/Resources/Managers/SceneManager.h"
#include "ChaosCore/Resources/Managers/MaterialManager.h"

#include "ChaosCore/Resources/Buffers/IVertexBuffer.h"
#include "ChaosCore/Resources/Buffers/IIndexBuffer.h"

#include "ChaosCore/Resources/Mesh.h"
#include "ChaosCore/Resources/Submesh.h"
#include "ChaosCore/Resources/ATexture.h"
#include "ChaosCore/Resources/ASkybox.h"
#include "ChaosCore/Resources/AShader.h"
#include "ChaosCore/Resources/ShaderTypes.h"
#include "ChaosCore/Scene/Scene.h"

#include "ChaosCore/Resources/Animation.h"
#include "ChaosCore/Resources/SkeletalMesh.h"
#include "ChaosCore/Resources/AnimationMesh.h"
#include "ChaosCore/Resources/Skeleton.h"
/************************
*          ECS          *
************************/
#include "ChaosCore/ECS/Types.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/ECS/Entity/AEntity.h"
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/ECS/System/System.h"

/************************
*       ENTITIES       *
************************/
#include "ChaosCore/GameObjects/GameObject.h"
#include "ChaosCore/GameObjects/EmptyGameObject.h"
#include "ChaosCore/GameObjects/MeshGameObject.h"
#include "ChaosCore/GameObjects/LightGameObject.h"
#include "ChaosCore/GameObjects/CameraGameObject.h"
#include "ChaosCore/GameObjects/SkeletalAnimationGameObject.h"
/************************
*       COMPONENTS      *
************************/
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Components/MeshComponent.h"
#include "ChaosCore/Components/ChildComponent.h"
#include "ChaosCore/Components/MaterialComponent.h"
#include "ChaosCore/Components/CameraComponent.h"
#include "ChaosCore/Components/LightComponent.h"

#include "ChaosCore/Components/SphereColliderComponent.h"
#include "ChaosCore/Components/BoxColliderComponent.h"
#include "ChaosCore/Components/RigidBodyComponent.h"

#include "ChaosCore/Components/AnimationComponent.h"
/************************
*         SYSTEMS       *
************************/
#include "ChaosCore/Systems/AnimationSystem.h"
#include "ChaosCore/Systems/RenderSystem.h"
#include "ChaosCore/Systems/CameraSystem.h"
#include "ChaosCore/Systems/LightSystem.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Systems/PhysicsSystem.h"


/************************
*         PHYSICS       *
************************/
#include "ChaosCore/Physics/Query.h"

/************************
*       SCRIPTING       *
************************/
#include "ChaosCore/Scripting/ScriptsHandler.h"

/************************
*         AUDIO         *
************************/
#include "ChaosCore/Components/AudioSourceComponent.h"
#include "ChaosCore/Resources/AudioClip.h"
#include "ChaosCore/Resources/Managers/AudioResourcesManager.h"
#include "ChaosCore/Systems/AudioSystem.h"