cmake_minimum_required(VERSION 3.12.2)

set(DEPENDENCIES_DIR "${CMAKE_CURRENT_LIST_DIR}")

if(CHAOS_GRAPHIC_API STREQUAL "OPENGL")
    add_compile_definitions("CHAOS_OPENGL")
    add_subdirectory("${DEPENDENCIES_DIR}/glad")
    link_directories("${DEPENDENCIES_DIR}/glfw/lib" 
                     "${DEPENDENCIES_DIR}/glew/lib")
endif()


link_directories("${DEPENDENCIES_DIR}/squirrel/lib")

include_directories(${DEPENDENCIES_DIR}/PhysX)
include_directories(${DEPENDENCIES_DIR}/PhysX/physx/include)
include_directories(${DEPENDENCIES_DIR}/PhysX/pxshared/include)

add_compile_definitions(_SQ64)
include_directories(${DEPENDENCIES_DIR}/squirrel/include)
include_directories(${DEPENDENCIES_DIR}/sqrat/include)
include_directories(${DEPENDENCIES_DIR}/simpleSquirrel/include)

#Audio
link_directories("${DEPENDENCIES_DIR}/FMOD/api/core/lib")
link_directories("${DEPENDENCIES_DIR}/FMOD/api/studio/lib")

include_directories(${DEPENDENCIES_DIR}/FMOD/api/core/inc)
include_directories(${DEPENDENCIES_DIR}/FMOD/api/studio/inc)