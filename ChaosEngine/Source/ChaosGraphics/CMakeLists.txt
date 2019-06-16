cmake_minimum_required(VERSION 3.12.2)

set(INCLUDEDIR "include/ChaosGraphics")
set(SOURCEDIR "src")

set(HEADER_FILES
        include/ChaosGraphics.h
       # ${INCLUDEDIR}/Cameras/OrbitalCamera.h
        )
set(SOURCE_FILES 
        #${SOURCEDIR}/Cameras/OrbitalCamera.cpp
        )

if(WIN32 OR UNIX)
    list(APPEND HEADER_FILES ${INCLUDEDIR}/Context/GLFWWindow.h)
    list(APPEND HEADER_FILES ${INCLUDEDIR}/Context/GLFWInput.h)

    list(APPEND SOURCE_FILES ${SOURCEDIR}/Context/GLFWWindow.cpp)
    list(APPEND SOURCE_FILES ${SOURCEDIR}/Context/GLFWInput.cpp)
endif()

if(CHAOS_GRAPHIC_API STREQUAL "OPENGL")
    list(APPEND HEADER_FILES 
        ${INCLUDEDIR}/Renderer/OpenGLRenderer.h 
        ${INCLUDEDIR}/Buffers/OpenGLVertexBuffer.h 
        ${INCLUDEDIR}/Buffers/OpenGLIndexBuffer.h
        ${INCLUDEDIR}/Buffers/OpenGLVertexArray.h 
        ${INCLUDEDIR}/Buffers/OpenGLUniformBuffer.h 
        ${INCLUDEDIR}/Buffers/OpenGLFrameBuffer.h 
        ${INCLUDEDIR}/Buffers/OpenGLBufferLayout.h 
        ${INCLUDEDIR}/Resources/GLSLShader.h
        ${INCLUDEDIR}/Resources/OpenGLTexture.h
        ${INCLUDEDIR}/Resources/OpenGLSkybox.h
        )
    list(APPEND SOURCE_FILES 
        ${SOURCEDIR}/Renderer/OpenGLRenderer.cpp 
        ${SOURCEDIR}/Buffers/OpenGLVertexBuffer.cpp 
        ${SOURCEDIR}/Buffers/OpenGLIndexBuffer.cpp 
        ${SOURCEDIR}/Buffers/OpenGLVertexArray.cpp 
        ${SOURCEDIR}/Buffers/OpenGLUniformBuffer.cpp 
        ${SOURCEDIR}/Buffers/OpenGLFrameBuffer.cpp 
        ${SOURCEDIR}/Buffers/OpenGLBufferLayout.cpp 
        ${SOURCEDIR}/Resources/GLSLShader.cpp
        ${SOURCEDIR}/Resources/OpenGLTexture.cpp
        ${SOURCEDIR}/Resources/OpenGLSkybox.cpp
        )
endif()

GroupSources("${SOURCE_FILES}") 
GroupSources("${HEADER_FILES}") 
add_library(ChaosGraphics STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(ChaosGraphics PUBLIC ChaosCore)

if(CHAOS_GRAPHIC_API STREQUAL "OPENGL")
    target_link_libraries(ChaosGraphics PUBLIC glad)
    if(WIN32)
        target_link_libraries(ChaosGraphics 
                                PUBLIC "glfw3" 
                                PUBLIC "opengl32")
    elseif(UNIX)
        target_link_libraries(ChaosGraphics 
                                PUBLIC "glfw" 
                                PUBLIC "GLU" 
                                PUBLIC "GL")
    endif()
endif()

target_include_directories(ChaosGraphics 
                            PUBLIC "include"
                            PUBLIC "${DEPENDENCIES_DIR}/glad/include"
                            PUBLIC "${DEPENDENCIES_DIR}/glfw/include")

target_precompiled_header(ChaosGraphics ${INCLUDEDIR}/chaos_graphics_pch.h)