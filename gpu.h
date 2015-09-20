#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "mmu.h"
#include "interrupts.h"

#include <iostream>

class MMU;

class Gpu {
public:
    Gpu(MMU *mmu, Interrupts *interrupts);

    void checkline(uint64_t clockticks);


    unsigned char readByte(unsigned short addr) {
        switch (0xF000 & addr) {
            case 0x8000:
            case 0x9000:
                return _vram.at(0x1FFF & addr);
            default:
                assert(false);
                return 0;
        }
    }

    void writeByte(unsigned short addr, unsigned char value) {
        switch (0xF000 & addr) {
            case 0x8000:
            case 0x9000:
                _vram.at(0x1FFF & addr) = value;
                updateTile(addr, value);
                return;
            default:
                std::cout << "Gpu::writeByte, out of bounds, addr: " << addr << std::endl;
                assert(false);
                return;
        }
    }

    unsigned char readIO(unsigned short addr) {
        auto gaddr = addr - 0xFF40;
        switch (gaddr) {
            case 0:
                return (_displayEnable ? 0x80 : 0) |
                        ((_tileSet == 0x0000) ? 0x10 : 0) |
                        ((_tileMap == 0x1C00) ? 0x08 : 0) |
                        (_spriteVDouble ? 0x04 : 0) |
                        (_spriteEnable ? 0x02 : 0) |
                        (_bgEnable ? 0x01 : 0);
            case 1:
                return (_lycLyCoincidenceFlag ? 4 : 0) | _linemode;

            case 2:
                return _scrollY;

            case 3:
                return _scrollX;

            case 4:
                return _scanline;

            case 5:
                return _raster;

            default:
                return _regs.at(gaddr);
        }
    }

    void writeIO(unsigned short addr, unsigned char value);

    void updateOAM(unsigned short addr, unsigned char value) {
        addr &= 0x00FF;
        assert((addr >> 2) < 40);
        _spriteData[addr >> 2][addr & 3] = value;
        std::sort(_spriteData.begin(), _spriteData.end());
    }

    void updateTile(unsigned short addr, unsigned char value) {
        if (addr & 1)
            addr--;
        auto saddr = addr & 0x1FFF;
        auto tile = (addr >> 4) & 511;
        auto y = (addr >> 1) & 7;
        for (auto x = 0; x < 8; ++x) {
            auto sx = 1 << (7 - x);
            _tiles.at(tile).at(y).at(x) = ((_vram.at(saddr) & sx) ? 1 : 0) | ((_vram.at(saddr + 1) & sx) ? 2 : 0);
        }
    }

    uint8_t readOAM(unsigned short addr) {
        return _oam.at(0x00FF & addr);
    }

    void writeOAM(unsigned short addr, unsigned char value) {
        _oam.at(0x00FF & addr) = value;
        updateOAM(addr, value);
    }

    void renderimage() {
        /*
        std::cout << "Render image" << std::endl;
        for(auto y = 0; y < 144; y+=10)
        {
            for(auto x = 0; x < 160 * 3; x+=10)
            {
                auto idx = y * (160*3) + x;
                char c = '*';
                if(_screenData.at(idx) < 255 )
                    c = ' ';
                std::cout << c;
            }
            std::cout << std::endl;

        }
        */
//        std::cout << "x: " << (unsigned int)_scrollX << ", y: " << (unsigned int)_scrollY << std::endl;
        _hasImage = true;
    }

	void renderScanline();

    bool hasImage()
    {
        if(!_hasImage)
            return false;
        _hasImage = false;
        return true;
    }

    std::vector<uint8_t>& getFrame()
    {
        return _screenData;
    }


//private:

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
                    assert(false);
                    return y;
            }
        }

		bool operator<(const SpriteData& other)
		{
			if (x < other.x) return true;
			if (x > other.x) return false;
			if (patternNum < other.patternNum) return true;
			if (patternNum > other.patternNum) return false;
			if (y < other.y) return true;
			return false;
		}
    };

	void writeTilesToFile()
	{
		std::ofstream file("tiles.raw");
		std::ofstream filebg("tilesbg.data", std::ofstream::binary);
		std::ofstream fileobj0("tilesobj0.data", std::ofstream::binary);
		std::ofstream fileobj1("tilesobj1.data", std::ofstream::binary);
		for (auto& tile : _tiles)
		{
			for (auto& row : tile)
			{
				for (auto& cell : row)
				{
					switch (cell)
					{
					case 0:
						file << " "; break;
					case 1:
						file << "."; break;
					case 2:
						file << "|"; break;
					case 3:
						file << "*"; break;
					default:
						break;
					}

					auto bgc = _paletteBg.at(cell);
					auto obj0c = _paletteObj0.at(cell);
					auto obj1c = _paletteObj1.at(cell);
					filebg << bgc << bgc << bgc;
					fileobj0 << obj0c << obj0c << obj0c;
					fileobj1 << obj1c << obj1c << obj1c;
				}
				file << std::endl;
			}
			file << std::endl;
		}
	}


    MMU *_mmu;
	Interrupts *_interrupts;

	std::vector<unsigned char> _regs;
	std::vector<unsigned char> _oam;
	std::vector<unsigned char> _vram;
	std::vector<std::vector<std::vector<uint8_t>>> _tiles;
	std::vector<uint8_t> _scanrow;
	std::vector<uint8_t> _screenData;
	std::vector<uint8_t> _paletteBg;
	std::vector<uint8_t> _paletteObj0;
	std::vector<uint8_t> _paletteObj1;
	std::vector<SpriteData> _spriteData;

	uint32_t _currscan;
	uint8_t _raster;

    uint8_t _linemode;






    bool _hasImage;



	// Control registers
	bool _bgEnable;
	bool _spriteEnable;
	bool _spriteVDouble;
	bool _displayEnable;
	uint16_t _tileMap;
	uint16_t _tileSet;

	// GPU registers
	uint8_t _scrollX;
	uint8_t _scrollY;
	uint8_t _scanline;
	uint64_t _modeclocks;
	bool _lycLyCoincidenceFlag;

	// STAT interrupt selections
	bool _lycLyCoincidenceSelection;
	bool _mode10;
	bool _mode01;
	bool _mode00;
};