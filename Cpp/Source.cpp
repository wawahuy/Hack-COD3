#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#include "AppProcess.h"

using namespace std;

//0x0003A784 
enum EOffsetPlayerData {
    Bullet1 = 0x358,
    BulletReload1 = 0x3D0,
    BulletReload2 = 0x3E8,
    Heal = 0x160
};

int main() {
    // test
    AppProcess* p = AppProcess::Create(_T("Call of Duty®: Modern Warfare® 3"));
    DWORD appAddress = p->getBaseAddress();
    DWORD playerDataAddress = 0;
    DWORD virtualAddressPlayerData = 0x0003A784;
    if (!p->readProcessMemory(appAddress + virtualAddressPlayerData, playerDataAddress)) {
        cout << "Couldn't read process memory";
        return -1;
    }
    cout << "Base exe address: " << appAddress << endl;
    cout << "Player address: " << playerDataAddress << endl;

    // update data
    int newBullet = 2000;
    int newReBullet = 200;
    int newHeal = 1000;
    while (true) {
        if (!p->writeProcessMemory(playerDataAddress + EOffsetPlayerData::Bullet1, newBullet)) {
            cout << "Couldn't write process memory" << endl;
        }
        if (!p->writeProcessMemory(playerDataAddress + EOffsetPlayerData::BulletReload1, newReBullet)) {
            cout << "Couldn't write process memory" << endl;
        }
        if (!p->writeProcessMemory(playerDataAddress + EOffsetPlayerData::BulletReload2, newReBullet)) {
            cout << "Couldn't write process memory" << endl;
        }
    /*    if (!p->writeProcessMemory(playerDataAddress + EOffsetPlayerData::Heal, newHeal)) {
            cout << "Couldn't write process memory" << endl;
        }*/
        Sleep(1000);
    }

    delete p;
    return 0;
}