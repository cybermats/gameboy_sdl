#pragma once

#include "mbc.h"
#include "gpu.h"
#include "interrupts.h"
#include "gbu-timer.h"
#include "joypad.h"
#include "gbu-serial.h"
#include "gb-audio.h"
#include <vector>

class Gpu;

class MMU
{
public:
    MMU(IMBC* mbc, Interrupts* interrupts, GbuTimer* timer, Joypad* joypad, Serial* serial, Audio* audio, bool useBios);

    unsigned char readByte(unsigned short addr);


    unsigned short readShort(unsigned short addr)
    {
        return readByte(addr) | (readByte(addr+1) << 8);
    }

    void writeByte(unsigned short addr, unsigned char value);

    void writeShort(unsigned short addr, unsigned short value)
    {
        unsigned char first = (unsigned char)(value & 0xFF);
        unsigned char second = (unsigned char)((value >> 8) & 0xFF);
        writeByte(addr, first);
        writeByte(addr+1, second);
    }

    void setGpu(Gpu* gpu);

	inline bool isBios()
	{
		return _isBios;
	}

private:
    IMBC* _mbc;
    Gpu* _gpu;
	Interrupts* _interrupts;
	GbuTimer* _timer;
	Joypad* _joypad;
	Serial* _serial;
	Audio* _audio;

    const unsigned char* _romBank0;
    const unsigned char* _romBankN;
    std::vector<unsigned char> _vram;
    unsigned char* _eram;
    unsigned char* _wramBank0;
    unsigned char* _wramBankN;
    std::vector<unsigned char> _hram;

    std::vector<unsigned char> _workRam;

    bool _isBios;



    unsigned char readIO(unsigned short addr);

    void writeIO(unsigned short addr, unsigned char value);


    static unsigned char _bios[];
};