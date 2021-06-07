#pragma once
#include <Windows.h>
#include <functional>
#include <iostream>
#include <vector>

class KeyboardHook
{
public:
	static KeyboardHook* getInstance();
	void async();
	void release();
	void listen(std::function<void(DWORD, DWORD)>);

private:
	static LRESULT CALLBACK keyboard_hook(const int code, const WPARAM wParam, const LPARAM lParam);
	static DWORD WINAPI StaticThreadStart(void* param);
	static KeyboardHook* m_instance;

	KeyboardHook();
	HHOOK m_hHook { NULL };
	HANDLE m_thread = NULL;
	DWORD m_threadId = 0;
	std::vector<std::function<void(DWORD, DWORD)>> m_callbacks;
};

