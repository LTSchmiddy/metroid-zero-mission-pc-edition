#pragma once
#include "../gba/GBAinline.h"
#include "../sdl/inputSDL.h"

#include "modscripts/game_data_classes/MemAddrRef.h"

#include <stdio.h>

#include <iostream>
#include <fstream>


// Local Variables:
#define SamusHealthBaseAddr 0x03001536
#define SamusMaxHealthBaseAddr 0x03001530

#define SamusPoseAddr 0x030013D4
#define SamusArmCannonDirAddr 0x030013D6
#define SamusDiagonalAimingFlagAddr 0x03001414


//enum class SamusPose {
enum SamusPose {
	Running = 0x0,
	Standing = 0x1,
	Turning_Around = 0x2,
	Shooting = 0x3,
	Crouching = 0x4,
	Turning_Around_And_Crouching = 0x5,
	Shooting_And_Crouching = 0x6,
	Skidding = 0x7,
	Jumping_falling = 0x8,
	Turning_Around_AndJumping_falling = 0x9,
	Landing = 0xA,
	Starting_A_Spin_jump = 0xB,
	Spinning = 0xC,
	Wall_jumping = 0xD,
	Space_jumping = 0xE,
	Screw_attacking = 0xF,
	Morphing = 0x10,
	Morph_Ball = 0x11,
	Rolling = 0x12,
	Unmorphing = 0x13,
	Jumping_falling_In_Morphball = 0x14,
	Hanging_On_A_Ledge = 0x15,
	Starting_To_Hold_Your_ArmCannon_Out_On_A_Ledge = 0x16,
	Starting_To_Hold_Your_ArmCannon_In_On_A_Ledge = 0x17,
	Holding_Your_ArmCannon_Out_On_A_Ledge = 0x18,
	Shooting_On_A_Ledge = 0x19,
	Pulling_Yourself_Up_From_Hanging = 0x1A,
	Pulling_Yourself_Forward_From_Hanging = 0x1B,
	Pulling_Yourself_Into_A_Morphball_Tunnel = 0x1C,
	Using_An_Elevator = 0x1D,
	Facing_The_Foreground = 0x1E,
	Turning_To_from_The_Foreground = 0x1F,
	Getting_Held_By_Chozo = 0x20,
	Delay_Before_Shinesparking = 0x21,
	Shinesparking = 0x22,
	Delay_After_Shinesparking = 0x23,
	Spinning_After_Shinespark = 0x24,
	Delay_Before_Ballsparking = 0x25,
	Ballsparking = 0x26,
	Delay_After_Ballsparking = 0x27,
	Hanging_On_A_Zipline = 0x28,
	Shooting_On_A_Zipline = 0x29,
	Turning_On_A_Zipline = 0x2A,
	Hanging_On_A_Zipline_In_Morphball = 0x2B,
	On_A_Save_Pad = 0x2C,
	Downloading_A_Map = 0x2D,
	Turning_Around_For_A_Map_Station = 0x2E,
	Getting_Hurt = 0x2F,
	Getting_Knocked_Back = 0x30,
	Getting_Hurt_In_Morphball = 0x31,
	Getting_Knocked_Back_Morphball = 0x32,
	Dying = 0x33,
	Entering_A_Crawlspace = 0x34,
	In_ACrawlspace = 0x35,
	Turned_Around_In_ACrawlspace = 0x36,
	Crawling = 0x37,
	Exiting_A_Crawlspace = 0x38,
	Turning_Around_In_A_Crawlspace = 0x39,
	Shooting_In_A_Crawlspace = 0x3A,
	Uncrouching_turned = 0x3B,
	Zero_Suit_Crouching = 0x3C,
	Grabbing_A_Ledge = 0x3D,
	Facing_The_Background = 0x3E,
	Turning_to_from_The_Background = 0x3F,
	Activating_The_Ziplines = 0x40,
	In_The_Space_Pirate_Ship = 0x41,
	Turning_Around_For_The_Space_Pirate_Ship = 0x42,

};

//enum class SamumArmCannonDirection {
enum SamumArmCannonDirection {
	Forward = 0x00,
	Diagonally_upwards = 0x01,
	Diagonally_downwards = 0x02,
	Upwards = 0x03,
	Downwards = 0x04,
	NoDirection = 0x05
};

//enum class SamusDiagonalAimingFlag {
enum SamusDiagonalAimingFlag {
	NoFlag = 0x00,
	Up = 0x01,
	Down = 0x02,
};


//struct SamusPlayerData {
//	MemAddrRef<uint16_t> health = MemAddrRef<uint16_t>(SamusHealthBaseAddr, MemAddrRefSize::HalfWord);
//	MemAddrRef<uint16_t> max_health = MemAddrRef<uint16_t>(SamusMaxHealthBaseAddr, MemAddrRefSize::HalfWord);
//
//	MemAddrRef<uint8_t> pose = MemAddrRef<uint8_t>(SamusPoseAddr, MemAddrRefSize::SingleByte);
//	MemAddrRef<uint8_t> cannon_direction = MemAddrRef<uint8_t>(SamusArmCannonDirAddr, MemAddrRefSize::SingleByte);
//	MemAddrRef<uint8_t> diagonal_aiming_flag = MemAddrRef<uint8_t>(SamusDiagonalAimingFlagAddr, MemAddrRefSize::SingleByte);
//};

//extern PlayerData Samus;

//uint16_t GetPlayerHealth();
//void SetPlayerHealth(uint16_t health);
//
//uint16_t GetPlayerMaxHealth();
//void SetPlayerMaxHealth(uint16_t health);
//
//SamusPose GetSamusPose();
//void SetSamusPose(SamusPose pose);
//
//SamumArmCannonDirection GetSamusCannonDir();
//void SetSamusCannonDir(SamumArmCannonDirection dir);
//
//SamusDiagonalAimingFlag GetSamusAimingFlag();
//void SetSamusAimingFlag(SamusDiagonalAimingFlag flag);

