#pragma once

#include "mbc.h"
#include <vector>

class Gpu;

class MMU
{
public:
    MMU(IMBC* mbc);

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

    void setIF(unsigned char flags)
    {
        _if |= flags;
    }

    void resetIF(unsigned char flags)
    {
        _if &= flags;
    }

    void setGpu(Gpu* gpu);

private:
    IMBC* _mbc;
    Gpu* _gpu;

    const unsigned char* _romBank0;
    const unsigned char* _romBankN;
    std::vector<unsigned char> _vram;
    unsigned char* _eram;
    unsigned char* _wramBank0;
    unsigned char* _wramBankN;
    std::vector<unsigned char> _hram;
    unsigned char _ier; // Interrupts Enable Register
    unsigned char _if;

    std::vector<unsigned char> _workRam;

    bool _isBios;



    unsigned char readIO(unsigned short addr);

    void writeIO(unsigned short addr, unsigned char value);


    static unsigned char _bios[];
};