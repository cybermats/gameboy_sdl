#include "gpu.h"
#include "mmu.h"

Gpu::Gpu(MMU* mmu, Interrupts* interrupts)
: _mmu(mmu)
, _interrupts(interrupts)
, _regs(0xc, 0)
, _vram(0x2000, 0)
, _paletteBg(4, 255)
, _paletteObj0(4, 255)
, _paletteObj1(4, 255)
  , _tiles(512, std::vector<std::vector<uint8_t>>(8, std::vector<uint8_t>(8, 0)))
, _screenData(160*144, 255)
, _oam(0xA0, 0)
, _scanline(0)
, _currscan(0)
, _linemode(0)
, _modeclocks(0)
, _scrollY(0)
, _scrollX(0)
, _raster(0)
, _displayEnable(false)
, _bgEnable(false)
, _spriteEnable(false)
, _spriteVDouble(0)
, _scanrow(160, 0)
, _spriteData(40)
, _tileSet(0x0000)
, _tileMap(0x1800)
, _hasImage(false)
, _lycLyCoincidenceFlag(false)
, _lycLyCoincidenceSelection(false)
, _mode10(false)
, _mode01(false)
, _mode00(false)
{}


void Gpu::checkline(uint64_t clockticks)
{
	bool oldEquals = _raster == _scanline;

    _modeclocks += clockticks;
    switch (_linemode)
    {
        // In HBlank
        case 0:
            if(_modeclocks >= 51)
            {
                // End of H Blank for last line. Render screen.
                if(_scanline == 143) {
                    _linemode = 1;
                    renderimage();
					_interrupts->flagInterrupt(Interrupts::Vblank);
                }
                else {
                    _linemode = 2;
                }
                _modeclocks -= 51;
                _scanline++;
                _currscan +=160;
            }
            break;
        // In VBlank
        case 1:
            if(_modeclocks >= 114) {
                _modeclocks -= 114;
                _scanline++;
                if (_scanline > 153) {
                    _scanline = 0;
                    _currscan = 0;
                    _linemode = 2;
                }
            }
            break;
        // In OAM read mode
        case 2:
            if(_modeclocks >= 20)
            {
                _modeclocks -= 20;
                _linemode = 3;
            }
            break;
        // VRAM read mode, scanline active
        case 3:
            if(_modeclocks >= 43)
            {
                _modeclocks -= 43;
                _linemode = 0;
				renderScanline();
            }
            break;
            // In HBlank
    }
	bool newEquals = _raster == _scanline;
	_lycLyCoincidenceFlag = newEquals;
	
	bool raiseInt = false;
	if ((_lycLyCoincidenceSelection && (oldEquals != newEquals) && _lycLyCoincidenceFlag) ||
	    (_mode00 && (_linemode == 0)) ||
		(_mode01 && (_linemode == 1)) ||
		(_mode10 && (_linemode == 2)))
		raiseInt = true;

	if (raiseInt)
		_interrupts->flagInterrupt(Interrupts::LcdStat);
}

