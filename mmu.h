#pragma once

#include "mbc.h"
#include <vector>

class MMU
{
public:
    MMU(IMBC* mbc);

    unsigned char readByte(unsigned short addr)
    {
        switch(addr & 0xF000)
        {
            // ROM Bank 0
            case 0x000:
            {
                if(_isBios)
                {
                    if(addr < 0x100)
                        return _bios[addr];
                    else if(addr == 0x100)
                        _isBios = false;
                }
                return _romBank0[addr];
            }
            case 0x1000:
            case 0x2000:
            case 0x3000:
                return _romBank0[addr];

                // ROM Bank N
            case 0x4000:
            case 0x5000:
            case 0x6000:
            case 0x7000:
                return _romBankN[0x3FFF & addr];

                // Video RAM
            case 0x8000:
            case 0x9000:
                return _vram[0x1FFF & addr];

                // External RAM
            case 0xA000:
            case 0xB000:
                return _eram[0x1FFF & addr];

                // Working RAM
            case 0xC000:
                return _wramBank0[0x0FFF & addr];
            case 0xD000:
                return _wramBankN[0x0FFF & addr];

                // Working RAM ECHO
            case 0xE000:
                return _wramBank0[0x0FFF & addr];
            case 0xF000:
            {
                if(addr < 0xFE00) // ECHO 0xF000 - 0xFDFF
                    return _wramBankN[0x0FFF & addr];
                if(addr < 0xFEA0) // OAM  0xFE00 - 0xFE9F
                    return _oam[0x00FF & addr];
                if(addr < 0xFF00) // Unusable 0xFEA0 - 0xFEFF
                    return 0;
                if(addr < 0xFF80) // I/O 0xFF00 - 0xFF7F
                    return readIO(addr);
                if(addr < 0xFFFF) // HRAM 0xFF80 - 0xFFFE
                    return _hram[0x007F & addr];
                return _ier; // Interrupts Enable Register
            }
            default:
                return 0; // Should never happen
        }
    }

    void writeByte(unsigned short addr, unsigned char value)
    {
        switch (0xF000 & addr)
        {
                // ROM Bank 0
            case 0x0000:
            case 0x1000:
            case 0x2000:
            case 0x3000:

                // ROM Bank N
            case 0x4000:
            case 0x5000:
            case 0x6000:
            case 0x7000:
                _mbc->update(addr, value);
                _romBank0 = _mbc->getRomBank0();
                _romBankN = _mbc->getRomBankN();
                _eram = _mbc->getRamBank();
                return;

                // Video RAM
            case 0x8000:
            case 0x9000:
                _vram[0x1FFF & addr] = value;
                // TODO: Handle video
                return;

                // External RAM
            case 0xA000:
            case 0xB000:
                _eram[0x1FFF & addr] = value;
                return;

                // Working RAM Bank 0
            case 0xC000:
                _wramBank0[0x0FFF & addr] = value;
                return;

                // Working RAM Bank N
            case 0xD000:
                _wramBankN[0x0FFF & addr] = value;
                return;

                // Working RAM ECHO
            case 0xE000:
                _wramBank0[0x0FFF & addr] = value;
                return;

                // Other
            case 0xF000:
            {
                if(addr < 0xFE00) // ECHO 0xF000 - 0xFDFF
                {
                    _wramBankN[0x0FFF & addr] = value;
                    return;
                }
                if(addr < 0xFEA0) // OAM  0xFE00 - 0xFE9F
                {
                    _oam[0x00FF & addr] = value;
                    // TODO: Handle video
                    return;
                }
                if(addr < 0xFF00) // Unusable 0xFEA0 - 0xFEFF
                    return;
                if(addr < 0xFF80) // I/O 0xFF00 - 0xFF7F
                {
                    writeIO(addr, value);
                    return;
                }
                if(addr < 0xFFFF) // HRAM 0xFF80 - 0xFFFE
                {
                    _hram[0x007F & addr] = value;
                    return;
                }
                _ier = value; // Interrupts Enable Register
                return;
            }
            default:
                return;
        }
    }

private:
    IMBC* _mbc;

    const unsigned char* _romBank0;
    const unsigned char* _romBankN;
    std::vector<unsigned char> _vram;
    unsigned char* _eram;
    unsigned char* _wramBank0;
    unsigned char* _wramBankN;
    std::vector<unsigned char> _oam;
    std::vector<unsigned char> _hram;
    unsigned char _ier; // Interrupts Enable Register

    std::vector<unsigned char> _workRam;

    bool _isBios;

    unsigned char readIO(unsigned short addr)
    {
        if(addr == 0xFF00) // Keyboard
            return 0; // TODO
        if(addr < 0xFF04) // Serial port
            return 0;
        if(addr < 0xFF08) // Timer
            return 0; // TODO
        if(addr == 0xFF0F) // Interrupt flags
            return 0; // TODO
        if(addr < 0xFF40) // Sound
            return 0; // Ignore?
        // TODO: GPU
        return 0;
    }

    void writeIO(unsigned short addr, unsigned char value)
    {
        if(addr == 0xFF00) // Keyboard
            return; // TODO:
        if(addr < 0xFF04) // Serial port
            return;
        if(addr < 0xFF08) // Timer
            return; // TODO
        if(addr == 0xFF0F) // Interrupt flags
            return; // TODO
        if(addr < 0xFF40) // Sound
            return; // Ignore?
        // TODO: GPU
        return;
    }


    static unsigned char _bios[];
};