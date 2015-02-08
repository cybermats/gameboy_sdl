#pragma once
#include <cassert>
#include <vector>
#include "mmu.h"

#include <iostream>

class MMU;

class Gpu {
public:
    Gpu(MMU *mmu);

    void checkline(uint64_t clockticks);


    unsigned char readByte(unsigned short addr) {
        switch (0xF000 & addr) {
            case 0x8000:
            case 0x9000:
                return _vram[0x1FFF & addr];
            default:
                assert("Bad read of video memory");
                return 0;
        }
    }

    unsigned char writeByte(unsigned short addr, unsigned char value) {
        switch (0xF000 & addr) {
            case 0x8000:
            case 0x9000:
                _vram[0x1FFF & addr] = value;
                updateTile(addr, value);
            default:
                assert("Bad write of video memory");
                return 0;
        }
    }

    unsigned char readIO(unsigned short addr) {
        auto gaddr = addr - 0xFF40;
        switch (gaddr) {
            case 0:
                return (_lcdOn ? 0x80 : 0) |
                        ((_bgtilebase == 0x0000) ? 0x10 : 0) |
                        ((_bgmapbase == 0x1C00) ? 0x08 : 0) |
                        (_spriteSize ? 0x04 : 0) |
                        (_spriteOn ? 0x02 : 0) |
                        (_bgOn ? 0x01 : 0);
            case 1:
                return (_currline == _raster ? 4 : 0) | _linemode;

            case 2:
                return _yscrl;

            case 3:
                return _xscrl;

            case 4:
                return _currline;

            case 5:
                return _raster;

            default:
                return _regs[gaddr];
        }
    }

    void writeIO(unsigned short addr, unsigned char value);

    void updateOAM(unsigned short addr, unsigned char value) {
        addr &= 0x00FF;
        assert((addr >> 2) < 40);
        _spriteData[addr >> 2][addr & 3] = value;
        std::sort(_spriteData.begin(), _spriteData.end(),
                [](const SpriteData &a, const SpriteData &b) {
                    if (a.x < b.x) return false;
                    if (a.patternNum < b.patternNum) return false;
                    return true;
                });

    }

    void updateTile(unsigned short addr, unsigned char value) {
        if (addr & 1)
            addr--;
        auto saddr = addr;
        auto tile = (addr >> 4) & 511;
        auto y = (addr >> 1) & 7;
        for (auto x = 0; x < 8; ++x) {
            auto sx = 1 << (7 - x);
            _tilemap[tile][y][x] = ((_vram[saddr] & sx) ? 1 : 0) | ((_vram[saddr + 1] & sx) ? 2 : 0);
        }
    }

    uint8_t readOAM(unsigned short addr) {
        return _oam[0x00FF & addr];
    }

    void writeOAM(unsigned short addr, unsigned char value) {
        _oam[0x00FF & addr] = value;
        updateOAM(addr, value);
    }

    void renderimage() {
    }


private:

    struct SpriteData {
    private:
        struct SpriteFlagsBits {
            uint8_t _unused :4;
            uint8_t paletteNum :1;
            uint8_t xFlip :1;
            uint8_t yFlip :1;
            uint8_t prio :1;
        };

        union SpriteFlagUnion {
            uint8_t data;
            SpriteFlagsBits bits;
        };
    public:
        SpriteData()
                : y(-16), x(-8), patternNum(0), flags({0}) {
        }

        uint8_t y;
        uint8_t x;
        uint8_t patternNum;
        SpriteFlagUnion flags;

        uint8_t &operator[](size_t idx) {
            assert(idx < 4);
            switch (idx) {
                case 0:
                    return y;
                case 1:
                    return x;
                case 2:
                    return patternNum;
                case 3:
                    return flags.data;
                default:
                    return y;
            }
        }
    };


    MMU *_mmu;

    uint64_t _modeclocks;
    uint8_t _linemode;
    uint8_t _currline;
    uint8_t _currscan;
    uint16_t _bgmapbase;
    uint16_t _bgtilebase;
    uint8_t _xscrl;
    uint8_t _yscrl;
    uint8_t _raster;

    bool _lcdOn;
    bool _bgOn;
    bool _spriteOn;
    bool _spriteSize;

    std::vector<unsigned char> _regs;
    std::vector<unsigned char> _vram;
    std::vector<unsigned char> _oam;
    std::vector<std::vector<std::vector<uint8_t>>> _tilemap;
    std::vector<uint8_t> _scanrow;
    std::vector<uint8_t> _screenData;
    std::vector<uint8_t> _paletteBg;
    std::vector<uint8_t> _paletteObj0;
    std::vector<uint8_t> _paletteObj1;
    std::vector<SpriteData> _spriteData;

};