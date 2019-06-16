cmake_minimum_required(VERSION 3.12.2)

set(INCLUDEDIR "include/ChaosEngine")
set(SOURCEDIR "src")

set(HEADER_FILES 
        include/ChaosEngine.h 
		${INCLUDEDIR}/export.h
		${INCLUDEDIR}/Application.h
		${INCLUDEDIR}/Base.h
		${INCLUDEDIR}/EntryPoint.h
		${INCLUDEDIR}/Loader.h
        )
set(SOURCE_FILES 
		${SOURCEDIR}/Application.cpp
		${SOURCEDIR}/Base.cpp
		${SOURCEDIR}/Loader.cpp
        )

GroupSources("${SOURCE_FILES}") 
GroupSources("${HEADER_FILES}") 
add_library(ChaosEngine SHARED ${SOURCE_FILES}  ${HEADER_FILES})

target_link_libraries(ChaosEngine PUBLIC ChaosCore
                                  PUBLIC ChaosGraphics)

target_include_directories(ChaosEngine	PUBLIC "include"
					PUBLIC "${COMMON_DIR}")

target_compile_definitions(ChaosEngine PRIVATE CHAOS_ENGINE_EXPORT)

add_custom_command(TARGET ChaosEngine POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${DEPENDENCIES_DIR}/PhysX/physx/lib/window/$<IF:$<CONFIG:Shipping>,shipping,debug>/dll"
		 $<TARGET_FILE_DIR:ChaosEngine>)
		 
#Audio
add_custom_command(TARGET ChaosEngine POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${DEPENDENCIES_DIR}/FMOD/api/core/lib/fmod$<$<NOT:$<CONFIG:Shipping>>:L>.dll"
		 $<TARGET_FILE_DIR:ChaosEngine>
		 
		 COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${DEPENDENCIES_DIR}/FMOD/api/studio/lib/fmodstudio$<$<NOT:$<CONFIG:Shipping>>:L>.dll"
		 $<TARGET_FILE_DIR:ChaosEngine>
		 )