#include "GameModHandler.h"

#include "../gba/GBAinline.h"
#include "../common/Types.h"
#include "../System.h"
#include "../Util.h"
#include "../sdl/inputSDL.h"
#include "../common/ConfigManager.h"
#include "../common/Patch.h"
#include "Player.h"

#include "modscripts\game_data_classes\MemAddrRef.h"


//Local Helper Functions:
unsigned getPlaceValue(unsigned num, unsigned place) {
	return unsigned(num / 10 ^ place) - unsigned(num / 10 ^ (place + 1)) * 10;
}



// Old Player Data Access Methods - I want to switch to using the MemAddrRef class instead.



//Re-doing these as #define macros
// Local Variables:
//uint32_t healthBaseAddr = 0x03001536;
//uint32_t maxHealthBaseAddr = 0x03001530;
//
//uint32_t SamusPoseAddr = 0x030013D4;
//uint32_t SamusArmCannonDirAddr = 0x030013D6;
//uint32_t SamusDiagonalAimingFlagAddr = 0x03001414;


/*
// Public Functions:
uint16_t GetPlayerHealth() {
	return CPUReadHalfWord(healthBaseAddr);
}

void SetPlayerHealth(uint16_t health) {
	CPUWriteHalfWord(healthBaseAddr, health);

}

uint16_t GetPlayerMaxHealth() {
	return CPUReadHalfWord(maxHealthBaseAddr);
}

void SetPlayerMaxHealth(uint16_t health) {
	CPUWriteHalfWord(maxHealthBaseAddr, health);
}

SamusPose GetSamusPose() {
	return (SamusPose) CPUReadByte(SamusPoseAddr);
}

void SetSamusPose(SamusPose pose) {
	CPUWriteByte(SamusPoseAddr, (uint8_t)pose);
}

SamumArmCannonDirection GetSamusCannonDir() {
	return (SamumArmCannonDirection) CPUReadByte(SamusArmCannonDirAddr);
}

void SetSamusCannonDir(SamumArmCannonDirection dir) {
	CPUWriteByte(SamusArmCannonDirAddr, (uint8_t)dir);
}

SamusDiagonalAimingFlag GetSamusAimingFlag() {
	return (SamusDiagonalAimingFlag) CPUReadByte(SamusDiagonalAimingFlagAddr);;
}

void SetSamusAimingFlag(SamusDiagonalAimingFlag flag) {
	CPUWriteByte(SamusDiagonalAimingFlagAddr, (uint8_t)flag);
}
*/
