#pragma once

enum class E_ChaosKeyCode : int16_t
{
	CHAOS_KEY_UNKNOWN            = -1,						
								   
	CHAOS_KEY_SPACE              = 32,
	CHAOS_KEY_APOSTROPHE         = 39,  /* ' */
	CHAOS_KEY_COMMA              = 44,  /* , */
	CHAOS_KEY_MINUS              = 45,  /* - */
	CHAOS_KEY_PERIOD             = 46,  /* . */
	CHAOS_KEY_SLASH              = 47,  /* / */
	CHAOS_KEY_0                  = 48,
	CHAOS_KEY_1                  = 49,
	CHAOS_KEY_2                  = 50,
	CHAOS_KEY_3                  = 51,
	CHAOS_KEY_4                  = 52,
	CHAOS_KEY_5                  = 53,
	CHAOS_KEY_6                  = 54,
	CHAOS_KEY_7                  = 55,
	CHAOS_KEY_8                  = 56,
	CHAOS_KEY_9                  = 57,
	CHAOS_KEY_SEMICOLON          = 59,  /* ; */
	CHAOS_KEY_EQUAL              = 61,  /* = */
	CHAOS_KEY_A                  = 65,
	CHAOS_KEY_B                  = 66,
	CHAOS_KEY_C                  = 67,
	CHAOS_KEY_D                  = 68,
	CHAOS_KEY_E                  = 69,
	CHAOS_KEY_F                  = 70,
	CHAOS_KEY_G                  = 71,
	CHAOS_KEY_H                  = 72,
	CHAOS_KEY_I                  = 73,
	CHAOS_KEY_J                  = 74,
	CHAOS_KEY_K                  = 75,
	CHAOS_KEY_L                  = 76,
	CHAOS_KEY_M                  = 77,
	CHAOS_KEY_N                  = 78,
	CHAOS_KEY_O                  = 79,
	CHAOS_KEY_P                  = 80,
	CHAOS_KEY_Q                  = 81,
	CHAOS_KEY_R                  = 82,
	CHAOS_KEY_S                  = 83,
	CHAOS_KEY_T                  = 84,
	CHAOS_KEY_U                  = 85,
	CHAOS_KEY_V                  = 86,
	CHAOS_KEY_W                  = 87,
	CHAOS_KEY_X                  = 88,
	CHAOS_KEY_Y                  = 89,
	CHAOS_KEY_Z                  = 90,
	CHAOS_KEY_LEFT_BRACKET       = 91,  /* [ */
	CHAOS_KEY_BACKSLASH          = 92,  /* \ */
	CHAOS_KEY_RIGHT_BRACKET      = 93,  /* ] */
	CHAOS_KEY_GRAVE_ACCENT       = 96,  /* ` */
	
