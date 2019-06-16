/*****************
*   CONTAINERS   *
******************/
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include <list>
#include <algorithm>
#include <unordered_set>

/*****************
* MULTITHREADING *
******************/
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <future>


/*****************
*    STRINGS     *
******************/
#include <string>
#include <cassert>
#include <memory>

/*****************
*    STREAMS     *
******************/
#include <iostream>
#include <fstream>
#include <sstream>

/*****************
*    NUMERICS    *
******************/
#include <climits>

/******************
*    UTILITIES    *
*******************/
#include <functional>
#if defined(CHAOS_PLATFORM_UNIX) || (defined(CHAOS_PLATFORM_WINDOWS) && _MSC_VER < 1914)
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#else
#include <filesystem>
namespace filesystem = std::filesystem;
#endif


/******************
*    SCRIPTING    *
*******************/
#include <squirrel.h>
#include <simplesquirrel/simplesquirrel.hpp>