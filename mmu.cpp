#include "mmu.h"
#include "gpu.h"

MMU::MMU(IMBC* mbc)
        : _mbc(mbc)
        , _gpu(nullptr)
        , _romBank0(nullptr)
        , _romBankN(nullptr)
        , _vram(0x2000, 0)
        , _eram(nullptr)
        , _wramBank0(nullptr)
        , _wramBankN(nullptr)
        , _hram(0x7e, 0)
        , _ier(0x0)
        , _workRam(0x2000, 0)
        , _isBios(true)
{
    _romBank0 = _mbc->getRomBank0();
    _romBankN = _mbc->getRomBankN();
    _eram = _mbc->getRamBank();

    _wramBank0 = &_workRam[0];
    _wramBankN = &_workRam[0x1000];
}

unsigned char MMU::readByte(unsigned short addr)
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
            assert(_gpu);
            return _gpu->readByte(addr);
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
            if(addr < 0xFEA0) { // OAM  0xFE00 - 0xFE9F
                assert(_gpu);
                return _gpu->readOAM(addr);
            }
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

void MMU::writeByte(unsigned short addr, unsigned char value)
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
            assert(_gpu);
            _gpu->writeByte(addr, value);
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
                assert(_gpu);
                _gpu->writeOAM(addr, value);
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
            assert(false);
            return;
    }
}

unsigned char MMU::readIO(unsigned short addr)
{
    if(addr == 0xFF00) // Keyboard
        return 0; // TODO
    if(addr < 0xFF04) // Serial port
        return 0;
    if(addr < 0xFF08) // Timer
        return 0; // TODO
    if(addr == 0xFF0F) // Interrupt flags
        return _if; // TODO
    if(addr < 0xFF40) // Sound
        return 0; // Ignore?
    if(addr < 0xFF4C) {
        assert(_gpu);
        return _gpu->readIO(addr);
    }
    return 0;
}

void MMU::writeIO(unsigned short addr, unsigned char value)
{
    if(addr == 0xFF00) // Keyboard
        return; // TODO:
    if(addr < 0xFF04) // Serial port
        return;
    if(addr < 0xFF08) // Timer
        return; // TODO
    if(addr == 0xFF0F) { // Interrupt flags
        _if = value;
        return; // TODO
    }
    if(addr < 0xFF40) // Sound
        return; // Ignore?
    if(addr < 0xFF4C) {
        assert(_gpu);
        _gpu->writeIO(addr, value);
    }
    return;
}


void MMU::setGpu(Gpu* gpu)
{
    _gpu = gpu;
}




unsigned char MMU::_bios[] = {
        0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
        0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
        0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
        0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
        0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
        0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
        0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
        0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
        0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
        0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
        0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
        0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
        0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
        0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C,
        0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
        0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
};