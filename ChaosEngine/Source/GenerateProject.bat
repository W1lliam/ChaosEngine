@IF NOT EXIST build\ (
MKDIR build 
)
@CD build
cmake .. -G "Visual Studio 15 2017" -A x64
@PAUSE