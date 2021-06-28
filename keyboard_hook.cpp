// keyboard_hook.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iostream>
#include <windows.h>
#include <WinUser.h>


void handle_key_event(DWORD keycode);
HHOOK hook_handle = nullptr;
HWND temp_handle = NULL;  //variable for saving handle to spotify 

LRESULT CALLBACK LowLevelKeyboardProc(int hook_proc_code, WPARAM event_type, LPARAM event_ptr_param) {
	if (hook_proc_code == HC_ACTION) {
		auto event_ptr = reinterpret_cast<KBDLLHOOKSTRUCT*>(event_ptr_param);
		DWORD keycode = event_ptr->vkCode;

		BOOL wasDown = FALSE;
		if (keycode == VK_RCONTROL)
			switch (event_type)
			{
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				if (!wasDown) {
					handle_key_event(keycode);
					wasDown = true;
				}
				break;
			case WM_KEYUP:
			case WM_SYSKEYUP:
				wasDown = FALSE;
				break;
			}
	}

	return CallNextHookEx(hook_handle, hook_proc_code, event_type, event_ptr_param);
}

void handle_key_event(DWORD keycode)
{

	HWND ip = FindWindowW(NULL, L"Spotify premium");

	if (ip == NULL)
		ip = temp_handle; //if spotify changed window name - use saved handler

	if (ip == NULL) //if saved handler is empty - spotify is not started or need to be paused first
		std::cout << "spotify is not started or playing now, start it or press pause" << std::endl;

	INPUT inputs[2] = {};
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_MEDIA_PLAY_PAUSE;
	inputs[0].ki.dwFlags = 0; //keydown

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_MEDIA_PLAY_PAUSE;
	inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;  //keyup

	if (SetForegroundWindow(ip))
		UINT uSent = SendInput(1, inputs, sizeof(INPUT));

	temp_handle = ip; //save spotify handler bacause spotify window name will change to new based on played song
}


int main()
{
	hook_handle = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);

	if (hook_handle == nullptr) {
		std::cout << "failed to register keyboard hook" << std::endl;
		return 1;
	}

	std::cout << "Media button is active!" << std::endl;

	MSG messages;

	while (GetMessage(&messages, nullptr, 0, 0)) {
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	UnhookWindowsHookEx(hook_handle);
	std::cout << "Media button stopped" << std::endl;
	return 0;
}