#include "ChaosEditor/InputHandler.h"
#include <QCursor>
#include <QApplication>
#include <ChaosEngine/Loader.h>

std::map<Qt::Key, bool> InputHandler::keyInput;
std::map<Qt::MouseButton, bool> InputHandler::mouseInput;

void InputHandler::Init()
{
    Chaos::Loader::Get().SetInput(new InputHandler);
}

E_ChaosKeyCode InputHandler::QtToChaosKeyCode(Qt::Key p_code)
{
    auto key = std::find_if(std::begin(s_keyCodeMap), std::end(s_keyCodeMap), [&](const std::pair<E_ChaosKeyCode, Qt::Key> &pair)
    {
        return pair.second == p_code;
    });
    if (key != s_keyCodeMap.end())
        return key->first;
    return E_ChaosKeyCode::CHAOS_KEY_UNKNOWN;
}

Qt::Key InputHandler::ChaosToQtKeyCode(E_ChaosKeyCode p_code)
{
    return s_keyCodeMap.at(p_code);
}

E_ChaosMouseCode InputHandler::QtToChaosMouseCode(Qt::MouseButton p_code)
{
    return std::find_if(std::begin(s_mouseCodeMap), std::end(s_mouseCodeMap), [&](const std::pair<E_ChaosMouseCode, Qt::MouseButton> &pair)
    {
        return pair.second == p_code;
    })->first;
}

Qt::MouseButton InputHandler::ChaosToQtMouseCode(E_ChaosMouseCode p_code)
{
    return s_mouseCodeMap.at(p_code);
}

bool InputHandler::IsKeyPressed(E_ChaosKeyCode keycode)
{
    return keyInput[s_keyCodeMap.at(keycode)];
}

bool InputHandler::IsKeyReleased(E_ChaosKeyCode keycode)
{
    return !keyInput[s_keyCodeMap.at(keycode)];
}

bool InputHandler::IsMouseButtonPressed(E_ChaosMouseCode button)
{
    return mouseInput[s_mouseCodeMap.at(button)];
}

bool InputHandler::IsMouseButtonReleased(E_ChaosMouseCode button)
{
    return !mouseInput[s_mouseCodeMap.at(button)];
}

Chaos::Input::MousePos InputHandler::GetMousePos()
{
    return { QCursor::pos().x(), QCursor::pos().y() };
}

float InputHandler::GetMouseX()
{
    return  QCursor::pos().x();
}

float InputHandler::GetMouseY()
{
    return QCursor::pos().y();
}

void InputHandler::SetMousePosition(double p_x, double p_y)
{
    QCursor::setPos((int)p_x, (int)p_y);
}

void InputHandler::HideMouse(bool p_visible)
{
    if(p_visible)
        QApplication::setOverrideCursor(Qt::BlankCursor);
    else
        QApplication::restoreOverrideCursor();
}

