# Chaos Engine™
[![Engine Video](https://i.imgur.com/0lX2Fsu.jpg)](https://www.youtube.com/watch?v=Ox2pbvT2vFY "Chaos Engine Video")
Developed by Team Mayhem™

## Description
Chaos Engine is a C++/Opengl (later added support for Linux and Vulkan) developed by a team of 3 devs called Team Mayhem during a 5 month time period as part of a graduation project, The objective of this project was to make a Game engine from scratch using as few libraries as possible.

## Constraints
- The editor must work on Windows (Later added support for Linux).
- The engine must use a custom mathematics library.
- The engine must support OpenGL as rendering API (Later added Vulkan).
- The engine can use an existing physic library.

## My part
During the development process i was responsible for:
- Memory management
- Resources management
- Setting up an Entity Component System.
- Basic Rendering API Wrapping
- Scripting integration (using Squirrel)
- Scene Management & Scene System
- Editor Inspector
- Image Based Lighting
- Skybox Preprocessing

## Libraries used
- PhysX
- Assimp
- SpdLog
- OpenGL
- Qt

## How to Buid Chaos Engine
CMake 3.12.2 or newer is needed to complete the build.
First under ChaosEngine/Source folder run GenerateProject.bat to generate the VS solution,
this will create a build folder that will contain said solution.
Solution's output will be located at ChaosEngine/Build.

# Chaos Editor™
Developed by Team Mayhem™

![alt text](https://i.imgur.com/KeMjqai.jpg)

# How to Build Chaos Editor
First install Qt 5.12.0 in C:/ or specify in ChaosEditor/Source/ChaosEditor/CMakeLists.txt the Qt folder path then under ChaosEditor/Source run GenerateProject.bat to generate the VS solution, this will create a build folder that will contain said solution.
Solution's output will be located at ChaosEditor/Build.

## General Inputs
 Right Click: Fly mode.
 Middle Click: Pan mode.
 Alt: Orbit mode.
 Left Shift: Move faster.
 
 Fly Mode:
  W: Move forward.
  A: Move left.
  S: Move backward.
  D: Move right.
  Q: Move down.
  E: Move up.

 Pan Mode:
  Mouse drag: Pan.

 Orbit Mode:
   Left Click & Drag: Orbit.

 ## Editor Inputs
 
 Q: Select tool.
 W: Move tool.
 E: Rotate tool.
 R: Scale tool.
