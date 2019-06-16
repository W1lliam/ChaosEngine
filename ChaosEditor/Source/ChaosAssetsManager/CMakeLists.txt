cmake_minimum_required(VERSION 3.12.2)

set(INCLUDEDIR "include/ChaosAssetsManager")
set(SOURCEDIR "src")

set(HEADER_FILES 
        ${INCLUDEDIR}/export.h
	include/ChaosAssetsManager.h
        ${INCLUDEDIR}/AssetsLoader/AssetsLoader.h
        )
set(SOURCE_FILES
        ${SOURCEDIR}/AssetsLoader/AssetsLoader.cpp)

GroupSources("${SOURCE_FILES}") 
GroupSources("${HEADER_FILES}") 

add_library(ChaosAssetsManager SHARED ${SOURCE_FILES}  ${HEADER_FILES})
target_link_libraries(ChaosAssetsManager PUBLIC assimp-vc140-mt)
target_include_directories(ChaosAssetsManager 
                                PUBLIC "include"
                                PUBLIC "${DEPENDENCIES_DIR}/assimp/include"
                                PUBLIC "${DEPENDENCIES_DIR}/stbimage/include"
                                PUBLIC "${COMMON_DIR}")
target_compile_definitions(ChaosAssetsManager PRIVATE CHAOS_ASSETS_MANAGER_EXPORT)

add_custom_command(TARGET ChaosAssetsManager POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DEPENDENCIES_DIR}/assimp/bin/assimp-vc140-mt.dll" $<TARGET_FILE_DIR:ChaosAssetsManager>)

target_precompiled_header(ChaosAssetsManager ${INCLUDEDIR}/chaos_assets_manager_pch.h)
