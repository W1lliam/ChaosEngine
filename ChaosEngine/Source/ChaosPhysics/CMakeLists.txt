cmake_minimum_required(VERSION 3.12.2)

set(INCLUDEDIR "include/ChaosPhysics")
set(SOURCEDIR "src")

set(HEADER_FILES
		${INCLUDEDIR}/Actor/PhysicsActor.h 
		${INCLUDEDIR}/Base/Cooking.h
		${INCLUDEDIR}/Base/PhysicsElementCreator.h 
		${INCLUDEDIR}/Base/physicsFoundation.h
		${INCLUDEDIR}/Base/VisualDebugger.h 
		${INCLUDEDIR}/Friction/Friction.h 
		${INCLUDEDIR}/Query/GeometryQuery.h
		${INCLUDEDIR}/Collision/CollisionMask.h
		${INCLUDEDIR}/Maths/Conversion.h
		${INCLUDEDIR}/Maths/Conversion.inl
		${INCLUDEDIR}/Scene/Scene.h 
		${INCLUDEDIR}/Shape/Shape.h
		${INCLUDEDIR}/Shape/MeshShape.h)

set(SOURCE_FILES
		${SOURCEDIR}/Actor/PhysicsActor.cpp
		${SOURCEDIR}/Base/Cooking.cpp
		${SOURCEDIR}/Base/PhysicsElementCreator.cpp 
		${SOURCEDIR}/Base/physicsFoundation.cpp
		${SOURCEDIR}/Base/VisualDebugger.cpp
		${SOURCEDIR}/Friction/Friction.cpp
		${SOURCEDIR}/Query/GeometryQuery.cpp 
		${SOURCEDIR}/Scene/Scene.cpp
		${SOURCEDIR}/Shape/Shape.cpp
		${SOURCEDIR}/Shape/MeshShape.cpp
		)

GroupSources("${SOURCE_FILES}") 
GroupSources("${HEADER_FILES}") 

IF(WIN32)
	add_library(PHYSX STATIC IMPORTED GLOBAL) 
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_DEBUG ${DEPENDENCIES_DIR}/PhysX/physx/lib/window/debug/*.lib)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_DEVELOPMENT ${DEPENDENCIES_DIR}/PhysX/physx/lib/window/debug/*.lib)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_SHIPPING ${DEPENDENCIES_DIR}/PhysX/physx/lib/window/shipping/*.lib)
	include_directories(${DEPENDENCIES_DIR}/PhysX)
	include_directories(${DEPENDENCIES_DIR}/PhysX/physx/include)
	include_directories(${DEPENDENCIES_DIR}/PhysX/pxshared/include)	
ELSE()
	add_library(PHYSX STATIC IMPORTED)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_DEBUG ${DEPENDENCIES_DIR}/PhysX/physx/lib/linux/debug/*.so)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_DEVELOPMENT ${DEPENDENCIES_DIR}/PhysX/physx/lib/linux/debug/*.so)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_SHIPPING ${DEPENDENCIES_DIR}/PhysX/physx/lib/linux/release/*.so)
	include_directories(${DEPENDENCIES_DIR}/PhysX)
	include_directories(${DEPENDENCIES_DIR}/PhysX/physx/include)
	include_directories(${DEPENDENCIES_DIR}/PhysX/pxshared/include)
ENDIF()

add_library(ChaosPhysics STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(ChaosPhysics PUBLIC "include")

target_link_libraries(ChaosPhysics PUBLIC ChaosCore
								   PUBLIC ChaosDebug
								   PUBLIC ChaosMaths
								   PUBLIC PHYSX)