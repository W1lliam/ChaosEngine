cmake_minimum_required(VERSION 3.12.2)

set(HEADER_FILES 
        Game.h
        )
set(SOURCE_FILES
        Game.cpp
    )

GroupSources("${SOURCE_FILES}") 
GroupSources("${HEADER_FILES}") 
add_executable(Game ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(Game  PUBLIC ChaosEngine)

if(WIN32)
    add_custom_command(TARGET Game POST_BUILD 
                       COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/../../ProcessedAssets/Assets" "$<TARGET_FILE_DIR:Game>/Assets")
endif()