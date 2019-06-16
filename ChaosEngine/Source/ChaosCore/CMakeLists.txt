cmake_minimum_required(VERSION 3.12.2)

set(INCLUDEDIR "include/ChaosCore")
set(SOURCEDIR "src")

set(HEADER_FILES
		#Core
        include/ChaosCore.h
		${INCLUDEDIR}/TransformBase.h
        ${INCLUDEDIR}/Transform.h
		${INCLUDEDIR}/DualQuatTransform.h

        #Context
        ${INCLUDEDIR}/Context/IWindow.h
        
        #Rendering
        ${INCLUDEDIR}/Rendering/IRenderer.h

		#Multithreading
        ${INCLUDEDIR}/Multithreading/Mutexes/Spinlock.h
        ${INCLUDEDIR}/Multithreading/Mutexes/SharedSpinlock.h

        ${INCLUDEDIR}/Multithreading/Locks/LockGuard.h
        ${INCLUDEDIR}/Multithreading/Locks/SharedLock.h
        ${INCLUDEDIR}/Multithreading/Locks/UniqueLock.h
        
        ${INCLUDEDIR}/Multithreading/Containers/LockFree/LFQueue.h
        ${INCLUDEDIR}/Multithreading/Containers/LockFree/LFStack.h

        ${INCLUDEDIR}/Multithreading/Containers/ThreadSafeArray.h
        ${INCLUDEDIR}/Multithreading/Containers/ThreadSafeList.h
        ${INCLUDEDIR}/Multithreading/Containers/ThreadSafeMap.h

        ${INCLUDEDIR}/Multithreading/Functions/FunctionWrapper.h
        ${INCLUDEDIR}/Multithreading/Threads/ThreadsJoiner.h
        ${INCLUDEDIR}/Multithreading/Threads/ThreadPool.h

		#Memory
        ${INCLUDEDIR}/Memory/Allocators/AAllocator.h
        ${INCLUDEDIR}/Memory/Allocators/LinearAllocator.h
        ${INCLUDEDIR}/Memory/Allocators/PoolAllocator.h
        ${INCLUDEDIR}/Memory/Allocators/StackAllocator.h
        ${INCLUDEDIR}/Memory/Allocators/DoubleBufferedAllocator.h
        ${INCLUDEDIR}/Memory/Allocators/TLSFAllocator.h

		#Containers
        ${INCLUDEDIR}/Containers/RHHashTable.h

		#Utils
        ${INCLUDEDIR}/Utils/StringID.h
        ${INCLUDEDIR}/Utils/CRC.h
        ${INCLUDEDIR}/Utils/GUID.h
        ${INCLUDEDIR}/Utils/Filesystem.h
        ${INCLUDEDIR}/Utils/algorithm.h
        ${INCLUDEDIR}/Utils/Singleton.h
        ${INCLUDEDIR}/Utils/Timer.h

		#Events
		${INCLUDEDIR}/Events/EventListener.h
		${INCLUDEDIR}/Events/AEvent.h
		${INCLUDEDIR}/Events/ECSEvents.h
		${INCLUDEDIR}/Events/KeyEvents.h
		${INCLUDEDIR}/Events/SceneEvents.h
		${INCLUDEDIR}/Events/MouseEvents.h
		${INCLUDEDIR}/Events/PhysicsSceneEvents.h
		${INCLUDEDIR}/Events/PhysicsColliderEvents.h
		${INCLUDEDIR}/Events/ApplicationEvents.h
		${INCLUDEDIR}/Events/EventHandler.h
		${INCLUDEDIR}/Events/IEventDispatcher.h
		${INCLUDEDIR}/Events/EventDispatcher.h
		${INCLUDEDIR}/Events/IEventDelegate.h
		${INCLUDEDIR}/Events/EventDelegate.h

		#Inputs
		${INCLUDEDIR}/Inputs/Input.h

        #Resources
        ${INCLUDEDIR}/Resources/AssetLoader.h
        ${INCLUDEDIR}/Resources/Managers/ResourceGroupManager.h
        ${INCLUDEDIR}/Resources/Managers/AResourceManager.h
        ${INCLUDEDIR}/Resources/Managers/MeshManager.h
        ${INCLUDEDIR}/Resources/Managers/TextureManager.h
        ${INCLUDEDIR}/Resources/Managers/SkyboxManager.h
        ${INCLUDEDIR}/Resources/Managers/ShaderManager.h
        ${INCLUDEDIR}/Resources/Managers/SceneManager.h
        ${INCLUDEDIR}/Resources/Managers/MaterialManager.h
		${INCLUDEDIR}/Resources/Managers/AnimationManager.h

        ${INCLUDEDIR}/Resources/AResource.h
		${INCLUDEDIR}/Resources/Animation.h
        ${INCLUDEDIR}/Resources/Vertex.h
        ${INCLUDEDIR}/Resources/Mesh.h
        ${INCLUDEDIR}/Resources/ShaderTypes.h
        ${INCLUDEDIR}/Resources/Material.h
        ${INCLUDEDIR}/Resources/SubMesh.h
		${INCLUDEDIR}/Resources/Skeleton.h
		${INCLUDEDIR}/Resources/AnimationMesh.h
        ${INCLUDEDIR}/Resources/ATexture.h
        ${INCLUDEDIR}/Resources/ASkybox.h
        ${INCLUDEDIR}/Resources/AShader.h
		${INCLUDEDIR}/Resources/SkeletalMesh.h

        ${INCLUDEDIR}/Resources/Buffers/IVertexBuffer.h
        ${INCLUDEDIR}/Resources/Buffers/IIndexBuffer.h
        ${INCLUDEDIR}/Resources/Buffers/IVertexArray.h
        ${INCLUDEDIR}/Resources/Buffers/IUniformBuffer.h
        ${INCLUDEDIR}/Resources/Buffers/IFrameBuffer.h

		#Scene
		${INCLUDEDIR}/Scene/Scene.h

        #ECS
        ${INCLUDEDIR}/ECS/Types.h
        ${INCLUDEDIR}/ECS/Entity/AEntity.h
        ${INCLUDEDIR}/ECS/Component/Component.h
        ${INCLUDEDIR}/ECS/Component/ScriptedComponent.h
        ${INCLUDEDIR}/ECS/System/System.h
        ${INCLUDEDIR}/ECS/System/ScriptedSystem.h
        ${INCLUDEDIR}/ECS/Managers/EntityManager.h
        ${INCLUDEDIR}/ECS/Managers/ComponentManager.h
        ${INCLUDEDIR}/ECS/Managers/SystemManager.h

        #GameObjects
        ${INCLUDEDIR}/GameObjects/GameObject.h
        ${INCLUDEDIR}/GameObjects/EmptyGameObject.h
        ${INCLUDEDIR}/GameObjects/MeshGameObject.h
        ${INCLUDEDIR}/GameObjects/LightGameObject.h
        ${INCLUDEDIR}/GameObjects/CameraGameObject.h
		${INCLUDEDIR}/GameObjects/RigidBodyGameObject.h
		${INCLUDEDIR}/GameObjects/SkeletalAnimationGameObject.h
        #Components
        ${INCLUDEDIR}/Components/TransformComponent.h
		${INCLUDEDIR}/Components/BoxColliderComponent.h
		${INCLUDEDIR}/Components/CapsuleColliderComponent.h
		${INCLUDEDIR}/Components/SphereColliderComponent.h
        ${INCLUDEDIR}/Components/MeshComponent.h
        ${INCLUDEDIR}/Components/MaterialComponent.h
        ${INCLUDEDIR}/Components/CameraComponent.h
		${INCLUDEDIR}/Components/Parameters/CameraComponentParams.h
        ${INCLUDEDIR}/Components/LightComponent.h
		${INCLUDEDIR}/Components/RigidBodyComponent.h
        ${INCLUDEDIR}/Components/ChildComponent.h
   
        #Systems
        ${INCLUDEDIR}/Systems/RenderSystem.h
        ${INCLUDEDIR}/Systems/CameraSystem.h
        ${INCLUDEDIR}/Systems/LightSystem.h
		${INCLUDEDIR}/Systems/PhysicsSystem.h
		${INCLUDEDIR}/Systems/CharacterControllerSystem.h
        ${INCLUDEDIR}/Systems/SceneSystem.h
		${INCLUDEDIR}/Systems/AnimationSystem.h 
        #Serialization
        ${INCLUDEDIR}/Serialization/Serializer.h
        ${INCLUDEDIR}/Serialization/BitReader.h
        ${INCLUDEDIR}/Serialization/BitWriter.h
        ${INCLUDEDIR}/Serialization/ReadStream.h
        ${INCLUDEDIR}/Serialization/WriteStream.h

		#Physics
		${INCLUDEDIR}/Physics/PhysicsFriction.h
		${INCLUDEDIR}/Physics/Collider.h
		${INCLUDEDIR}/Physics/MeshCollider.h
		${INCLUDEDIR}/Physics/Query.h
		${INCLUDEDIR}/Physics/CollisionFiltering.h

        #Scripting
        ${INCLUDEDIR}/Scripting/ScriptsHandler.h
		
		#Audio
		${INCLUDEDIR}/Components/AudioSourceComponent.h
		${INCLUDEDIR}/Systems/AudioSystem.h
		${INCLUDEDIR}/Resources/AudioClip.h
		${INCLUDEDIR}/Resources/Managers/AudioResourcesManager.h

		#Animation
		${INCLUDEDIR}/Animation/Animator.h
		${INCLUDEDIR}/Animation/AnimationBlender.h
		${INCLUDEDIR}/Animation/PoseCalculator.h 
		${INCLUDEDIR}/Animation/Pose.h 
		${INCLUDEDIR}/Components/AnimationComponent.h
		)

