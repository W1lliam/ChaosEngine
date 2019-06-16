cmake_minimum_required(VERSION 3.12.2)

set(INCLUDEDIR "include/ChaosDebug")
set(SOURCEDIR "src")

set(HEADER_FILES
        include/ChaosDebug.h
        ${INCLUDEDIR}/Log.h
        ${INCLUDEDIR}/Assert.h
        )

set(SOURCE_FILES 
        ${SOURCEDIR}/Log.cpp
        )


GroupSources("${SOURCE_FILES}") 
GroupSources("${HEADER_FILES}") 

add_library(ChaosDebug STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(ChaosDebug 
                            PUBLIC "include"
                            PUBLIC "${DEPENDENCIES_DIR}/spdlog/include")

target_precompiled_header(ChaosDebug ${INCLUDEDIR}/chaos_debug_pch.h)