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
, _enableDisplay(false)
, _enableBackground(false)
, _enableSprite(false)
, _enableWindow(false)
, _spriteVDouble(0)
, _scanrow(160, 0)
, _spriteData(40)
, _tileSet(0x0000)
, _tileMapBackground(0x1800)
, _tileMapWindow(0x1800)
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
			_enableDisplay = (value&(1 << 7)) ? 1 : 0;
			_tileMapWindow = (value&(1 << 6)) ? 0x1C00 : 0x1800;
			_enableWindow = (value & (1 << 5)) ? 1 : 0;
			_tileSet = (value&(1 << 4)) ? 0x0000 : 0x0800;
			_tileMapBackground = (value&(1 << 3)) ? 0x1C00 : 0x1800;
			_spriteVDouble = (value&(1 << 2)) ? 1 : 0;
			_enableSprite = (value&(1 << 1)) ? 1 : 0;
			_enableBackground = (value&(1 << 0)) ? 1 : 0;
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
                auto v = _mmu->readByte((uint16_t)((value<<8)+i));
                _oam.at(i) = v;
                updateOAM((uint16_t)(0xFE00+i), v);
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
		case 0xa:
			_windowY = value;
			break;
		case 0xb:
			_windowX = value;
			break;
    }
}

void Gpu::renderScanline()
{

	unsigned char scanlineRow[160];

	if (!_enableDisplay)
	{
		int pixelOffset = _scanline * 160;
		for (int i = 0; i < 160; ++i)
			_screenData[i + pixelOffset] = 255;
		return;
	}

	// if bg enabled
	if (_enableBackground)
	{
		int pixelOffset = _scanline * 160;
		int mapOffset = _tileMapBackground;
		mapOffset += (((_scanline + _scrollY) & 255) >> 3) << 5;
		int lineOffset = (_scrollX >> 3) & 0x1F;

		int x = _scrollX & 7;
		int y = (_scanline + _scrollY) & 7;


		unsigned short tile = _vram.at(mapOffset + lineOffset);
		if (_tileSet && tile < 0x80)
			tile += 0x100;

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

	if (_enableWindow)
	{
		if (_windowY <= _scanline)
		{
			int pixelOffset = _scanline * 160 + _windowX - 7;
			int mapOffset = _tileMapWindow;
			mapOffset += (((_scanline - _windowY) & 255) >> 3) << 5;
			int lineOffset = 0;

			int x = (_windowX - 7) & 7;
			int y = _scanline & 7;

			unsigned short tile = _vram.at(mapOffset + lineOffset);
			if (_tileSet && tile < 0x80)
				tile += 0x100;

			for (int i = _windowX - 7; i < 160; ++i)
			{
				auto color = _tiles.at(tile).at(y).at(x);
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
	}

	if (_enableSprite)
	{
		unsigned char sprite_height = _spriteVDouble ? 16 : 8;
		unsigned char sprite_width = 8;

		std::vector<SpriteData> sprites(_spriteData.begin(), _spriteData.end());
		std::sort(sprites.begin(), sprites.end());

		for (int i = 0; i < 40; ++i)
		{
			const auto& sprite = sprites.at(i);
			if (sprite.x == 0 && sprite.y == 0)
				continue;

			auto sx = sprite.x - 8;
			auto sy = sprite.y - 16;

			if (sy <= _scanline && (sy + sprite_height) > _scanline)
			{
				const auto& pal = sprite.flags.bits.paletteNum ? _paletteObj1 : _paletteObj0;

				auto pixelOffset = _scanline * 160 + sx;
				auto tileRow = (uint8_t)(_scanline - sy);
				if (sprite.flags.bits.yFlip)
					tileRow = sprite_height - tileRow - 1;

				unsigned char tile = sprite.patternNum & (_spriteVDouble ? 0xfe : 0xff);
				if (tileRow & 8)
					tile++;
				tileRow = tileRow & 7;

				for (int x = 0; x < sprite_width; ++x)
				{
					if ((sx + x) >= 0 && (sx + x < 160) && (!sprite.flags.bits.prio || !scanlineRow[sx + x]))
					{
						unsigned char color;
						if (sprite.flags.bits.xFlip)
							color = _tiles.at(tile).at(tileRow).at(7 - x);
						else
							color = _tiles.at(tile).at(tileRow).at(x);

						if (color)
						{
							_screenData[pixelOffset] = pal.at(color);
						}
					}
					++pixelOffset;
				}
			}
		}
	}
}