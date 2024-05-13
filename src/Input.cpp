#include "stdafx.h"

#include "Input.h"

void Input::Update()
{
	XINPUT_STATE state;
	if (FAILED(HRESULT_FROM_WIN32(XInputGetState(0, &state))))
		state = { 0 };

	for (auto& button : m_state)
	{
		button.pressed_previous = button.pressed;

		button.pressed = (GetAsyncKeyState(button.virtual_key) & 0x8000) != 0;
		if (button.pressed)
			continue;

		button.pressed = (GetAsyncKeyState(button.alt_virtual_key) & 0x8000) != 0;
		if (button.pressed)
			continue;

		button.pressed = (state.Gamepad.wButtons & button.xinput_button) != 0;
	}
}

bool Input::WasPressed(Button b)
{
	for (auto& button : m_state)
	{
		if (button.button == b)
		{
			return button.pressed && !button.pressed_previous;
		}
	}

	return false;
}
