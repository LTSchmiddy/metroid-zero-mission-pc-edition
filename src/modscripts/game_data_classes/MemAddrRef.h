#pragma once
#include "GameModHandler.h"
#include "Player.h"

#include "../gba/GBAinline.h"
#include "../common/Types.h"
#include "../System.h"
#include "../Util.h"



enum MemAddrRefSize {
	SingleByte = 1,
	HalfWord = 2,
	Word = 4
};


// This class should give me easier control of variables in the emulated ram.
// The template type 'T' is used for converting the data between the game ram, and how I want the data represented in the code.
// For example, the byte for current game-mode will only ever be one of a couple values. I can easily work with those if I use an enum.
//template <typename T>
//template <Enum T, class U>
template <class T>
class MemAddrRef {
private:
	void do_init(uint32_t p_addr, MemAddrRefSize p_size);


public:
	MemAddrRefSize size = MemAddrRefSize::SingleByte;
	uint32_t addr;

	MemAddrRef(uint32_t p_addr, MemAddrRefSize p_size);
	MemAddrRef(uint32_t p_addr, uint8_t p_size);
	
	T get();
	void set(T value);


};

template class MemAddrRef<uint8_t>;
template class MemAddrRef<uint16_t>;

