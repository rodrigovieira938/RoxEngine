#pragma once
#include "glm/glm.hpp"
#include <cstdint>
#include <variant>

namespace RoxEngine {
    using KeyCode = uint16_t;

	namespace Key
	{
		enum : KeyCode {
		// From glfw3.h
			Space               = 32,
			Apostrophe          = 39, /* ' */
			Comma               = 44, /* , */
			Minus               = 45, /* - */
			Period              = 46, /* . */
			Slash               = 47, /* / */

			D0                  = 48, /* 0 */
			D1                  = 49, /* 1 */
			D2                  = 50, /* 2 */
			D3                  = 51, /* 3 */
			D4                  = 52, /* 4 */
			D5                  = 53, /* 5 */
			D6                  = 54, /* 6 */
			D7                  = 55, /* 7 */
			D8                  = 56, /* 8 */
			D9                  = 57, /* 9 */

			Semicolon           = 59, /* ; */
			Equal               = 61, /* = */

			A                   = 65,
			B                   = 66,
			C                   = 67,
			D                   = 68,
			E                   = 69,
			F                   = 70,
			G                   = 71,
			H                   = 72,
			I                   = 73,
			J                   = 74,
			K                   = 75,
			L                   = 76,
			M                   = 77,
			N                   = 78,
			O                   = 79,
			P                   = 80,
			Q                   = 81,
			R                   = 82,
			S                   = 83,
			T                   = 84,
			U                   = 85,
			V                   = 86,
			W                   = 87,
			X                   = 88,
			Y                   = 89,
			Z                   = 90,

			LeftBracket         = 91,  /* [ */
			Backslash           = 92,  /* \ */
			RightBracket        = 93,  /* ] */
			GraveAccent         = 96,  /* ` */

			World1              = 161, /* non-US #1 */
			World2              = 162, /* non-US #2 */

			/* Function keys */
			Escape              = 256,
			Enter               = 257,
			Tab                 = 258,
			Backspace           = 259,
			Insert              = 260,
			Delete              = 261,
			Right               = 262,
			Left                = 263,
			Down                = 264,
			Up                  = 265,
			PageUp              = 266,
			PageDown            = 267,
			Home                = 268,
			End                 = 269,
			CapsLock            = 280,
			ScrollLock          = 281,
			NumLock             = 282,
			PrintScreen         = 283,
			Pause               = 284,
			F1                  = 290,
			F2                  = 291,
			F3                  = 292,
			F4                  = 293,
			F5                  = 294,
			F6                  = 295,
			F7                  = 296,
			F8                  = 297,
			F9                  = 298,
			F10                 = 299,
			F11                 = 300,
			F12                 = 301,
			F13                 = 302,
			F14                 = 303,
			F15                 = 304,
			F16                 = 305,
			F17                 = 306,
			F18                 = 307,
			F19                 = 308,
			F20                 = 309,
			F21                 = 310,
			F22                 = 311,
			F23                 = 312,
			F24                 = 313,
			F25                 = 314,

			/* Keypad */
			KP0                 = 320,
			KP1                 = 321,
			KP2                 = 322,
			KP3                 = 323,
			KP4                 = 324,
			KP5                 = 325,
			KP6                 = 326,
			KP7                 = 327,
			KP8                 = 328,
			KP9                 = 329,
			KPDecimal           = 330,
			KPDivide            = 331,
			KPMultiply          = 332,
			KPSubtract          = 333,
			KPAdd               = 334,
			KPEnter             = 335,
			KPEqual             = 336,

			LeftShift           = 340,
			LeftControl         = 341,
			LeftAlt             = 342,
			LeftSuper           = 343,
			RightShift          = 344,
			RightControl        = 345,
			RightAlt            = 346,
			RightSuper          = 347,
			Menu                = 348,
            MAX = Menu,
		};
	}
    enum class KeyState {
        NONE,
        PRESSED,
        REPEAT,
        RELEASED,
    };
	enum class MouseState {
		NORMAL = 0,
		DISABLED,
		HIDDEN,
	};
	enum class MouseButton {
		BUTTON_1 = 0,
		BUTTON_2 = 1,
		BUTTON_3 = 2,
		BUTTON_4 = 3,
		BUTTON_5 = 4,
		BUTTON_6 = 5,
		BUTTON_7 = 6,
		BUTTON_8 = 7,

		LEFT   = MouseButton::BUTTON_1,
		RIGHT  = MouseButton::BUTTON_2,
		MIDDLE = MouseButton::BUTTON_3,
		MAX    = MouseButton::BUTTON_8,
	};
    class Input {
    public:
        Input() = delete;
        static KeyState GetKeyState(KeyCode code);
		// Returns the time the key press time in ms
		static double GetKeyPressDuration(KeyCode code);
        // whether key is pressed or repeating
        inline static bool IsKeyDown(KeyCode code)          {auto state = GetKeyState(code); return state == KeyState::PRESSED || state == KeyState::REPEAT;}
        // whether key is none or release
        inline static bool IsKeyUp(KeyCode code)            {auto state = GetKeyState(code); return state == KeyState::NONE    || state == KeyState::RELEASED;}
        //if the key is pressed
        inline static bool IsKeyPressed(KeyCode code)       {auto state = GetKeyState(code); return state == KeyState::PRESSED;}
        //if the key is repeating
        inline static bool IsKeyRepeat(KeyCode code)        {auto state = GetKeyState(code); return state == KeyState::REPEAT;}
        //if the key is releasing
        inline static bool IsKeyReleased(KeyCode code)      {auto state = GetKeyState(code); return state == KeyState::RELEASED;}
        //TODO: GetLastKeyPressed and GetKeysPressed - Maybe used for keyrebinding, and some other stuff
    
		static void SetMouseState(MouseState state);
		static double GetMouseScroll();
		static bool MousePositionChanged();
		static glm::dvec2 GetMousePosition();

		static KeyState GetMouseButtonState(MouseButton btn);

		// Returns the time the mouse button press time in ms
		static double GetMousePressDuration(MouseButton btn);
        // whether mouse button is pressed or repeating
        inline static bool IsMouseButtonDown(MouseButton btn)          {auto state = GetMouseButtonState(btn); return state == KeyState::PRESSED || state == KeyState::REPEAT;}
        // whether mouse button is none or release
        inline static bool IsMouseButtonUp(MouseButton btn)            {auto state = GetMouseButtonState(btn); return state == KeyState::NONE    || state == KeyState::RELEASED;}
        //if the mouse button is pressed
        inline static bool IsMouseButtonPressed(MouseButton btn)       {auto state = GetMouseButtonState(btn); return state == KeyState::PRESSED;}
        //if the mouse button is repeating
        inline static bool IsMouseButtonRepeat(MouseButton btn)        {auto state = GetMouseButtonState(btn); return state == KeyState::REPEAT;}
        //if the mouse button is releasing
        inline static bool IsMouseButtonReleased(MouseButton btn)      {auto state = GetMouseButtonState(btn); return state == KeyState::RELEASED;}
	
		static std::variant<KeyCode, MouseButton, std::monostate> GetLastKeyPressed();
		static std::variant<KeyCode, MouseButton, std::monostate> GetLastKeyReleased();

		static void ResetLastKeyPressed();
		static void ResetLastKeyReleased();

	private:
        friend class Engine;
        static void Init();
        static void Update();
        static void Shutdown();
    };
}