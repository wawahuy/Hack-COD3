#include "KeyboardHook.h"

KeyboardHook* KeyboardHook::m_instance;


KeyboardHook::KeyboardHook()
{
}


KeyboardHook* KeyboardHook::getInstance()
{
    if (!m_instance) {
        m_instance = new KeyboardHook();
    }
    return m_instance;
}

void KeyboardHook::async()
{
    if (!m_thread) {
        this->m_thread = CreateThread(0, 0, StaticThreadStart, (void*)this, 0, &this->m_threadId);
    }
}

void KeyboardHook::release()
{
    CloseHandle(m_thread);
}

void KeyboardHook::listen(std::function<void(DWORD, DWORD)> func)
{
    this->m_callbacks.push_back(func);
}

LRESULT KeyboardHook::keyboard_hook(const int code, const WPARAM wParam, const LPARAM lParam)
{
    if (wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
        DWORD wVirtKey = kbdStruct->vkCode;
        DWORD wScanCode = kbdStruct->scanCode;
        for (auto cb : m_instance->m_callbacks) {
            cb(wVirtKey, wScanCode);
        }
    }
    return CallNextHookEx(m_instance->m_hHook, code, wParam, lParam);
}

DWORD __stdcall KeyboardHook::StaticThreadStart(void* param)
{
    KeyboardHook* o = (KeyboardHook*)param;
    o->m_hHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        KeyboardHook::keyboard_hook,
        NULL,
        0
    );
    if (o->m_hHook == NULL) {
        std::cout << "Keyboard hook failed!" << std::endl;
    }
    std::cout << "start thread keyboard" << std::endl;
    while (GetMessageW(NULL, NULL, 0, 0));
    return 0;
}