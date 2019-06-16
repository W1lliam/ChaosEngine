cmake_minimum_required(VERSION 3.12.2)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(INCLUDEDIR "include/ChaosEditor")
set(SOURCEDIR "src")
set(CHAOSENGINEDIR "${CMAKE_SOURCE_DIR}/../../ChaosEngine")

set(QT_DIR "D:/Qt/5.12.0/msvc2017_64/")
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui PATHS "${QT_DIR}/lib/cmake/Qt5") # Core, Widgets, Gui are the required for now
set_target_properties(Qt5::Core Qt5::Widgets Qt5::Gui PROPERTIES MAP_IMPORTED_CONFIG_DEVELOPMENT "DEBUG")


set(HEADER_FILES 
        ${INCLUDEDIR}/editor.h
        ${INCLUDEDIR}/TabWidget.h
        ${INCLUDEDIR}/CameraSystem.h
        ${INCLUDEDIR}/Console/Console.h
        ${INCLUDEDIR}/Console/EditorSink.h
        ${INCLUDEDIR}/InputHandler.h
        ${INCLUDEDIR}/MenuBar.h
        ${INCLUDEDIR}/Hierarchy.h
        ${INCLUDEDIR}/LevelEditor.h
        
        #InspectorUI
        ${INCLUDEDIR}/Inspector/Inspector.h
        ${INCLUDEDIR}/Inspector/Category.h
        ${INCLUDEDIR}/Inspector/DragLabel.h
        ${INCLUDEDIR}/Inspector/DragSpinBox.h
        ${INCLUDEDIR}/Inspector/RangeEdit.h
        ${INCLUDEDIR}/Inspector/Vector2Widget.h
        ${INCLUDEDIR}/Inspector/Vector3Widget.h
        ${INCLUDEDIR}/Inspector/QuaternionWidget.h
        ${INCLUDEDIR}/Inspector/ColorPicker.h
        ${INCLUDEDIR}/Inspector/ComponentDialog.h
        ${INCLUDEDIR}/Inspector/MaterialGUI.h
        
        #ComponentsGUI
        ${INCLUDEDIR}/ComponentsGUI/AComponentGUI.h
        ${INCLUDEDIR}/ComponentsGUI/ComponentGUIFactory.h
        ${INCLUDEDIR}/ComponentsGUI/ScriptedComponentGUI.h
        ${INCLUDEDIR}/ComponentsGUI/TransformComponentGUI.h
        ${INCLUDEDIR}/ComponentsGUI/LightComponentGUI.h
        ${INCLUDEDIR}/ComponentsGUI/ChildComponentGUI.h
        ${INCLUDEDIR}/ComponentsGUI/CameraComponentGUI.h
        ${INCLUDEDIR}/ComponentsGUI/MaterialComponentGUI.h
        ${INCLUDEDIR}/ComponentsGUI/MeshComponentGUI.h
		${INCLUDEDIR}/ComponentsGUI/SphereColliderComponentGUI.h
		${INCLUDEDIR}/ComponentsGUI/BoxColliderComponentGUI.h
		${INCLUDEDIR}/ComponentsGUI/CapsuleColliderComponentGUI.h
		${INCLUDEDIR}/ComponentsGUI/RigidBodyComponentGUI.h
		${INCLUDEDIR}/ComponentsGUI/AudioSourceComponentGUI.h
		${INCLUDEDIR}/ComponentsGUI/AnimationComponentGUI.h

		#Gizmos
		${INCLUDEDIR}/Gizmo/im3d.h
		${INCLUDEDIR}/Gizmo/im3d_config.h
		${INCLUDEDIR}/Gizmo/im3d_math.h
		${INCLUDEDIR}/Gizmo/Gizmo.h
         )

