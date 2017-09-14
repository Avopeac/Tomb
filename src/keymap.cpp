#include "keymap.h"

using namespace input;

void Keymap::UpdateCurrentInput(SDL_Event & e)
{
	switch (e.type)
	{
		case SDL_KEYDOWN: { UpdateKey(e.key.keysym, Keystate::Down); } break;
		case SDL_KEYUP: { UpdateKey(e.key.keysym, Keystate::Up); } break;
		default: break;
	}
}

void Keymap::CarryCurrentInput()
{
	SDL_memcpy(previous_, current_, sizeof(Keystate) * (Uint16)Key::KeyCount);
}

bool Keymap::KeyDown(Key key)
{
	return previous_[(Uint16)key] == Keystate::Up &&
		current_[(Uint16)key] == Keystate::Down;
}

bool Keymap::KeyUp(Key key)
{
	return previous_[(Uint16)key] == Keystate::Down &&
		current_[(Uint16)key] == Keystate::Up;
}

bool Keymap::KeyPressed(Key key)
{
	return previous_[(Uint16)key] == Keystate::Down &&
		current_[(Uint16)key] == Keystate::Down;
}

void Keymap::UpdateKey(SDL_Keysym &keysym, Keystate state)
{
	switch (keysym.mod)
	{
		case SDL_Keymod::KMOD_CAPS: { current_[(Uint16)Key::KeyModCapsLock] = state; } break;
		case SDL_Keymod::KMOD_LALT: { current_[(Uint16)Key::KeyModLeftAlt] = state; }  break;
		case SDL_Keymod::KMOD_RALT: { current_[(Uint16)Key::KeyModRightAlt] = state; }  break;
		case SDL_Keymod::KMOD_LCTRL: { current_[(Uint16)Key::KeyModLeftCtrl] = state; }  break;
		case SDL_Keymod::KMOD_RCTRL: { current_[(Uint16)Key::KeyModRightCtrl] = state; }  break;
		case SDL_Keymod::KMOD_LSHIFT: { current_[(Uint16)Key::KeyModLeftShift] = state; }  break;
		case SDL_Keymod::KMOD_RSHIFT: { current_[(Uint16)Key::KeyModRightShift] = state; }  break;
		default: break;
	}

	switch (keysym.sym)
	{
		case SDLK_0: { current_[(Uint16)Key::Key0] = state; } break;
		case SDLK_1: { current_[(Uint16)Key::Key1] = state; } break;
		case SDLK_2: { current_[(Uint16)Key::Key2] = state; } break;
		case SDLK_3: { current_[(Uint16)Key::Key3] = state; } break;
		case SDLK_4: { current_[(Uint16)Key::Key4] = state; } break;
		case SDLK_5: { current_[(Uint16)Key::Key5] = state; } break;
		case SDLK_6: { current_[(Uint16)Key::Key6] = state; } break;
		case SDLK_7: { current_[(Uint16)Key::Key7] = state; } break;
		case SDLK_8: { current_[(Uint16)Key::Key8] = state; } break;
		case SDLK_9: { current_[(Uint16)Key::Key9] = state; } break;
		case SDLK_a: { current_[(Uint16)Key::KeyA] = state; } break;
		case SDLK_b: { current_[(Uint16)Key::KeyB] = state; } break;
		case SDLK_c: { current_[(Uint16)Key::KeyC] = state; } break;
		case SDLK_d: { current_[(Uint16)Key::KeyD] = state; } break;
		case SDLK_e: { current_[(Uint16)Key::KeyE] = state; } break;
		case SDLK_f: { current_[(Uint16)Key::KeyF] = state; } break;
		case SDLK_g: { current_[(Uint16)Key::KeyG] = state; } break;
		case SDLK_h: { current_[(Uint16)Key::KeyH] = state; } break;
		case SDLK_i: { current_[(Uint16)Key::KeyI] = state; } break;
		case SDLK_j: { current_[(Uint16)Key::KeyJ] = state; } break;
		case SDLK_k: { current_[(Uint16)Key::KeyK] = state; } break;
		case SDLK_l: { current_[(Uint16)Key::KeyL] = state; } break;
		case SDLK_m: { current_[(Uint16)Key::KeyM] = state; } break;
		case SDLK_n: { current_[(Uint16)Key::KeyN] = state; } break;
		case SDLK_o: { current_[(Uint16)Key::KeyO] = state; } break;
		case SDLK_p: { current_[(Uint16)Key::KeyP] = state; } break;
		case SDLK_q: { current_[(Uint16)Key::KeyQ] = state; } break;
		case SDLK_r: { current_[(Uint16)Key::KeyR] = state; } break;
		case SDLK_s: { current_[(Uint16)Key::KeyS] = state; } break;
		case SDLK_t: { current_[(Uint16)Key::KeyT] = state; } break;
		case SDLK_u: { current_[(Uint16)Key::KeyU] = state; } break;
		case SDLK_v: { current_[(Uint16)Key::KeyV] = state; } break;
		case SDLK_w: { current_[(Uint16)Key::KeyW] = state; } break;
		case SDLK_x: { current_[(Uint16)Key::KeyX] = state; } break;
		case SDLK_y: { current_[(Uint16)Key::KeyY] = state; } break;
		case SDLK_z: { current_[(Uint16)Key::KeyZ] = state; } break;
		case SDLK_HOME: { current_[(Uint16)Key::KeyHome] = state; } break;
		case SDLK_END: { current_[(Uint16)Key::KeyEnd] = state; } break;
		case SDLK_PAGEUP: { current_[(Uint16)Key::KeyPgUp] = state; } break;
		case SDLK_PAGEDOWN: { current_[(Uint16)Key::KeyPgDown] = state; } break;
		case SDLK_SPACE: { current_[(Uint16)Key::KeySpace] = state; } break;
		case SDLK_RETURN: { current_[(Uint16)Key::KeyEnter] = state; } break;
		case SDLK_DELETE: { current_[(Uint16)Key::KeyDelete] = state; } break;
		case SDLK_TAB: { current_[(Uint16)Key::KeyTab] = state; } break;
		default: break;
	}
}