	CHAOS_KEY_ESCAPE             = 256,
	CHAOS_KEY_ENTER              = 257,
	CHAOS_KEY_TAB                = 258,
	CHAOS_KEY_BACKSPACE          = 259,
	CHAOS_KEY_INSERT             = 260,
	CHAOS_KEY_DELETE             = 261,
	CHAOS_KEY_RIGHT              = 262,
	CHAOS_KEY_LEFT               = 263,
	CHAOS_KEY_DOWN               = 264,
	CHAOS_KEY_UP                 = 265,
	CHAOS_KEY_PAGE_UP            = 266,
	CHAOS_KEY_PAGE_DOWN          = 267,
	CHAOS_KEY_HOME               = 268,
	CHAOS_KEY_END                = 269,
	CHAOS_KEY_CAPS_LOCK          = 280,
	CHAOS_KEY_SCROLL_LOCK        = 281,
	CHAOS_KEY_NUM_LOCK           = 282,
	CHAOS_KEY_PRINT_SCREEN       = 283,
	CHAOS_KEY_PAUSE              = 284,
	CHAOS_KEY_F1                 = 290,
	CHAOS_KEY_F2                 = 291,
	CHAOS_KEY_F3                 = 292,
	CHAOS_KEY_F4                 = 293,
	CHAOS_KEY_F5                 = 294,
	CHAOS_KEY_F6                 = 295,
	CHAOS_KEY_F7                 = 296,
	CHAOS_KEY_F8                 = 297,
	CHAOS_KEY_F9                 = 298,
	CHAOS_KEY_F10                = 299,
	CHAOS_KEY_F11                = 300,
	CHAOS_KEY_F12                = 301,
	CHAOS_KEY_F13                = 302,
	CHAOS_KEY_F14                = 303,
	CHAOS_KEY_F15                = 304,
	CHAOS_KEY_F16                = 305,
	CHAOS_KEY_F17                = 306,
	CHAOS_KEY_F18                = 307,
	CHAOS_KEY_F19                = 308,
	CHAOS_KEY_F20                = 309,
	CHAOS_KEY_F21                = 310,
	CHAOS_KEY_F22                = 311,
	CHAOS_KEY_F23                = 312,
	CHAOS_KEY_F24                = 313,
	CHAOS_KEY_F25                = 314,
	CHAOS_KEY_KP_0               = 320,
	CHAOS_KEY_KP_1               = 321,
	CHAOS_KEY_KP_2               = 322,
	CHAOS_KEY_KP_3               = 323,
	CHAOS_KEY_KP_4               = 324,
	CHAOS_KEY_KP_5               = 325,
	CHAOS_KEY_KP_6               = 326,
	CHAOS_KEY_KP_7               = 327,
	CHAOS_KEY_KP_8               = 328,
	CHAOS_KEY_KP_9               = 329,
	CHAOS_KEY_KP_DECIMAL         = 330,
	CHAOS_KEY_KP_DIVIDE          = 331,
	CHAOS_KEY_KP_MULTIPLY        = 332,
	CHAOS_KEY_KP_SUBTRACT        = 333,
	CHAOS_KEY_KP_ADD             = 334,
	CHAOS_KEY_KP_ENTER           = 335,
	CHAOS_KEY_KP_EQUAL           = 336,
	CHAOS_KEY_LEFT_SHIFT         = 340,
	CHAOS_KEY_LEFT_CONTROL       = 341,
	CHAOS_KEY_LEFT_ALT           = 342,
	CHAOS_KEY_LEFT_SUPER         = 343,
	CHAOS_KEY_RIGHT_SHIFT        = 344,
	CHAOS_KEY_RIGHT_CONTROL      = 345,
	CHAOS_KEY_RIGHT_ALT          = 346,
	CHAOS_KEY_RIGHT_SUPER        = 347,
	CHAOS_KEY_MENU               = 348,
	
	CHAOS_KEY_LAST               = CHAOS_KEY_MENU
};

enum class E_ChaosKeyModFlag : uint8_t
{
		CHAOS_MOD_SHIFT          = 0x0001,
								   
		CHAOS_MOD_CONTROL        = 0x0002,
								   
		CHAOS_MOD_ALT            = 0x0004,
								   
		CHAOS_MOD_SUPER          = 0x0008
};

enum class E_ChaosMouseCode : uint8_t
{
	CHAOS_MOUSE_BUTTON_1         = 0,
	CHAOS_MOUSE_BUTTON_2         = 1,
	CHAOS_MOUSE_BUTTON_3         = 2,
	CHAOS_MOUSE_BUTTON_4         = 3,
	CHAOS_MOUSE_BUTTON_5         = 4,
	CHAOS_MOUSE_BUTTON_6         = 5,
	CHAOS_MOUSE_BUTTON_7         = 6,
	CHAOS_MOUSE_BUTTON_8         = 7,
	CHAOS_MOUSE_BUTTON_LAST      = CHAOS_MOUSE_BUTTON_8,
	CHAOS_MOUSE_BUTTON_LEFT      = CHAOS_MOUSE_BUTTON_1,
	CHAOS_MOUSE_BUTTON_RIGHT     = CHAOS_MOUSE_BUTTON_2,
	CHAOS_MOUSE_BUTTON_MIDDLE    = CHAOS_MOUSE_BUTTON_3
};

enum class E_ChaosJoystickCode : uint8_t
{
	CHAOS_JOYSTICK_1             = 0,
	CHAOS_JOYSTICK_2             = 1,
	CHAOS_JOYSTICK_3             = 2,
	CHAOS_JOYSTICK_4             = 3,
	CHAOS_JOYSTICK_5             = 4,
	CHAOS_JOYSTICK_6             = 5,
	CHAOS_JOYSTICK_7             = 6,
	CHAOS_JOYSTICK_8             = 7,
	CHAOS_JOYSTICK_9             = 8,
	CHAOS_JOYSTICK_10            = 9,
	CHAOS_JOYSTICK_11            = 10,
	CHAOS_JOYSTICK_12            = 11,
	CHAOS_JOYSTICK_13            = 12,
	CHAOS_JOYSTICK_14            = 13,
	CHAOS_JOYSTICK_15            = 14,
	CHAOS_JOYSTICK_16            = 15,
	CHAOS_JOYSTICK_LAST          = CHAOS_JOYSTICK_16
};