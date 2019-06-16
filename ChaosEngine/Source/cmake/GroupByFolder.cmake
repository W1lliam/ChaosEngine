
macro(GroupSources elements)
# Include all header and c files
#file(GLOB_RECURSE elements RELATIVE ${dir} *.h *.hpp *.c *.cpp *.cc)
foreach(element ${elements})
    # Extract filename and directory
    get_filename_component(element_name ${element} NAME)
    get_filename_component(element_dir ${element} DIRECTORY)

    if (NOT ${element_dir} STREQUAL "")
    # If the file is in a subdirectory use it as source group.

        # Use the full hierarchical structure to build source_groups.
        string(REPLACE "/" "\\" group_name ${element_dir})
        source_group("${group_name}" FILES ${CMAKE_CURRENT_SOURCE_DIR}/${element})
    else()
        # If the file is in the root directory, place it in the root source_group.
        source_group("\\" FILES ${CMAKE_CURRENT_SOURCE_DIR}/${element})
    endif()
endforeach()
endmacro()