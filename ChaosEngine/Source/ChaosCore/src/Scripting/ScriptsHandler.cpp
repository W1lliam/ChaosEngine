#include "ChaosCore/Scripting/ScriptsHandler.h"
#include "ChaosCore/Inputs/Input.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Rendering/IRenderer.h"
#include <ChaosMaths.h>
#include <Globals.h>
#include <KeyCodes.h>

namespace Chaos
{
    ScriptsHandler::ScriptsHandler()
    {
        m_virtualMachine = std::make_unique<ssq::VM>(1024, ssq::Libs::ALL);
        CHAOS_CORE_ASSERT(m_virtualMachine != nullptr, "Failed to initialize Squirrel");

        ExposeKeyCodes();
        ExposeLogger();
        ExposeVector3();
        Input::Expose(*m_virtualMachine);
    }

    ScriptsHandler::~ScriptsHandler()
    {
    }

    void ScriptsHandler::CompileComponents()
    {
        CompileAndRunFolder("Assets/DefaultResources/Scripts/");
        CompileAndRunFolder("Assets/Scripts/Components/");
    }
    void ScriptsHandler::CompileSystems()
    {
        CompileAndRunFolder("Assets/Scripts/Systems/", true);
    }

    void ScriptsHandler::CompileComponent(const std::string& p_componentName)
    {
        if (filesystem::path(p_componentName).extension() == ".nut")
        {
            try
            {
                ssq::Script sqScript = m_virtualMachine->compileFile(p_componentName.c_str());
                m_virtualMachine->run(sqScript);
            }
            catch (ssq::RuntimeException p_exception)
            {
                CHAOS_CORE_WARN("Script execution failed: {}", p_exception.what());
            }
            catch (ssq::CompileException p_exception)
            {
                CHAOS_CORE_WARN("Script compilation failed: {}", p_exception.what());
            }
        }
    }

    ScriptedSystem* ScriptsHandler::CompileSystem(const std::string& p_systemName)
    {
        filesystem::path path = "Assets/Scripts/Systems/" + p_systemName + ".nut";
        if (filesystem::exists(path))
        {
            try
            {
                ssq::Script sqScript = m_virtualMachine->compileFile(path.string().c_str());
                m_virtualMachine->run(sqScript);
                return &SystemManager::Get().RegisterScriptedSystem(*m_virtualMachine);
            }
            catch (ssq::RuntimeException p_exception)
            {
                CHAOS_CORE_WARN("Script execution failed: {}", p_exception.what());
            }
            catch (ssq::CompileException p_exception)
            {
                CHAOS_CORE_WARN("Script compilation failed: {}", p_exception.what());
            }
        }
            return nullptr;
    }

    void ScriptsHandler::CompileAndRunFolder(const filesystem::path& p_path, bool p_registerAsSystems)
    {
        for (auto& script : filesystem::recursive_directory_iterator(p_path))
        {
            if (script.path().extension() == ".nut")
            {
                try
                {
                    ssq::Script sqScript = m_virtualMachine->compileFile(script.path().string().c_str());
                    m_virtualMachine->run(sqScript);

                    if (p_registerAsSystems)
                    {
                        SystemManager::Get().RegisterScriptedSystem(*m_virtualMachine);
                    }
                }
                catch (ssq::RuntimeException p_exception)
                {
					CHAOS_CORE_WARN("Script execution failed: {}", p_exception.what());
                }
                catch (ssq::CompileException p_exception)
                {
					CHAOS_CORE_WARN("Script compilation failed: {}", p_exception.what());
                }
            }
        }
    }

    void ScriptsHandler::ExposeLogger()
    {
        ssq::Table inputTable = m_virtualMachine->addTable("Log");
        inputTable.addFunc("Trace",     [](std::string& message) { Debug::Log::Get().GetClientLogger()->trace(message);     });
        inputTable.addFunc("Info",      [](std::string& message) { Debug::Log::Get().GetClientLogger()->info(message);      });
        inputTable.addFunc("Warn",      [](std::string& message) { Debug::Log::Get().GetClientLogger()->warn(message);      });
        inputTable.addFunc("Error",     [](std::string& message) { Debug::Log::Get().GetClientLogger()->error(message);     });
        inputTable.addFunc("Critical",  [](std::string& message) { Debug::Log::Get().GetClientLogger()->critical(message);  });
    }

