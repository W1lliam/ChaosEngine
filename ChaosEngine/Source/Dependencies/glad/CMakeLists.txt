cmake_minimum_required(VERSION 3.12.2)

if(CHAOS_GRAPHIC_API STREQUAL "OPENGL")
set(INCLUDEDIR "include")
set(SOURCEDIR "src")

set(HEADER_FILES
        ${INCLUDEDIR}/glad/glad.h
        ${INCLUDEDIR}/KHR/khrplatform.h
        )
set(SOURCE_FILES 
        ${SOURCEDIR}/glad.c
        )

GroupSources("${SOURCE_FILES}") 
GroupSources("${HEADER_FILES}") 
add_library(glad STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(glad PUBLIC "include") 
endif()