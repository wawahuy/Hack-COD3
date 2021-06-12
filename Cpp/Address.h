#pragma once
#include "AppProcess.h"
#include "Address.h"

enum EBase {
	Entity = 0x00E7DC50,		// Address pointer of entity first
	Player = 0x0003A784			// Address offset 0 of player
};

enum EEntity {
	Next = 0x74,				// Offset next pointer of EBase::Entity
	Heal = 0x150				// Offset next data of entity
};

enum EPlayer {
	X = 0x1C,
    Y = 0x20,
    Z = 0x24,
    Bullet1 = 0x358,
    BulletReload1 = 0x3D0,
};