set(SOURCE_FILES
        ${SOURCEDIR}/main.cpp
        ${SOURCEDIR}/editor.cpp
        ${SOURCEDIR}/LevelEditor.cpp
        ${SOURCEDIR}/CameraSystem.cpp
        ${SOURCEDIR}/TabWidget.cpp
        ${SOURCEDIR}/Console/Console.cpp
        ${SOURCEDIR}/MenuBar.cpp
        ${SOURCEDIR}/InputHandler.cpp
        ${SOURCEDIR}/Hierarchy.cpp

        #InspectorUI
        ${SOURCEDIR}/Inspector/Inspector.cpp
        ${SOURCEDIR}/Inspector/Category.cpp
        ${SOURCEDIR}/Inspector/DragLabel.cpp
        ${SOURCEDIR}/Inspector/DragSpinBox.cpp
        ${SOURCEDIR}/Inspector/RangeEdit.cpp
        ${SOURCEDIR}/Inspector/Vector3Widget.cpp
        ${SOURCEDIR}/Inspector/Vector2Widget.cpp
        ${SOURCEDIR}/Inspector/QuaternionWidget.cpp
        ${SOURCEDIR}/Inspector/ColorPicker.cpp
        ${SOURCEDIR}/Inspector/ComponentDialog.cpp
        ${SOURCEDIR}/Inspector/MaterialGUI.cpp

        #ComponentsGUI
        ${SOURCEDIR}/ComponentsGUI/AComponentGUI.cpp
        ${SOURCEDIR}/ComponentsGUI/ComponentGUIFactory.cpp
        ${SOURCEDIR}/ComponentsGUI/ScriptedComponentGUI.cpp
        ${SOURCEDIR}/ComponentsGUI/TransformComponentGUI.cpp
        ${SOURCEDIR}/ComponentsGUI/LightComponentGUI.cpp
        ${SOURCEDIR}/ComponentsGUI/ChildComponentGUI.cpp
        ${SOURCEDIR}/ComponentsGUI/CameraComponentGUI.cpp
        ${SOURCEDIR}/ComponentsGUI/MaterialComponentGUI.cpp
        ${SOURCEDIR}/ComponentsGUI/MeshComponentGUI.cpp
		${SOURCEDIR}/ComponentsGUI/SphereColliderComponentGUI.cpp
		${SOURCEDIR}/ComponentsGUI/BoxColliderComponentGUI.cpp
		${SOURCEDIR}/ComponentsGUI/CapsuleColliderComponentGUI.cpp
		${SOURCEDIR}/ComponentsGUI/RigidBodyComponentGUI.cpp
		${SOURCEDIR}/ComponentsGUI/AudioSourceComponentGUI.cpp
		${SOURCEDIR}/ComponentsGUI/AnimationComponentGUI.cpp
		#Gizmo
        ${SOURCEDIR}/Gizmo/im3d.cpp
        ${SOURCEDIR}/Gizmo/Gizmo.cpp
         )

link_directories("${CHAOSENGINEDIR}/Build/lib/$<CONFIGURATION>")
link_directories("${CHAOSENGINEDIR}/Source/Dependencies/glfw/lib")
link_directories("${CHAOSENGINEDIR}/Source/Dependencies/squirrel/lib")
link_directories("${CHAOSENGINEDIR}/Source/Dependencies/squirrel/lib")
link_directories("${CHAOSENGINEDIR}/Source/Dependencies/FMOD/api/core/lib")
link_directories("${CHAOSENGINEDIR}/Source/Dependencies/FMOD/api/studio/lib")

add_compile_definitions(_ENABLE_EXTENDED_ALIGNED_STORAGE _SQ64 QT_NO_FOREACH)

GroupSources("${SOURCE_FILES}") 
GroupSources("${HEADER_FILES}") 


