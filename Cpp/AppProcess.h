#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>

class AppProcess
{
public:
	static AppProcess* Create(LPCWSTR name);
	DWORD getBaseAddress();
	DWORD getModuleBaseAddress(TCHAR* moduleName);

	template<class T>
	BOOL  readProcessMemory(DWORD address, T& varOut);

	template<class T>
	BOOL  writeProcessMemory(DWORD address, T& varIn);

private:
	AppProcess();
	HANDLE m_pHandle;
	DWORD m_pid;
};

template<class T>
BOOL AppProcess::readProcessMemory(DWORD address, T& varOut)
{
	return ReadProcessMemory(this->m_pHandle, (LPVOID)address, &varOut, sizeof(varOut), NULL);
}

template<class T>
inline BOOL AppProcess::writeProcessMemory(DWORD address, T& varIn)
{
	return WriteProcessMemory(this->m_pHandle, (LPVOID)(address), &varIn, sizeof(varIn), 0);
}
