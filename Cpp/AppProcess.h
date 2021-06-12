#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#include <glm/vec2.hpp>

class AppProcess
{
public:
	static AppProcess* Create(LPCWSTR name);
	uintptr_t getBaseAddress();
	uintptr_t getModuleBaseAddress(TCHAR* moduleName);
	glm::vec2 getWindowPosition();
	glm::vec2 getWindowSize();
	
	template<class T>
	BOOL  readProcessMemory(uintptr_t address, T& varOut, unsigned int size, unsigned int* numberOfByteRead);

	template<class T>
	BOOL  readProcessMemory(uintptr_t address, T& varOut);

	template<class T>
	BOOL  writeProcessMemory(uintptr_t address, T& varIn);

private:
	AppProcess();
	HWND m_hwnd;
	HANDLE m_pHandle;
	DWORD m_pid;
};

template<class T>
inline BOOL AppProcess::readProcessMemory(uintptr_t address, T& varOut, unsigned int size, unsigned int* numberOfByteRead)
{
	return ReadProcessMemory(this->m_pHandle, (LPVOID)address, &varOut, size, numberOfByteRead);
}

template<class T>
BOOL AppProcess::readProcessMemory(uintptr_t address, T& varOut)
{
	return ReadProcessMemory(this->m_pHandle, (LPVOID)address, &varOut, sizeof(varOut), NULL);
}

template<class T>
inline BOOL AppProcess::writeProcessMemory(uintptr_t address, T& varIn)
{
	return WriteProcessMemory(this->m_pHandle, (LPVOID)(address), &varIn, sizeof(varIn), NULL);
}
