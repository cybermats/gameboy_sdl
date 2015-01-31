#pragma once
#include <vector>

class IMBC
{
public:
    virtual const unsigned char* getRomBank0() const = 0;

    virtual const unsigned char* getRomBankN() const = 0;

    virtual unsigned char* getRamBank() = 0;

    virtual void update(unsigned short addr, unsigned char value) = 0;
};

class MBC1 : public IMBC
{
public:
    MBC1(std::vector<unsigned char> rom, std::vector<unsigned char> ram)
        : _rom(rom)
        , _ram(ram)
        , _romSelector(1)
        , _ramSelector(0)
        , _ramMode(0)
    {}


    virtual const unsigned char *getRomBank0() const override
    {
        return &_rom[0];
    }

    virtual const unsigned char *getRomBankN() const override
    {
        size_t romBank = _romSelector & 0x001f;
        if(!_ramMode)
            romBank += (0x0003 & _ramSelector) << 5;
        return &_rom[0x4000 * romBank];
    }

    virtual unsigned char *getRamBank() override
    {
        if(_ram.empty())
            return nullptr;

        size_t ramBank = 0;
        if(_ramMode)
            ramBank = 0x0003 & _ramSelector;
        return &_ram[0x2000 * ramBank];
    }

    virtual void update(unsigned short addr, unsigned char value) override
    {
        if(addr < 0x2000)
            return; // RAM Enable
        if(addr < 0x4000)
        {
            _romSelector = value;
            return;
        }
        if(addr < 0x6000)
        {
            _ramSelector = value;
            return;
        }
        if(addr < 0x8000)
        {
            _ramMode = (value & 0x0001);
            return;
        }
    }

private:
    std::vector<unsigned char> _rom;
    std::vector<unsigned char> _ram;

    unsigned char _romSelector;
    unsigned char _ramSelector;
    bool _ramMode;
};

