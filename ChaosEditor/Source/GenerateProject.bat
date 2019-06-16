@IF NOT EXIST build\ (
MKDIR build 
)
@CD build
cmake .. -A x64
@PAUSE