set(SOURCE_FILES
        #Core
        ${SOURCEDIR}/Transform.cpp
		${SOURCEDIR}/TransformBase.cpp

		#Multithreading
        ${SOURCEDIR}/Multithreading/Mutexes/Spinlock.cpp
        ${SOURCEDIR}/Multithreading/Mutexes/SharedSpinlock.cpp

        ${SOURCEDIR}/Multithreading/Threads/ThreadsJoiner.cpp
        ${SOURCEDIR}/Multithreading/Threads/ThreadPool.cpp

		#Memory
        ${SOURCEDIR}/Memory/Allocators/AAllocator.cpp
        ${SOURCEDIR}/Memory/Allocators/LinearAllocator.cpp
        ${SOURCEDIR}/Memory/Allocators/PoolAllocator.cpp
        ${SOURCEDIR}/Memory/Allocators/StackAllocator.cpp
        ${SOURCEDIR}/Memory/Allocators/DoubleBufferedAllocator.cpp
        ${SOURCEDIR}/Memory/Allocators/TLSFAllocator.cpp

		#Events
        ${SOURCEDIR}/Events/EventHandler.cpp
        ${SOURCEDIR}/Events/EventListener.cpp

		#Utils
		${SOURCEDIR}/Utils/StringID.cpp
		${SOURCEDIR}/Utils/Timer.cpp

        #Resources
        ${SOURCEDIR}/Resources/AssetLoader.cpp
        ${SOURCEDIR}/Resources/Managers/ResourceGroupManager.cpp
        ${SOURCEDIR}/Resources/Managers/AResourceManager.cpp
        ${SOURCEDIR}/Resources/Managers/MeshManager.cpp
        ${SOURCEDIR}/Resources/Managers/TextureManager.cpp
        ${SOURCEDIR}/Resources/Managers/SkyboxManager.cpp
        ${SOURCEDIR}/Resources/Managers/ShaderManager.cpp
        ${SOURCEDIR}/Resources/Managers/SceneManager.cpp
        ${SOURCEDIR}/Resources/Managers/MaterialManager.cpp
		${SOURCEDIR}/Resources/Managers/AnimationManager.cpp
        
		${SOURCEDIR}/Resources/AResource.cpp
        ${SOURCEDIR}/Resources/Mesh.cpp
        ${SOURCEDIR}/Resources/Material.cpp
        ${SOURCEDIR}/Resources/SubMesh.cpp
        ${SOURCEDIR}/Resources/ATexture.cpp
        ${SOURCEDIR}/Resources/ASkybox.cpp
        ${SOURCEDIR}/Resources/AShader.cpp
		${SOURCEDIR}/Resources/AResource.cpp
		${SOURCEDIR}/Resources/Skeleton.cpp
		${SOURCEDIR}/Resources/AnimationMesh.cpp
		${SOURCEDIR}/Resources/Animation.cpp
        ${SOURCEDIR}/Resources/SkeletalMesh.cpp
		#Scene
		${SOURCEDIR}/Scene/Scene.cpp
        
        #ECS
        ${SOURCEDIR}/ECS/System/System.cpp
        ${SOURCEDIR}/ECS/System/ScriptedSystem.cpp
        ${SOURCEDIR}/ECS/Component/Component.cpp
        ${SOURCEDIR}/ECS/Component/ScriptedComponent.cpp
        ${SOURCEDIR}/ECS/Entity/AEntity.cpp
        ${SOURCEDIR}/ECS/Managers/EntityManager.cpp
        ${SOURCEDIR}/ECS/Managers/ComponentManager.cpp
        ${SOURCEDIR}/ECS/Managers/SystemManager.cpp

        #GameObjects
        ${SOURCEDIR}/GameObjects/EmptyGameObject.cpp
        ${SOURCEDIR}/GameObjects/MeshGameObject.cpp
		${SOURCEDIR}/GameObjects/SkeletalAnimationGameObject.cpp
        ${SOURCEDIR}/GameObjects/LightGameObject.cpp
        ${SOURCEDIR}/GameObjects/CameraGameObject.cpp
		${SOURCEDIR}/GameObjects/RigidBodyGameObject.cpp
        
        #Components
        ${SOURCEDIR}/Components/TransformComponent.cpp
        ${SOURCEDIR}/Components/MeshComponent.cpp
		${SOURCEDIR}/Components/BoxColliderComponent.cpp
		${SOURCEDIR}/Components/CapsuleColliderComponent.cpp
		${SOURCEDIR}/Components/SphereColliderComponent.cpp
        ${SOURCEDIR}/Components/MaterialComponent.cpp
        ${SOURCEDIR}/Components/CameraComponent.cpp
        ${SOURCEDIR}/Components/LightComponent.cpp
		${SOURCEDIR}/Components/RigidBodyComponent.cpp
        ${SOURCEDIR}/Components/ChildComponent.cpp

        #Systems
        ${SOURCEDIR}/Systems/RenderSystem.cpp
        ${SOURCEDIR}/Systems/CameraSystem.cpp
        ${SOURCEDIR}/Systems/LightSystem.cpp
		${SOURCEDIR}/Systems/PhysicsSystem.cpp
        ${SOURCEDIR}/Systems/SceneSystem.cpp
		${SOURCEDIR}/Systems/AnimationSystem.cpp 
        #Serialization
        ${SOURCEDIR}/Serialization/BitReader.cpp
        ${SOURCEDIR}/Serialization/BitWriter.cpp
        ${SOURCEDIR}/Serialization/ReadStream.cpp
        ${SOURCEDIR}/Serialization/WriteStream.cpp

		#Physics
		${SOURCEDIR}/Physics/PhysicsFriction.cpp
		${SOURCEDIR}/Physics/Collider.cpp
		${SOURCEDIR}/Physics/Query.cpp
        ${SOURCEDIR}/Physics/CollisionFiltering.cpp

        #Scripting
        ${SOURCEDIR}/Scripting/ScriptsHandler.cpp
		
		#Audio
		${SOURCEDIR}/Components/AudioSourceComponent.cpp
		${SOURCEDIR}/Systems/AudioSystem.cpp
		${SOURCEDIR}/Resources/AudioClip.cpp
		${SOURCEDIR}/Resources/Managers/AudioResourcesManager.cpp

		#Animation
		${SOURCEDIR}/Animation/Pose.cpp
		${SOURCEDIR}/Animation/AnimationBlender.cpp
		${SOURCEDIR}/Animation/PoseCalculator.cpp 
		${SOURCEDIR}/Animation/Animator.cpp
		${SOURCEDIR}/Components/AnimationComponent.cpp
        )

 GroupSources("${SOURCE_FILES}") 
 GroupSources("${HEADER_FILES}") 

add_library(ChaosCore STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(ChaosCore 
                            PUBLIC "include"
                            PUBLIC ${COMMON_DIR})

							#ChaosAudio removed
target_link_libraries(ChaosCore PUBLIC ChaosDebug
                                PUBLIC ChaosMaths
								PUBLIC ChaosPhysics
                                PUBLIC "${DEPENDENCIES_DIR}/squirrel/lib/$<CONFIG>/sqstdlib_static.lib"
                                PUBLIC "${DEPENDENCIES_DIR}/squirrel/lib/$<CONFIG>/squirrel_static.lib"
                                PUBLIC "${DEPENDENCIES_DIR}/simpleSquirrel/lib/$<CONFIG>/simplesquirrel_static.lib"
								PUBLIC "fmod$<IF:$<CONFIG:Shipping>,,L>_vc.lib"
								PUBLIC "fmodstudio$<IF:$<CONFIG:Shipping>,,L>_vc.lib"
                                )

target_precompiled_header(ChaosCore ${INCLUDEDIR}/chaos_core_pch.h)