IF(WIN32)
	add_library(PHYSX STATIC IMPORTED GLOBAL) 
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_DEBUG ${CHAOSENGINEDIR}/Source/Dependencies/PhysX/physx/lib/window/debug/*.lib)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_DEVELOPMENT ${CHAOSENGINEDIR}/Source/Dependencies/PhysX/physx/lib/window/debug/*.lib)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_SHIPPING ${CHAOSENGINEDIR}/Source/Dependencies/PhysX/physx/lib/window/shipping/*.lib)
    target_include_directories(PHYSX INTERFACE "${CHAOSENGINEDIR}/Source/Dependencies/PhysX"
									 INTERFACE "${CHAOSENGINEDIR}/Source/Dependencies/PhysX/physx/include"
									 INTERFACE "${CHAOSENGINEDIR}/Source/Dependencies/PhysX/pxshared/include")
ELSE()
	add_library(PHYSX STATIC IMPORTED)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_DEBUG ${CHAOSENGINEDIR}/Source/Dependencies/PhysX/physx/lib/linux/debug/*.so)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_DEVELOPMENT ${CHAOSENGINEDIR}/Source/Dependencies/PhysX/physx/lib/linux/debug/*.so)
	set_property(TARGET PHYSX PROPERTY IMPORTED_LOCATION_SHIPPING ${CHAOSENGINEDIR}/Source/Dependencies/PhysX/physx/lib/linux/release/*.so)
    target_include_directories(PHYSX INTERFACE "${CHAOSENGINEDIR}/Source/Dependencies/PhysX"
									 INTERFACE "${CHAOSENGINEDIR}/Source/Dependencies/PhysX/physx/include"
									 INTERFACE "${CHAOSENGINEDIR}/Source/Dependencies/PhysX/pxshared/include")
ENDIF()

add_executable(ChaosEditor	${HEADER_FILES} ${SOURCE_FILES})


target_link_libraries(ChaosEditor	PUBLIC ChaosAssetsManager
									PUBLIC Qt5::Core
									PUBLIC Qt5::Gui
									PUBLIC Qt5::Widgets
                                    PUBLIC ChaosEngine  
                                    PUBLIC ChaosCore    
                                    PUBLIC ChaosDebug   
                                    PUBLIC ChaosMaths   
                                    PUBLIC ChaosGraphics
                                    PUBLIC ChaosPhysics
                                    PUBLIC glad
									PUBLIC glfw3
                                    PUBLIC PHYSX
									PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/squirrel/lib/$<CONFIG>/sqstdlib_static.lib"
                                    PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/squirrel/lib/$<CONFIG>/squirrel_static.lib"
                                    PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/simpleSquirrel/lib/$<CONFIG>/simplesquirrel_static.lib"
                                    PUBLIC "fmod$<IF:$<CONFIG:Shipping>,,L>_vc.lib"
								    PUBLIC "fmodstudio$<IF:$<CONFIG:Shipping>,,L>_vc.lib"
									)         


target_include_directories(ChaosEditor	PUBLIC "include"
										PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/spdlog/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/glad/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/ChaosEngine/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/ChaosCore/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/ChaosDebug/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/ChaosMaths/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/ChaosGraphics/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/ChaosPhysics/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/glfw/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/squirrel/include"
										PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/simpleSquirrel/include"
                                        PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/FMOD/api/core/inc"
                                        PUBLIC "${CHAOSENGINEDIR}/Source/Dependencies/FMOD/api/studio/inc"
                                        )

add_custom_command(TARGET ChaosEditor POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT_DIR}/bin/Qt5Widgets$<$<NOT:$<CONFIG:Shipping>>:d>.dll" $<TARGET_FILE_DIR:ChaosEditor>
					COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT_DIR}/bin/Qt5Core$<$<NOT:$<CONFIG:Shipping>>:d>.dll" $<TARGET_FILE_DIR:ChaosEditor>
					COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT_DIR}/bin/Qt5Gui$<$<NOT:$<CONFIG:Shipping>>:d>.dll" $<TARGET_FILE_DIR:ChaosEditor>
					COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CHAOSENGINEDIR}/Build/bin/$<CONFIG>/ChaosEngine.dll" $<TARGET_FILE_DIR:ChaosEditor>
	                COMMAND ${CMAKE_COMMAND} -E copy_directory    "${CHAOSENGINEDIR}/Source/Dependencies/PhysX/physx/lib/window/$<IF:$<CONFIG:Shipping>,shipping,debug>/dll" $<TARGET_FILE_DIR:ChaosEditor>
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CHAOSENGINEDIR}/Source/Dependencies/FMOD/api/core/lib/fmod$<$<NOT:$<CONFIG:Shipping>>:L>.dll" $<TARGET_FILE_DIR:ChaosEditor>
		            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CHAOSENGINEDIR}/Source/Dependencies/FMOD/api/studio/lib/fmodstudio$<$<NOT:$<CONFIG:Shipping>>:L>.dll" $<TARGET_FILE_DIR:ChaosEditor>

                    COMMAND ${CMAKE_COMMAND} -E copy_directory    "${CMAKE_SOURCE_DIR}/../../ProcessedAssets/Assets" "$<TARGET_FILE_DIR:ChaosEditor>/Assets"
                    COMMAND ${CMAKE_COMMAND} -E copy_directory    "${CMAKE_SOURCE_DIR}/../EditorResources" "$<TARGET_FILE_DIR:ChaosEditor>/EditorResources"
                )