    void ScriptsHandler::ExposeVector3()
    {
        ssq::Class sqVector3 = m_virtualMachine->addClass("Vector3", ssq::Class::Ctor<Maths::Vector3(float,float,float)>());

        sqVector3.addVar("x", &Maths::Vector3::x);
        sqVector3.addVar("y", &Maths::Vector3::y);
        sqVector3.addVar("z", &Maths::Vector3::z);

        sqVector3.addFunc<Maths::Vector3>("_mul", &Maths::Vector3::operator*);
        sqVector3.addFunc("_add",       &Maths::Vector3::operator+);
        sqVector3.addFunc("_sub",       &Maths::Vector3::operator-);
        sqVector3.addFunc("_div",       &Maths::Vector3::operator/);
        sqVector3.addFunc("_tostring",  &Maths::Vector3::ToString);
        sqVector3.addFunc("tostring",   &Maths::Vector3::ToString);
        sqVector3.addFunc("_unm",       &Maths::Vector3::operator-);

        sqVector3.addFunc("Cross",      &Maths::Vector3::Cross);
        sqVector3.addFunc("Dot",        &Maths::Vector3::Dot);
        sqVector3.addFunc("Length",     &Maths::Vector3::Length);
        sqVector3.addFunc("Normalized", &Maths::Vector3::Normalized);
        sqVector3.addFunc("Normalize",  &Maths::Vector3::Normalize);

    }

