
/****************
*   CONTAINERS  *
*****************/
#include <vector>
#include <unordered_map>


/****************
*    STRINGS    *
*****************/
#include <string>
#include <sstream>
#include <cassert>
#include <memory>

/****************
*    STREAMS    *
*****************/
#include <iostream>
#include <fstream>

/****************
*   PLATFORMS   *
*****************/
#ifdef CHAOS_PLATFORM_WINDOWS
    #include <Windows.h>
#endif

/****************
*  GRAPHICS API *
*****************/
#ifdef CHAOS_OPENGL
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
#endif