void Gpu::writeIO(unsigned short addr, unsigned char value)
{
    auto gaddr = addr - 0xFF40;
    _regs.at(gaddr) = value;
    switch(gaddr)
    {
        case 0:
            _displayEnable = (value&0x80)?1:0;
            _tileSet = (value&0x10)?0x0000:0x0800;
            _tileMap = (value&0x08)?0x1C00:0x1800;
            _spriteVDouble = (value&0x04)?1:0;
            _spriteEnable = (value&0x02)?1:0;
            _bgEnable = (value&0x01)?1:0;
            break;

		case 1:
			_lycLyCoincidenceSelection = (value & (1 << 6)) > 0;
			_mode10 = (value & (1 << 5)) > 0;
			_mode01 = (value & (1 << 4)) > 0;
			_mode00 = (value & (1 << 3)) > 0;
			break;

        case 2:
            _scrollY = value;
            break;

        case 3:
            _scrollX = value;
            break;

		case 4:
			_scanline = 0;
			break;

        case 5:
            _raster = value;
			break;

            // OAM DMA
        case 6:
            for(auto i=0; i<160; i++)
            {
                auto v = _mmu->readByte((value<<8)+i);
                _oam.at(i) = v;
                updateOAM(0xFE00+i, v);
            }
            break;

            // BG palette mapping
        case 7:
            for(auto i=0;i<4;i++)
            {
                switch((value>>(i*2))&3)
                {
                    case 0: _paletteBg.at(i) = 255; break;
                    case 1: _paletteBg.at(i) = 192; break;
                    case 2: _paletteBg.at(i) = 96; break;
                    case 3: _paletteBg.at(i) = 0; break;
                }
            }
            break;

            // OBJ0 palette mapping
        case 8:
            for(auto i=0;i<4;i++)
            {
                switch((value>>(i*2))&3)
                {
                    case 0: _paletteObj0.at(i) = 255; break;
                    case 1: _paletteObj0.at(i) = 192; break;
                    case 2: _paletteObj0.at(i) = 96; break;
                    case 3: _paletteObj0.at(i) = 0; break;
                }
            }
            break;

            // OBJ1 palette mapping
        case 9:
            for(auto i=0;i<4;i++)
            {
                switch((value>>(i*2))&3)
                {
                    case 0: _paletteObj1.at(i) = 255; break;
                    case 1: _paletteObj1.at(i) = 192; break;
                    case 2: _paletteObj1.at(i) = 96; break;
                    case 3: _paletteObj1.at(i) = 0; break;
                }
            }
            break;
    }
}

void Gpu::renderScanline()
{
	int mapOffset = _tileMap;
	mapOffset += (((_scanline + _scrollY) & 255) >> 3) << 5;

	int lineOffset = (_scrollX);

	int x = _scrollX & 7;
	int y = (_scanline + _scrollY) & 7;

	int pixelOffset = _scanline * 160;

	unsigned short tile = _vram.at(mapOffset + lineOffset);
	if (_tileSet && tile < 0x80)
		tile += 0x100;

	unsigned char scanlineRow[160];

	if (!_displayEnable)
	{
		for (int i = 0; i < 160; ++i)
			_screenData[i + pixelOffset] = 255;
		return;
	}

	// if bg enabled
	if (_bgEnable)
	{
		for (int i = 0; i < 160; ++i)
		{
			unsigned char color = _tiles.at(tile).at(y).at(x);
			scanlineRow[i] = color;

			_screenData[pixelOffset] = _paletteBg.at(color);

			++pixelOffset;
			++x;
			if (x == 8)
			{
				x = 0;
				lineOffset = (lineOffset + 1) & 31;
				tile = _vram.at(mapOffset + lineOffset);
				if (_tileSet && tile < 0x80)
					tile += 0x100;
			}
		}
	}

	if (_spriteEnable)
	{
		for (int i = 0; i < 40; ++i)
		{
			const auto& sprite = _spriteData.at(i);
			auto sx = sprite.x;
			auto sy = sprite.y;
			if (sx == 0 && sy == 0)
				continue;

			if (sy <= _scanline && (sy + 8) > _scanline)
			{
				const auto& pal = sprite.flags.bits.paletteNum ? _paletteObj1 : _paletteObj0;

				int pixelOffset = _scanline * 160 + sx;
				unsigned char tileRow = _scanline - sy;
				if (sprite.flags.bits.xFlip)
					tileRow = 7 - tileRow;

				for (int x = 0; x < 8; ++x)
				{
					if ((sx + x) >= 0 && (sx + x < 160) && (~sprite.flags.bits.prio || !scanlineRow[sx + x]))
					{
						unsigned char color;
						if (sprite.flags.bits.yFlip)
							color = _tiles.at(sprite.patternNum).at(tileRow).at(7 - x);
						else
							color = _tiles.at(sprite.patternNum).at(tileRow).at(x);

						if (color)
						{
							_screenData[pixelOffset] = pal.at(color);
						}
						++pixelOffset;
					}
				}
			}
		}
	}
}