const std::unordered_map<E_ChaosKeyCode, Qt::Key> InputHandler::s_keyCodeMap =
{
    {E_ChaosKeyCode::CHAOS_KEY_UNKNOWN,         Qt::Key::Key_unknown},
    {E_ChaosKeyCode::CHAOS_KEY_SPACE,           Qt::Key::Key_Space},
    {E_ChaosKeyCode::CHAOS_KEY_APOSTROPHE,      Qt::Key::Key_Apostrophe},
    {E_ChaosKeyCode::CHAOS_KEY_COMMA,           Qt::Key::Key_Comma},
    {E_ChaosKeyCode::CHAOS_KEY_MINUS,           Qt::Key::Key_Minus},
    {E_ChaosKeyCode::CHAOS_KEY_PERIOD,          Qt::Key::Key_Period},
    {E_ChaosKeyCode::CHAOS_KEY_SLASH,           Qt::Key::Key_Slash},

    {E_ChaosKeyCode::CHAOS_KEY_0,               Qt::Key::Key_0},
    {E_ChaosKeyCode::CHAOS_KEY_1,               Qt::Key::Key_1},
    {E_ChaosKeyCode::CHAOS_KEY_2,               Qt::Key::Key_2},
    {E_ChaosKeyCode::CHAOS_KEY_3,               Qt::Key::Key_3},
    {E_ChaosKeyCode::CHAOS_KEY_4,               Qt::Key::Key_4},
    {E_ChaosKeyCode::CHAOS_KEY_5,               Qt::Key::Key_5},
    {E_ChaosKeyCode::CHAOS_KEY_6,               Qt::Key::Key_6},
    {E_ChaosKeyCode::CHAOS_KEY_7,               Qt::Key::Key_7},
    {E_ChaosKeyCode::CHAOS_KEY_8,               Qt::Key::Key_8},
    {E_ChaosKeyCode::CHAOS_KEY_9,               Qt::Key::Key_9},

    {E_ChaosKeyCode::CHAOS_KEY_SEMICOLON,       Qt::Key::Key_Semicolon},
    {E_ChaosKeyCode::CHAOS_KEY_EQUAL,           Qt::Key::Key_Equal},
    {E_ChaosKeyCode::CHAOS_KEY_A,               Qt::Key::Key_A},
    {E_ChaosKeyCode::CHAOS_KEY_B,               Qt::Key::Key_B},
    {E_ChaosKeyCode::CHAOS_KEY_C,               Qt::Key::Key_C},
    {E_ChaosKeyCode::CHAOS_KEY_D,               Qt::Key::Key_D},
    {E_ChaosKeyCode::CHAOS_KEY_E,               Qt::Key::Key_E},
    {E_ChaosKeyCode::CHAOS_KEY_F,               Qt::Key::Key_F},
    {E_ChaosKeyCode::CHAOS_KEY_G,               Qt::Key::Key_G},
    {E_ChaosKeyCode::CHAOS_KEY_H,               Qt::Key::Key_H},
    {E_ChaosKeyCode::CHAOS_KEY_I,               Qt::Key::Key_I},
    {E_ChaosKeyCode::CHAOS_KEY_J,               Qt::Key::Key_J},
    {E_ChaosKeyCode::CHAOS_KEY_K,               Qt::Key::Key_K},
    {E_ChaosKeyCode::CHAOS_KEY_L,               Qt::Key::Key_L},
    {E_ChaosKeyCode::CHAOS_KEY_M,               Qt::Key::Key_M},
    {E_ChaosKeyCode::CHAOS_KEY_N,               Qt::Key::Key_N},
    {E_ChaosKeyCode::CHAOS_KEY_O,               Qt::Key::Key_O},
    {E_ChaosKeyCode::CHAOS_KEY_P,               Qt::Key::Key_P},
    {E_ChaosKeyCode::CHAOS_KEY_Q,               Qt::Key::Key_Q},
    {E_ChaosKeyCode::CHAOS_KEY_R,               Qt::Key::Key_R},
    {E_ChaosKeyCode::CHAOS_KEY_S,               Qt::Key::Key_S},
    {E_ChaosKeyCode::CHAOS_KEY_T,               Qt::Key::Key_T},
    {E_ChaosKeyCode::CHAOS_KEY_U,               Qt::Key::Key_U},
    {E_ChaosKeyCode::CHAOS_KEY_V,               Qt::Key::Key_V},
    {E_ChaosKeyCode::CHAOS_KEY_W,               Qt::Key::Key_W},
    {E_ChaosKeyCode::CHAOS_KEY_X,               Qt::Key::Key_X},
    {E_ChaosKeyCode::CHAOS_KEY_Y,               Qt::Key::Key_Y},
    {E_ChaosKeyCode::CHAOS_KEY_Z,               Qt::Key::Key_Z},
    {E_ChaosKeyCode::CHAOS_KEY_LEFT_BRACKET,    Qt::Key::Key_BracketLeft},
    {E_ChaosKeyCode::CHAOS_KEY_BACKSLASH,       Qt::Key::Key_Backslash},
    {E_ChaosKeyCode::CHAOS_KEY_RIGHT_BRACKET,   Qt::Key::Key_BraceRight},
    {E_ChaosKeyCode::CHAOS_KEY_GRAVE_ACCENT,    Qt::Key::Key_Agrave},

    {E_ChaosKeyCode::CHAOS_KEY_ESCAPE,          Qt::Key::Key_Escape},
    {E_ChaosKeyCode::CHAOS_KEY_ENTER,           Qt::Key::Key_Enter},
    {E_ChaosKeyCode::CHAOS_KEY_TAB,             Qt::Key::Key_Tab},
    {E_ChaosKeyCode::CHAOS_KEY_BACKSPACE,       Qt::Key::Key_Backspace},
    {E_ChaosKeyCode::CHAOS_KEY_INSERT,          Qt::Key::Key_Insert},
    {E_ChaosKeyCode::CHAOS_KEY_DELETE,          Qt::Key::Key_Delete},
    {E_ChaosKeyCode::CHAOS_KEY_RIGHT,           Qt::Key::Key_Right},
    {E_ChaosKeyCode::CHAOS_KEY_LEFT,            Qt::Key::Key_Left},
    {E_ChaosKeyCode::CHAOS_KEY_DOWN,            Qt::Key::Key_Down},
    {E_ChaosKeyCode::CHAOS_KEY_UP,              Qt::Key::Key_Up},
    {E_ChaosKeyCode::CHAOS_KEY_PAGE_UP,         Qt::Key::Key_PageUp},
    {E_ChaosKeyCode::CHAOS_KEY_PAGE_DOWN,       Qt::Key::Key_PageDown},
    {E_ChaosKeyCode::CHAOS_KEY_HOME,            Qt::Key::Key_Home},
    {E_ChaosKeyCode::CHAOS_KEY_END,             Qt::Key::Key_End},
    {E_ChaosKeyCode::CHAOS_KEY_CAPS_LOCK,       Qt::Key::Key_CapsLock},
    {E_ChaosKeyCode::CHAOS_KEY_SCROLL_LOCK,     Qt::Key::Key_ScrollLock},
    {E_ChaosKeyCode::CHAOS_KEY_NUM_LOCK,        Qt::Key::Key_NumLock},
    {E_ChaosKeyCode::CHAOS_KEY_PAUSE,           Qt::Key::Key_Pause},
    {E_ChaosKeyCode::CHAOS_KEY_F1,              Qt::Key::Key_F1},
    {E_ChaosKeyCode::CHAOS_KEY_F2,              Qt::Key::Key_F2},
    {E_ChaosKeyCode::CHAOS_KEY_F3,              Qt::Key::Key_F3},
    {E_ChaosKeyCode::CHAOS_KEY_F4,              Qt::Key::Key_F4},
    {E_ChaosKeyCode::CHAOS_KEY_F5,              Qt::Key::Key_F5},
    {E_ChaosKeyCode::CHAOS_KEY_F6,              Qt::Key::Key_F6},
    {E_ChaosKeyCode::CHAOS_KEY_F7,              Qt::Key::Key_F7},
    {E_ChaosKeyCode::CHAOS_KEY_F8,              Qt::Key::Key_F8},
    {E_ChaosKeyCode::CHAOS_KEY_F9,              Qt::Key::Key_F9},
    {E_ChaosKeyCode::CHAOS_KEY_F10,             Qt::Key::Key_F10},
    {E_ChaosKeyCode::CHAOS_KEY_F11,             Qt::Key::Key_F11},
    {E_ChaosKeyCode::CHAOS_KEY_F12,             Qt::Key::Key_F12},
    {E_ChaosKeyCode::CHAOS_KEY_F13,             Qt::Key::Key_F13},
    {E_ChaosKeyCode::CHAOS_KEY_F14,             Qt::Key::Key_F14},
    {E_ChaosKeyCode::CHAOS_KEY_F15,             Qt::Key::Key_F15},
    {E_ChaosKeyCode::CHAOS_KEY_F16,             Qt::Key::Key_F16},
    {E_ChaosKeyCode::CHAOS_KEY_F17,             Qt::Key::Key_F17},
    {E_ChaosKeyCode::CHAOS_KEY_F18,             Qt::Key::Key_F18},
    {E_ChaosKeyCode::CHAOS_KEY_F19,             Qt::Key::Key_F19},
    {E_ChaosKeyCode::CHAOS_KEY_F20,             Qt::Key::Key_F20},
    {E_ChaosKeyCode::CHAOS_KEY_F21,             Qt::Key::Key_F21},
    {E_ChaosKeyCode::CHAOS_KEY_F22,             Qt::Key::Key_F22},
    {E_ChaosKeyCode::CHAOS_KEY_F23,             Qt::Key::Key_F23},
    {E_ChaosKeyCode::CHAOS_KEY_F24,             Qt::Key::Key_F24},
    {E_ChaosKeyCode::CHAOS_KEY_F25,             Qt::Key::Key_F25},

    {E_ChaosKeyCode::CHAOS_KEY_LEFT_SHIFT,      Qt::Key::Key_Shift},
    {E_ChaosKeyCode::CHAOS_KEY_LEFT_CONTROL,    Qt::Key::Key_Control},
    {E_ChaosKeyCode::CHAOS_KEY_LEFT_ALT,        Qt::Key::Key_Alt},
    {E_ChaosKeyCode::CHAOS_KEY_LEFT_SUPER,      Qt::Key::Key_Super_L},
    {E_ChaosKeyCode::CHAOS_KEY_RIGHT_SHIFT,     Qt::Key::Key_Shift },
    {E_ChaosKeyCode::CHAOS_KEY_RIGHT_CONTROL,   Qt::Key::Key_Control},
    {E_ChaosKeyCode::CHAOS_KEY_RIGHT_ALT,       Qt::Key::Key_AltGr},
    {E_ChaosKeyCode::CHAOS_KEY_RIGHT_SUPER,     Qt::Key::Key_Super_R},
    {E_ChaosKeyCode::CHAOS_KEY_MENU,            Qt::Key::Key_Menu},

};

const std::unordered_map<E_ChaosMouseCode, Qt::MouseButton> InputHandler::s_mouseCodeMap =
{
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_1,        Qt::MouseButton::LeftButton},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_2,        Qt::MouseButton::RightButton},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_3,        Qt::MouseButton::MiddleButton},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_4,        Qt::MouseButton::ExtraButton1},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_5,        Qt::MouseButton::ExtraButton2},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_6,        Qt::MouseButton::ExtraButton3},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_7,        Qt::MouseButton::ExtraButton4},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_8,        Qt::MouseButton::ExtraButton5},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_LAST,     Qt::MouseButton::ExtraButton5},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_LEFT,     Qt::MouseButton::LeftButton},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_RIGHT,    Qt::MouseButton::RightButton},
    {E_ChaosMouseCode::CHAOS_MOUSE_BUTTON_MIDDLE,   Qt::MouseButton::MiddleButton}
};
