#pragma once
#include "AppProcess.h"
#include "Address.h"

namespace address {
	enum EBase {
		Entity = 0x00E7DC50,		// Address pointer of entity first
		Player = 0x0003A784,		// Address pointer 0 of player
		ViewMatrix = 0x20EB908
	};

	enum EEntity {
		Next = 0x74,				// Offset next pointer of EBase::Entity
		Heal = 0x150,				// Offset next data of entity
		EntityVec3 = 0x344,
	};

	enum EPlayer {
		PlayerVec3 = 0x1C,
		Bullet1 = 0x358,
		BulletReload1 = 0x3D0,
	};


	enum EViewMatrix {
		Position = 0x0,
		V1 = 0xC,
		V2 = 0x18,
		V3 = 24
	};
}