#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xinput.h> // make sure the linker lib uses xinput1_3.dll and not any other
#include <array>

class Input
{
public:
	enum class Button
	{
		Up,
		Down,
		Left,
		Right,
		Enter,
		Exit,
		Special
	};

	void Update();
	bool WasPressed(Button button);

private:
	struct ButtonState
	{
		Button button;
		int virtual_key;
		int alt_virtual_key;
		unsigned short xinput_button;
		bool pressed;
		bool pressed_previous;
	};

	std::array<ButtonState, 7> m_state = {{
		{ Button::Special, VK_F1,     VK_OEM_3, XINPUT_GAMEPAD_LEFT_SHOULDER },
		{ Button::Up,      'W',       VK_UP,    XINPUT_GAMEPAD_DPAD_UP },
		{ Button::Down,    'S',       VK_DOWN,  XINPUT_GAMEPAD_DPAD_DOWN },
		{ Button::Left,    'A',       VK_LEFT,  XINPUT_GAMEPAD_DPAD_LEFT },
		{ Button::Right,   'D',       VK_RIGHT, XINPUT_GAMEPAD_DPAD_RIGHT },
		{ Button::Enter,   VK_RETURN, VK_SPACE, XINPUT_GAMEPAD_A },
		{ Button::Exit,    VK_ESCAPE, VK_BACK,  XINPUT_GAMEPAD_B },
	}};
};
