cmake_minimum_required(VERSION 3.12.2)

set(INCLUDEDIR "include/ChaosAssetLoader")
set(SOURCEDIR "src")

set(HEADER_FILES 
        ${INCLUDEDIR}/AssetLoader.h
		include/ChaosAssetLoader.h
        )
set(SOURCE_FILES
        ${SOURCEDIR}/AssetLoader.cpp)

add_library(ChaosAssetLoader STATIC ${SOURCE_FILES}  ${HEADER_FILES})
target_include_directories(ChaosAssetLoader 
                                PUBLIC "include"
                                PUBLIC "${COMMON_DIR}")

target_link_libraries(ChaosAssetLoader PUBLIC ChaosDebug)

if(UNIX)
        target_link_libraries(ChaosAssetLoader PUBLIC "stdc++fs")
endif()