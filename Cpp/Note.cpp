//
//enum EEntity {
//    Base = 0x00E7DC50,
//    Next = 0x74,
//    In_heal = 0x150
//};
//
////0x0003A784 
//enum EOffsetPlayerData {
//    X = 0x1C,
//    Y = 0x20,
//    Z = 0x24,
//    Bullet1 = 0x358,
//    BulletReload1 = 0x3D0,
//    BulletReload2 = 0x3E8,
//};
//
//int main() {
//    // test
//    AppProcess* p = AppProcess::Create(_T("Call of Duty®: Modern Warfare® 3"));
//    DWORD appAddress = p->getBaseAddress();
//    DWORD playerDataAddress = 0;
//    DWORD virtualAddressPlayerData = 0x0003A784;
//    if (!p->readProcessMemory(appAddress + virtualAddressPlayerData, playerDataAddress)) {
//        cout << "Couldn't read process memory";
//        return -1;
//    }
//    cout << "Base exe address: " << appAddress << endl;
//    cout << "Player address: " << playerDataAddress << endl;
//
//    //KeyboardHook::getInstance()->async();
//    //KeyboardHook::getInstance()->listen([&](DWORD vk, DWORD code) {
//    //    if (vk == VK_SPACE) {
//    //        float z;
//    //        if (!p->readProcessMemory(playerDataAddress + EOffsetPlayerData::Z, z)) {
//    //            cout << "Couldn't read process memory";
//    //            return -1;
//    //        }
//    //        z += 200;
//    //        if (!p->writeProcessMemory(playerDataAddress + EOffsetPlayerData::Z, z)) {
//    //            cout << "Couldn't write process memory" << endl;
//    //        }
//    //    }
//    //    if (vk == VK_BACK) {
//    //        float z;
//    //        if (!p->readProcessMemory(playerDataAddress + EOffsetPlayerData::Z, z)) {
//    //            cout << "Couldn't read process memory";
//    //            return -1;
//    //        }
//    //        z -= 200;
//    //        if (!p->writeProcessMemory(playerDataAddress + EOffsetPlayerData::Z, z)) {
//    //            cout << "Couldn't write process memory" << endl;
//    //        }
//    //    }
//    //});
//
//    // update data
//    int newBullet = 2000;
//    int newReBullet = 200;
//    int newHeal = 1000;
//
//    while (true) {
//        system("cls");
//        for (int i = 0; i < 14; i++) {
//            DWORD address;
//            if (p->readProcessMemory(appAddress + EEntity::Base + EEntity::Next * i, address)) {
//                int healEn;
//                if (p->readProcessMemory(address + EEntity::In_heal, healEn)) {
//                    cout << i << " - " << address << " address: " << healEn << endl;
//                }
//            }
//        }
//        if (!p->writeProcessMemory(playerDataAddress + EOffsetPlayerData::Bullet1, newBullet)) {
//            cout << "Couldn't write process memory" << endl;
//        }
//        if (!p->writeProcessMemory(playerDataAddress + EOffsetPlayerData::BulletReload1, newReBullet)) {
//            cout << "Couldn't write process memory" << endl;
//        }
//        Sleep(1000);
//    }
//
//    delete p;
//    return 0;
//}