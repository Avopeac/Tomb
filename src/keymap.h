#pragma once

#include "SDL.h"

namespace input
{
	enum class Keystate : Uint8
	{
		Up = 0,
		Down,
	};

	enum class Key : Uint16
	{
		KeyModLeftAlt = 0, KeyModRightAlt, KeyModLeftCtrl, KeyModRightCtrl, KeyModLeftShift,
		KeyModRightShift, KeyTab, KeyModCapsLock, KeyHome, KeyEnd, KeyPgUp, KeyPgDown,
		KeySpace, KeyEnter, Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9, 
		KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ,	KeyK, KeyL,	KeyM, KeyN,	KeyO,
		KeyP, KeyQ, KeyR, KeyS,	KeyT, KeyU, KeyV, KeyW,	KeyX, KeyY, KeyZ, KeyLeftBracket, KeyDelete,
		KeyUnknown, KeyCount = KeyUnknown
	};

	class Keymap
	{

		Keystate previous_[(Uint16)Key::KeyCount]{};
		Keystate current_[(Uint16)Key::KeyCount]{};

	public:

		static Keymap &Get()
		{
			static Keymap instance;
			return instance;
		}

		void UpdateCurrentInput(SDL_Event &e);

		void CarryCurrentInput();

		bool KeyDown(Key key);

		bool KeyUp(Key key);

		bool KeyPressed(Key key);

	private:

		void UpdateKey(SDL_Keysym &keysym, Keystate state);

	};
}