    void ScriptsHandler::ExposeKeyCodes()
    {
        ssq::Enum keyCodes = m_virtualMachine->addEnum("KeyCode");
        {
            keyCodes.addSlot("CHAOS_KEY_UNKNOWN", (int)E_ChaosKeyCode::CHAOS_KEY_UNKNOWN);

            keyCodes.addSlot("CHAOS_KEY_SPACE", (int)E_ChaosKeyCode::CHAOS_KEY_SPACE);
            keyCodes.addSlot("CHAOS_KEY_APOSTROPHE", (int)E_ChaosKeyCode::CHAOS_KEY_APOSTROPHE);
            keyCodes.addSlot("CHAOS_KEY_COMMA", (int)E_ChaosKeyCode::CHAOS_KEY_COMMA);
            keyCodes.addSlot("CHAOS_KEY_MINUS", (int)E_ChaosKeyCode::CHAOS_KEY_MINUS);
            keyCodes.addSlot("CHAOS_KEY_PERIOD", (int)E_ChaosKeyCode::CHAOS_KEY_PERIOD);
            keyCodes.addSlot("CHAOS_KEY_SLASH", (int)E_ChaosKeyCode::CHAOS_KEY_SLASH);
            keyCodes.addSlot("CHAOS_KEY_0", (int)E_ChaosKeyCode::CHAOS_KEY_0);
            keyCodes.addSlot("CHAOS_KEY_1", (int)E_ChaosKeyCode::CHAOS_KEY_1);
            keyCodes.addSlot("CHAOS_KEY_2", (int)E_ChaosKeyCode::CHAOS_KEY_2);
            keyCodes.addSlot("CHAOS_KEY_3", (int)E_ChaosKeyCode::CHAOS_KEY_3);
            keyCodes.addSlot("CHAOS_KEY_4", (int)E_ChaosKeyCode::CHAOS_KEY_4);
            keyCodes.addSlot("CHAOS_KEY_5", (int)E_ChaosKeyCode::CHAOS_KEY_5);
            keyCodes.addSlot("CHAOS_KEY_6", (int)E_ChaosKeyCode::CHAOS_KEY_6);
            keyCodes.addSlot("CHAOS_KEY_7", (int)E_ChaosKeyCode::CHAOS_KEY_7);
            keyCodes.addSlot("CHAOS_KEY_8", (int)E_ChaosKeyCode::CHAOS_KEY_8);
            keyCodes.addSlot("CHAOS_KEY_9", (int)E_ChaosKeyCode::CHAOS_KEY_9);
            keyCodes.addSlot("CHAOS_KEY_SEMICOLON", (int)E_ChaosKeyCode::CHAOS_KEY_SEMICOLON);
            keyCodes.addSlot("CHAOS_KEY_EQUAL", (int)E_ChaosKeyCode::CHAOS_KEY_EQUAL);
            keyCodes.addSlot("CHAOS_KEY_A", (int)E_ChaosKeyCode::CHAOS_KEY_A);
            keyCodes.addSlot("CHAOS_KEY_B", (int)E_ChaosKeyCode::CHAOS_KEY_B);
            keyCodes.addSlot("CHAOS_KEY_C", (int)E_ChaosKeyCode::CHAOS_KEY_C);
            keyCodes.addSlot("CHAOS_KEY_D", (int)E_ChaosKeyCode::CHAOS_KEY_D);
            keyCodes.addSlot("CHAOS_KEY_E", (int)E_ChaosKeyCode::CHAOS_KEY_E);
            keyCodes.addSlot("CHAOS_KEY_F", (int)E_ChaosKeyCode::CHAOS_KEY_F);
            keyCodes.addSlot("CHAOS_KEY_G", (int)E_ChaosKeyCode::CHAOS_KEY_G);
            keyCodes.addSlot("CHAOS_KEY_H", (int)E_ChaosKeyCode::CHAOS_KEY_H);
            keyCodes.addSlot("CHAOS_KEY_I", (int)E_ChaosKeyCode::CHAOS_KEY_I);
            keyCodes.addSlot("CHAOS_KEY_J", (int)E_ChaosKeyCode::CHAOS_KEY_J);
            keyCodes.addSlot("CHAOS_KEY_K", (int)E_ChaosKeyCode::CHAOS_KEY_K);
            keyCodes.addSlot("CHAOS_KEY_L", (int)E_ChaosKeyCode::CHAOS_KEY_L);
            keyCodes.addSlot("CHAOS_KEY_M", (int)E_ChaosKeyCode::CHAOS_KEY_M);
            keyCodes.addSlot("CHAOS_KEY_N", (int)E_ChaosKeyCode::CHAOS_KEY_N);
            keyCodes.addSlot("CHAOS_KEY_O", (int)E_ChaosKeyCode::CHAOS_KEY_O);
            keyCodes.addSlot("CHAOS_KEY_P", (int)E_ChaosKeyCode::CHAOS_KEY_P);
            keyCodes.addSlot("CHAOS_KEY_Q", (int)E_ChaosKeyCode::CHAOS_KEY_Q);
            keyCodes.addSlot("CHAOS_KEY_R", (int)E_ChaosKeyCode::CHAOS_KEY_R);
            keyCodes.addSlot("CHAOS_KEY_S", (int)E_ChaosKeyCode::CHAOS_KEY_S);
            keyCodes.addSlot("CHAOS_KEY_T", (int)E_ChaosKeyCode::CHAOS_KEY_T);
            keyCodes.addSlot("CHAOS_KEY_U", (int)E_ChaosKeyCode::CHAOS_KEY_U);
            keyCodes.addSlot("CHAOS_KEY_V", (int)E_ChaosKeyCode::CHAOS_KEY_V);
            keyCodes.addSlot("CHAOS_KEY_W", (int)E_ChaosKeyCode::CHAOS_KEY_W);
            keyCodes.addSlot("CHAOS_KEY_X", (int)E_ChaosKeyCode::CHAOS_KEY_X);
            keyCodes.addSlot("CHAOS_KEY_Y", (int)E_ChaosKeyCode::CHAOS_KEY_Y);
            keyCodes.addSlot("CHAOS_KEY_Z", (int)E_ChaosKeyCode::CHAOS_KEY_Z);
            keyCodes.addSlot("CHAOS_KEY_LEFT_BRACKET", (int)E_ChaosKeyCode::CHAOS_KEY_LEFT_BRACKET);
            keyCodes.addSlot("CHAOS_KEY_BACKSLASH", (int)E_ChaosKeyCode::CHAOS_KEY_BACKSLASH);
            keyCodes.addSlot("CHAOS_KEY_RIGHT_BRACKET", (int)E_ChaosKeyCode::CHAOS_KEY_RIGHT_BRACKET);
            keyCodes.addSlot("CHAOS_KEY_GRAVE_ACCENT", (int)E_ChaosKeyCode::CHAOS_KEY_GRAVE_ACCENT);

            keyCodes.addSlot("CHAOS_KEY_ESCAPE", (int)E_ChaosKeyCode::CHAOS_KEY_ESCAPE);
            keyCodes.addSlot("CHAOS_KEY_ENTER", (int)E_ChaosKeyCode::CHAOS_KEY_ENTER);
            keyCodes.addSlot("CHAOS_KEY_TAB", (int)E_ChaosKeyCode::CHAOS_KEY_TAB);
            keyCodes.addSlot("CHAOS_KEY_BACKSPACE", (int)E_ChaosKeyCode::CHAOS_KEY_BACKSPACE);
            keyCodes.addSlot("CHAOS_KEY_INSERT", (int)E_ChaosKeyCode::CHAOS_KEY_INSERT);
            keyCodes.addSlot("CHAOS_KEY_DELETE", (int)E_ChaosKeyCode::CHAOS_KEY_DELETE);
            keyCodes.addSlot("CHAOS_KEY_RIGHT", (int)E_ChaosKeyCode::CHAOS_KEY_RIGHT);
            keyCodes.addSlot("CHAOS_KEY_LEFT", (int)E_ChaosKeyCode::CHAOS_KEY_LEFT);
            keyCodes.addSlot("CHAOS_KEY_DOWN", (int)E_ChaosKeyCode::CHAOS_KEY_DOWN);
            keyCodes.addSlot("CHAOS_KEY_UP", (int)E_ChaosKeyCode::CHAOS_KEY_UP);
            keyCodes.addSlot("CHAOS_KEY_PAGE_UP", (int)E_ChaosKeyCode::CHAOS_KEY_PAGE_UP);
            keyCodes.addSlot("CHAOS_KEY_PAGE_DOWN", (int)E_ChaosKeyCode::CHAOS_KEY_PAGE_DOWN);
            keyCodes.addSlot("CHAOS_KEY_HOME", (int)E_ChaosKeyCode::CHAOS_KEY_HOME);
            keyCodes.addSlot("CHAOS_KEY_END", (int)E_ChaosKeyCode::CHAOS_KEY_END);
            keyCodes.addSlot("CHAOS_KEY_CAPS_LOCK", (int)E_ChaosKeyCode::CHAOS_KEY_CAPS_LOCK);
            keyCodes.addSlot("CHAOS_KEY_SCROLL_LOCK", (int)E_ChaosKeyCode::CHAOS_KEY_SCROLL_LOCK);
            keyCodes.addSlot("CHAOS_KEY_NUM_LOCK", (int)E_ChaosKeyCode::CHAOS_KEY_NUM_LOCK);
            keyCodes.addSlot("CHAOS_KEY_PRINT_SCREEN", (int)E_ChaosKeyCode::CHAOS_KEY_PRINT_SCREEN);
            keyCodes.addSlot("CHAOS_KEY_PAUSE", (int)E_ChaosKeyCode::CHAOS_KEY_PAUSE);
            keyCodes.addSlot("CHAOS_KEY_F1", (int)E_ChaosKeyCode::CHAOS_KEY_F1);
            keyCodes.addSlot("CHAOS_KEY_F2", (int)E_ChaosKeyCode::CHAOS_KEY_F2);
            keyCodes.addSlot("CHAOS_KEY_F3", (int)E_ChaosKeyCode::CHAOS_KEY_F3);
            keyCodes.addSlot("CHAOS_KEY_F4", (int)E_ChaosKeyCode::CHAOS_KEY_F4);
            keyCodes.addSlot("CHAOS_KEY_F5", (int)E_ChaosKeyCode::CHAOS_KEY_F5);
            keyCodes.addSlot("CHAOS_KEY_F6", (int)E_ChaosKeyCode::CHAOS_KEY_F6);
            keyCodes.addSlot("CHAOS_KEY_F7", (int)E_ChaosKeyCode::CHAOS_KEY_F7);
            keyCodes.addSlot("CHAOS_KEY_F8", (int)E_ChaosKeyCode::CHAOS_KEY_F8);
            keyCodes.addSlot("CHAOS_KEY_F9", (int)E_ChaosKeyCode::CHAOS_KEY_F9);
            keyCodes.addSlot("CHAOS_KEY_F10", (int)E_ChaosKeyCode::CHAOS_KEY_F10);
            keyCodes.addSlot("CHAOS_KEY_F11", (int)E_ChaosKeyCode::CHAOS_KEY_F11);
            keyCodes.addSlot("CHAOS_KEY_F12", (int)E_ChaosKeyCode::CHAOS_KEY_F12);
            keyCodes.addSlot("CHAOS_KEY_F13", (int)E_ChaosKeyCode::CHAOS_KEY_F13);
            keyCodes.addSlot("CHAOS_KEY_F14", (int)E_ChaosKeyCode::CHAOS_KEY_F14);
            keyCodes.addSlot("CHAOS_KEY_F15", (int)E_ChaosKeyCode::CHAOS_KEY_F15);
            keyCodes.addSlot("CHAOS_KEY_F16", (int)E_ChaosKeyCode::CHAOS_KEY_F16);
            keyCodes.addSlot("CHAOS_KEY_F17", (int)E_ChaosKeyCode::CHAOS_KEY_F17);
            keyCodes.addSlot("CHAOS_KEY_F18", (int)E_ChaosKeyCode::CHAOS_KEY_F18);
            keyCodes.addSlot("CHAOS_KEY_F19", (int)E_ChaosKeyCode::CHAOS_KEY_F19);
            keyCodes.addSlot("CHAOS_KEY_F20", (int)E_ChaosKeyCode::CHAOS_KEY_F20);
            keyCodes.addSlot("CHAOS_KEY_F21", (int)E_ChaosKeyCode::CHAOS_KEY_F21);
            keyCodes.addSlot("CHAOS_KEY_F22", (int)E_ChaosKeyCode::CHAOS_KEY_F22);
            keyCodes.addSlot("CHAOS_KEY_F23", (int)E_ChaosKeyCode::CHAOS_KEY_F23);
            keyCodes.addSlot("CHAOS_KEY_F24", (int)E_ChaosKeyCode::CHAOS_KEY_F24);
            keyCodes.addSlot("CHAOS_KEY_F25", (int)E_ChaosKeyCode::CHAOS_KEY_F25);
            keyCodes.addSlot("CHAOS_KEY_KP_0", (int)E_ChaosKeyCode::CHAOS_KEY_KP_0);
            keyCodes.addSlot("CHAOS_KEY_KP_1", (int)E_ChaosKeyCode::CHAOS_KEY_KP_1);
            keyCodes.addSlot("CHAOS_KEY_KP_2", (int)E_ChaosKeyCode::CHAOS_KEY_KP_2);
            keyCodes.addSlot("CHAOS_KEY_KP_3", (int)E_ChaosKeyCode::CHAOS_KEY_KP_3);
            keyCodes.addSlot("CHAOS_KEY_KP_4", (int)E_ChaosKeyCode::CHAOS_KEY_KP_4);
            keyCodes.addSlot("CHAOS_KEY_KP_5", (int)E_ChaosKeyCode::CHAOS_KEY_KP_5);
            keyCodes.addSlot("CHAOS_KEY_KP_6", (int)E_ChaosKeyCode::CHAOS_KEY_KP_6);
            keyCodes.addSlot("CHAOS_KEY_KP_7", (int)E_ChaosKeyCode::CHAOS_KEY_KP_7);
            keyCodes.addSlot("CHAOS_KEY_KP_8", (int)E_ChaosKeyCode::CHAOS_KEY_KP_8);
            keyCodes.addSlot("CHAOS_KEY_KP_9", (int)E_ChaosKeyCode::CHAOS_KEY_KP_9);
            keyCodes.addSlot("CHAOS_KEY_KP_DECIMAL", (int)E_ChaosKeyCode::CHAOS_KEY_KP_DECIMAL);
            keyCodes.addSlot("CHAOS_KEY_KP_DIVIDE", (int)E_ChaosKeyCode::CHAOS_KEY_KP_DIVIDE);
            keyCodes.addSlot("CHAOS_KEY_KP_MULTIPLY", (int)E_ChaosKeyCode::CHAOS_KEY_KP_MULTIPLY);
            keyCodes.addSlot("CHAOS_KEY_KP_SUBTRACT", (int)E_ChaosKeyCode::CHAOS_KEY_KP_SUBTRACT);
            keyCodes.addSlot("CHAOS_KEY_KP_ADD", (int)E_ChaosKeyCode::CHAOS_KEY_KP_ADD);
            keyCodes.addSlot("CHAOS_KEY_KP_ENTER", (int)E_ChaosKeyCode::CHAOS_KEY_KP_ENTER);
            keyCodes.addSlot("CHAOS_KEY_KP_EQUAL", (int)E_ChaosKeyCode::CHAOS_KEY_KP_EQUAL);
            keyCodes.addSlot("CHAOS_KEY_LEFT_SHIFT", (int)E_ChaosKeyCode::CHAOS_KEY_LEFT_SHIFT);
            keyCodes.addSlot("CHAOS_KEY_LEFT_CONTROL", (int)E_ChaosKeyCode::CHAOS_KEY_LEFT_CONTROL);
            keyCodes.addSlot("CHAOS_KEY_LEFT_ALT", (int)E_ChaosKeyCode::CHAOS_KEY_LEFT_ALT);
            keyCodes.addSlot("CHAOS_KEY_LEFT_SUPER", (int)E_ChaosKeyCode::CHAOS_KEY_LEFT_SUPER);
            keyCodes.addSlot("CHAOS_KEY_RIGHT_SHIFT", (int)E_ChaosKeyCode::CHAOS_KEY_RIGHT_SHIFT);
            keyCodes.addSlot("CHAOS_KEY_RIGHT_CONTROL", (int)E_ChaosKeyCode::CHAOS_KEY_RIGHT_CONTROL);
            keyCodes.addSlot("CHAOS_KEY_RIGHT_ALT", (int)E_ChaosKeyCode::CHAOS_KEY_RIGHT_ALT);
            keyCodes.addSlot("CHAOS_KEY_RIGHT_SUPER", (int)E_ChaosKeyCode::CHAOS_KEY_RIGHT_SUPER);
            keyCodes.addSlot("CHAOS_KEY_MENU", (int)E_ChaosKeyCode::CHAOS_KEY_MENU);

            keyCodes.addSlot("CHAOS_KEY_LAST", (int)E_ChaosKeyCode::CHAOS_KEY_LAST);
        }

        ssq::Enum keyModFlag = m_virtualMachine->addEnum("KeyModFlag");
        keyModFlag.addSlot("CHAOS_MOD_SHIFT", (int)E_ChaosKeyModFlag::CHAOS_MOD_SHIFT);
        keyModFlag.addSlot("CHAOS_MOD_CONTROL", (int)E_ChaosKeyModFlag::CHAOS_MOD_CONTROL);
        keyModFlag.addSlot("CHAOS_MOD_ALT", (int)E_ChaosKeyModFlag::CHAOS_MOD_ALT);
        keyModFlag.addSlot("CHAOS_MOD_SUPER", (int)E_ChaosKeyModFlag::CHAOS_MOD_SUPER);

        ssq::Enum mouseCode = m_virtualMachine->addEnum("MouseCode");
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_1", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_1);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_2", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_2);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_3", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_3);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_4", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_4);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_5", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_5);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_6", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_6);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_7", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_7);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_8", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_8);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_LAST", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_LAST);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_LEFT", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_LEFT);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_RIGHT", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_RIGHT);
        mouseCode.addSlot("CHAOS_MOUSE_BUTTON_MIDDLE", (int)E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_MIDDLE);

        ssq::Enum joystickCode = m_virtualMachine->addEnum("JoystickCode");
        joystickCode.addSlot("CHAOS_JOYSTICK_1", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_1);
        joystickCode.addSlot("CHAOS_JOYSTICK_2", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_2);
        joystickCode.addSlot("CHAOS_JOYSTICK_3", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_3);
        joystickCode.addSlot("CHAOS_JOYSTICK_4", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_4);
        joystickCode.addSlot("CHAOS_JOYSTICK_5", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_5);
        joystickCode.addSlot("CHAOS_JOYSTICK_6", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_6);
        joystickCode.addSlot("CHAOS_JOYSTICK_7", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_7);
        joystickCode.addSlot("CHAOS_JOYSTICK_8", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_8);
        joystickCode.addSlot("CHAOS_JOYSTICK_9", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_9);
        joystickCode.addSlot("CHAOS_JOYSTICK_10", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_10);
        joystickCode.addSlot("CHAOS_JOYSTICK_11", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_11);
        joystickCode.addSlot("CHAOS_JOYSTICK_12", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_12);
        joystickCode.addSlot("CHAOS_JOYSTICK_13", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_13);
        joystickCode.addSlot("CHAOS_JOYSTICK_14", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_14);
        joystickCode.addSlot("CHAOS_JOYSTICK_15", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_15);
        joystickCode.addSlot("CHAOS_JOYSTICK_16", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_16);
        joystickCode.addSlot("CHAOS_JOYSTICK_LAST", (int)E_ChaosJoystickCode::CHAOS_JOYSTICK_LAST);
    }
}