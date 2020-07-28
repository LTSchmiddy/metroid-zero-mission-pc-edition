#include "MemAddrRef.h"

//#include "GameModHandler.h"

#include "../gba/GBAinline.h"
#include "../common/Types.h"
#include "../System.h"
#include "../Util.h"
#include "../sdl/inputSDL.h"
#include "../common/ConfigManager.h"
#include "../common/Patch.h"

//export
template <class T>
void MemAddrRef<T>::do_init(uint32_t p_addr, MemAddrRefSize p_size) {
	addr = p_addr;
	size = p_size;
}

template <class T>
MemAddrRef<T>::MemAddrRef(uint32_t p_addr, MemAddrRefSize p_size) {
	do_init(p_addr, p_size);
}

template <class T>
MemAddrRef<T>::MemAddrRef(uint32_t p_addr, uint8_t p_size) {
	do_init(p_addr, (MemAddrRefSize)p_size);
}

template<class T>
T MemAddrRef<T>::get()
{
	if (size == MemAddrRefSize::SingleByte) {
		return (T)CPUReadByte(addr);

	} else if(size == MemAddrRefSize::HalfWord) {
		return (T)CPUReadHalfWord(addr);

	}
	else if (size == MemAddrRefSize::Word) {
		return (T)CPUReadMemory(addr);

	} else {
		return 0;
	}
}

template<class T>
void MemAddrRef<T>::set(T value) {
	if (size == MemAddrRefSize::SingleByte) {
		CPUWriteByte(addr, (uint8_t) value);

	}
	else if (size == MemAddrRefSize::HalfWord) {
		CPUWriteHalfWord(addr, (uint16_t) value);

	}
	else if (size == MemAddrRefSize::Word) {
		CPUWriteMemory(addr, (uint32_t) value);

	}
}


