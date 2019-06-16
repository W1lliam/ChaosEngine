cmake_minimum_required(VERSION 3.12.2)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/CMakePCH)
include(${CMAKE_SOURCE_DIR}/cmake/PreventInSourceBuilds.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/GroupByFolder.cmake)

set(CMAKE_CONFIGURATION_TYPES "Debug;Development;Shipping")
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;Development")

project(ChaosEngine C CXX CXXPCH)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Define c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CHAOS_SIMD "Enable SIMD instructions" ON)

set(COMMON_DIR "${CMAKE_SOURCE_DIR}/../../Commons")

#Available options : OPENGL
set(CHAOS_GRAPHIC_API "OPENGL")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../Build/lib/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../Build/lib/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../Build/bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin-int)

if(MSVC_VERSION GREATER_EQUAL "1900")
    add_compile_definitions("CHAOS_$<UPPER_CASE:$<CONFIG>>;")
else()
    set(COMPILE_DEFINITIONS_DEBUG       "${COMPILE_DEFINITIONS_DEBUG}CHAOS_DEBUG;")
    set(COMPILE_DEFINITIONS_DEVELOPMENT "${COMPILE_DEFINITIONS_DEVELOPMENT}CHAOS_DEVELOPMENT;")
    set(COMPILE_DEFINITIONS_SHIPPING    "${COMPILE_DEFINITIONS_SHIPPING}CHAOS_SHIPPING;")
endif()


if(WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG       "${CMAKE_CXX_FLAGS_DEBUG} /Od /Z7 /MDd")
    set(CMAKE_CXX_FLAGS_DEVELOPMENT "${CMAKE_CXX_FLAGS_DEVELOPMENT} /O2 /Z7 /MDd")
    set(CMAKE_CXX_FLAGS_SHIPPING    "${CMAKE_CXX_FLAGS_RELEASE} /O2 /MD")
    add_compile_definitions(CHAOS_PLATFORM_WINDOWS _ENABLE_EXTENDED_ALIGNED_STORAGE NOMINMAX)

    # Define c++ standard for Visual Studio 15
    if (MSVC_VERSION GREATER_EQUAL "1900")
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std:c++17" _cpp_latest_flag_supported)
        if (_cpp_latest_flag_supported)
            add_compile_options("/std:c++17")
        endif()
    endif()
endif()

if(UNIX)
    set(CMAKE_CXX_FLAGS_DEBUG       "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Winvalid-pch -g")
    set(CMAKE_CXX_FLAGS_DEVELOPMENT "${CMAKE_CXX_FLAGS_DEVELOPMENT} -O2 -Wall -Winvalid-pch -g")
    set(CMAKE_CXX_FLAGS_SHIPPING    "${CMAKE_CXX_FLAGS_RELEASE} -O3")

    add_compile_definitions(CHAOS_PLATFORM_UNIX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

include(Dependencies/Dependencies.cmake)
add_subdirectory(ChaosDebug)
add_subdirectory(ChaosMaths)
add_subdirectory(ChaosCore)
add_subdirectory(ChaosPhysics)
add_subdirectory(ChaosGraphics)
add_subdirectory(ChaosEngine)
add_subdirectory(Game)


if(MSVC)
    set_property(TARGET ChaosDebug ChaosMaths ChaosCore ChaosGraphics ChaosPhysics APPEND PROPERTY LINK_FLAGS_DEBUG "/DEBUG:FASTLINK /INCREMENTAL:NO")
    set_property(TARGET ChaosDebug ChaosMaths ChaosCore ChaosGraphics ChaosPhysics APPEND PROPERTY LINK_FLAGS_DEVELOPMENT "/DEBUG:FASTLINK /INCREMENTAL:NO")
    set_property(TARGET ChaosDebug ChaosMaths ChaosCore ChaosGraphics ChaosPhysics APPEND PROPERTY LINK_FLAGS_SHIPPING "/DEBUG:NONE /OPT:REF,ICF,LBR") 
    set_property(TARGET ChaosEngine Game APPEND PROPERTY LINK_FLAGS_DEBUG "/DEBUG:FASTLINK /INCREMENTAL:NO")
    set_property(TARGET ChaosEngine Game APPEND PROPERTY LINK_FLAGS_DEVELOPMENT "/DEBUG:FASTLINK /INCREMENTAL:NO")
    set_property(TARGET ChaosEngine Game APPEND PROPERTY LINK_FLAGS_SHIPPING "/DEBUG:NONE /OPT:REF,ICF,LBR")
endif()
set_property(TARGET Game PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Game>)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)