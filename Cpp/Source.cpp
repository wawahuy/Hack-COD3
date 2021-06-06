#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>

using namespace std;


DWORD GetBaseAddress(const HANDLE hProcess) {
    if (hProcess == NULL)
        return NULL; // No access to the process

    HMODULE lphModule[1024]; // Array that receives the list of module handles
    DWORD lpcbNeeded(NULL); // Output of EnumProcessModules, giving the number of bytes requires to store all modules handles in the lphModule array

    if (!EnumProcessModules(hProcess, lphModule, sizeof(lphModule), &lpcbNeeded))
        return NULL; // Impossible to read modules

    TCHAR szModName[MAX_PATH];
    if (!GetModuleFileNameEx(hProcess, lphModule[0], szModName, sizeof(szModName) / sizeof(TCHAR)))
        return NULL; // Impossible to get module info

    return (DWORD)lphModule[0]; // Module 0 is apparently always the EXE itself, returning its address
}

// Hàm lấy địa chỉ của module "hw.dll"
DWORD getModuleBaseAddress(TCHAR* moduleName, DWORD pid) {
    DWORD moduleBaseAddress = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    MODULEENTRY32 modEntry;
    modEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnap, &modEntry))
    {
        do {
            cout << modEntry.szModule << endl;
            if (_tcscmp(modEntry.szModule, moduleName) == 0)
            {
                moduleBaseAddress = (DWORD)modEntry.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnap, &modEntry));


    }
    CloseHandle(hSnap);
    return moduleBaseAddress;
}

int main() {
    // Lấy process id thay vì phải làm thủ công như lần trước Call of Duty®: Modern Warfare®
    DWORD pid;
    HWND hGameWindow = FindWindow(NULL, _T("Call of Duty®: Modern Warfare® 3"));
    GetWindowThreadProcessId(hGameWindow, &pid);
    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    cout << "Proc handle:" << pHandle << endl;


    //// Tính địa chỉ của biến máu
    DWORD baseAddress;
    //char moduleName[] = "iw5sp.exe";
    //DWORD moduleAddress = getModuleBaseAddress((TCHAR*)moduleName, pid);
    DWORD moduleAddress = GetBaseAddress(pHandle);
    cout << "Module address:" << moduleAddress << endl;

    // 0x00809820 chính là offset tìm được bằng Cheat Engine
    if (!ReadProcessMemory(pHandle, (LPVOID)(moduleAddress + 0x0003A784), &baseAddress, sizeof(baseAddress), NULL)) {
        cout << "Couldn't read process memory";
        return -1;
    }
    //// 0x504 chính là offset tìm được bằng Cheat Engine
    DWORD healthAddress = baseAddress + 0x358;
    DWORD noReloadBullet = baseAddress + 0x3D0;

    cout << "Static address:" << (moduleAddress + 0x0003A784) << endl;
    cout << "Base address:" << baseAddress << endl;
    cout << "Health address: " << healthAddress << endl;
    cout << "noReloadBullet address: " << noReloadBullet << endl;

    int newHealth = 2000;
    int newReBullet = 200;
    while (true) {
        if (!WriteProcessMemory(pHandle, (LPVOID)(healthAddress), &newHealth, sizeof(newHealth), 0)) {
            cout << "Couldn't write process memory" << endl;
        }
        if (!WriteProcessMemory(pHandle, (LPVOID)(noReloadBullet), &newReBullet, sizeof(newReBullet), 0)) {
            cout << "Couldn't write process memory" << endl;
        }
        Sleep(1000);
    }
}