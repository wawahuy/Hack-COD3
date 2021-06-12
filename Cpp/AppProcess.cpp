#include "AppProcess.h"

AppProcess::AppProcess()
{
}

AppProcess* AppProcess::Create(LPCWSTR name)
{
    AppProcess* proccess = new AppProcess();
	proccess->m_hwnd = FindWindow(NULL, name);
	GetWindowThreadProcessId(proccess->m_hwnd, &proccess->m_pid);
	proccess->m_pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proccess->m_pid);
	return proccess;
}

uintptr_t AppProcess::getBaseAddress()
{
    if (this->m_pHandle == NULL)
        return NULL; 

    HMODULE lphModule[1024]; // Array that receives the list of module handles
    DWORD lpcbNeeded(NULL); // Output of EnumProcessModules, giving the number of bytes requires to store all modules handles in the lphModule array

    if (!EnumProcessModules(this->m_pHandle, lphModule, sizeof(lphModule), &lpcbNeeded))
        return NULL; // Impossible to read modules

    TCHAR szModName[MAX_PATH];
    if (!GetModuleFileNameEx(this->m_pHandle, lphModule[0], szModName, sizeof(szModName) / sizeof(TCHAR)))
        return NULL; // Impossible to get module info

    return (DWORD)lphModule[0]; // Module 0 is apparently always the EXE itself, returning its address
}

uintptr_t AppProcess::getModuleBaseAddress(TCHAR* moduleName)
{
    DWORD moduleBaseAddress = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->m_pid);
    MODULEENTRY32 modEntry;
    modEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnap, &modEntry))
    {
        do {
            if (_tcscmp(modEntry.szModule, moduleName) == 0)
            {
                moduleBaseAddress = (DWORD)modEntry.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnap, &modEntry));
    }
    CloseHandle(hSnap);
    return moduleBaseAddress;
    return 0;
}

glm::vec2 AppProcess::getWindowPosition()
{
    RECT rect;
    GetWindowRect(m_hwnd, &rect);
    return glm::vec2(rect.left, rect.top);
}

glm::vec2 AppProcess::getWindowSize()
{
    RECT rect;
    GetWindowRect(m_hwnd, &rect);
    return glm::vec2(rect.right - rect.left, rect.bottom - rect.top);
}

bool AppProcess::isOpen()
{
    return m_pHandle != NULL;
}
