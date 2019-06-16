set(DEFAULT_OUT_OF_SOURCE_FOLDER "build")

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(WARNING "In-source build attempt")
    message("##############################################################################")
    message("# ChaosEngine should not be built in the source directory                    #") 
    message("# You should run cmake in a build directory.                                 #")
    message("# For example:                                                               #")
    message("# mkdir ChaosBuild                                                           #")
    message("# cd ChaosBuild                                                              #")
    message("# cmake .. # or ccmake, or cmake-gui                                         #")
    message("# We will run a new build from a ChaosBuild folder with the command          #")
    message("# cmake -S . -B ChaosBuild                                                   #")
    message("##############################################################################")


    
    
    # Run CMake with out of source flag
    execute_process(
        COMMAND ${CMAKE_COMMAND} -S . -B ${DEFAULT_OUT_OF_SOURCE_FOLDER}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
        
    message("##############################################################################")
    message("# NOTE: Given that you already tried to make an in-source build              #")
    message("#       CMake have already created several files & directories               #")
    message("#       in your source tree like a CMakeFiles folder                         #")
    message("#       and a CMakeCache.txt file.                                           #")
    message("##############################################################################")
        
    # Cause fatal error to stop the script from further execution
    message("Quitting configuration.")
    message(FATAL_ERROR "This error prevents CMake from running the initial in-source build.")
    
endif ()