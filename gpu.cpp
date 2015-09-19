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
  , _tilemap(512, std::vector<std::vector<uint8_t>>(8, std::vector<uint8_t>(8, 0)))
, _screenData(160*144, 255)
, _oam(0xA0, 0)
, _currline(0)
, _currscan(0)
, _linemode(0)
, _modeclocks(0)
, _yscrl(0)
, _xscrl(0)
, _raster(0)
, _lcdOn(false)
, _bgOn(false)
, _spriteOn(false)
, _spriteSize(0)
, _scanrow(160, 0)
, _spriteData(40)
, _bgtilebase(0x0000)
, _bgmapbase(0x1800)
, _hasImage(false)
{}


void Gpu::checkline(uint64_t clockticks)
{
    _modeclocks += clockticks;
    switch (_linemode)
    {
        // In HBlank
        case 0:
            if(_modeclocks >= 51)
            {
                // End of H Blank for last line. Render screen.
                if(_currline == 143) {
                    _linemode = 1;
                    renderimage();
					_interrupts->flagInterrupt(Interrupts::Vblank);
                }
                else {
                    _linemode = 2;
                }
                _modeclocks -= 51;
                _currline++;
                _currscan +=160;
            }
            break;
        // In VBlank
        case 1:
            if(_modeclocks >= 114) {
                _modeclocks -= 114;
                _currline++;
                if (_currline > 153) {
                    _currline = 0;
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
                if(_lcdOn)
                {
                    if(_bgOn)
                    {
                        size_t linebase = _currscan;
                        auto mapbase = _bgmapbase + ((((_currline + _yscrl)&255)>>3)<<5);
                        uint8_t y = (_currline + _yscrl)&7;
                        uint8_t x = _xscrl &7;
                        uint8_t t = (_xscrl >>3)&31;
                        uint8_t w=160;
                        if(_bgtilebase)
                        {
                            uint8_t tile = _vram.at(mapbase+t);
                            if(tile<128) tile=256+tile;
                            auto tilerow = _tilemap.at(tile).at(y);
                            do
                            {
                                _scanrow.at(160-w) = tilerow.at(x);
                                assert(linebase < _screenData.size());
                                _screenData.at(linebase) = _paletteBg.at(tilerow.at(x));
                                x++;
                                if(x==8) { t=(t+1)&31; x=0; tile=_vram.at(mapbase+t); if(tile<128) tile=256+tile; tilerow = _tilemap.at(tile).at(y); }
                                linebase+=1;
                            } while(--w);
                        }
                        else
                        {
							auto adress = _vram.at(mapbase + t);

                            auto tilerow=_tilemap.at(_vram.at(mapbase+t)).at(y);
                            do
                            {
                                auto tmp = tilerow.at(x);
                                _scanrow.at(160-w) = tmp;
                                assert(linebase < _screenData.size());
                                _screenData.at(linebase) = _paletteBg.at(tmp);
                                x++;
                                if(x==8) 
								{ 
									t=(t+1)&31; 
									x=0; 
									tilerow=_tilemap.at(_vram.at(mapbase+t)).at(y); 
								}
                                linebase+=1;
                            } while(--w);
                        }
                    }
                    if(_spriteOn)
                    {
                        uint8_t cnt = 0;
                        std::vector<uint8_t> tilerow;
                        std::vector<uint8_t> pal;
                        uint8_t x;
                        size_t linebase = _currscan;
                        for(auto i=0; i<40; i++)
                        {
                            auto& obj = _spriteData.at(i);
                            if((obj.y <= _currline) && (((uint32_t)(obj.y+8)) > _currline))
                            {
                                if(obj.flags.bits.yFlip)
                                    tilerow = _tilemap.at(obj.patternNum).at(7-(_currline -obj.y));
                                else
                                    tilerow = _tilemap.at(obj.patternNum).at(_currline -obj.y);

                                if(obj.flags.bits.paletteNum) pal=_paletteObj1;
                                else pal=_paletteObj0;

								linebase = (_currline * 160 + obj.x);
								if (obj.flags.bits.xFlip)
                                {
                                    for(x=0; x<8; x++)
                                    {
                                        if(obj.x+x >=0 && obj.x+x < 160)
                                        {
                                            if(tilerow.at(7-x) && (obj.flags.bits.prio || !_scanrow.at(x)))
                                            {
                                                _screenData.at(linebase) = pal.at(tilerow.at(7-x));
                                            }
                                        }
                                        linebase+=1;
                                    }
                                }
                                else
                                {
                                    for(x=0; x<8; x++)
                                    {
                                        if(obj.x+x >=0 && obj.x+x < 160)
                                        {
                                            if(tilerow.at(x) && (obj.flags.bits.prio || !_scanrow.at(x)))
                                            {
                                                _screenData.at(linebase) = pal.at(tilerow.at(x));
                                            }
                                        }
                                        linebase+=1;
                                    }
                                }
                                cnt++; if(cnt>10) break;
                            }
                        }
                    }
                }
            }
            break;
            // In HBlank
    }
}

void Gpu::writeIO(unsigned short addr, unsigned char value)
{
    auto gaddr = addr - 0xFF40;
    _regs.at(gaddr) = value;
    switch(gaddr)
    {
        case 0:
            _lcdOn = (value&0x80)?1:0;
            _bgtilebase = (value&0x10)?0x0000:0x0800;
            _bgmapbase = (value&0x08)?0x1C00:0x1800;
            _spriteSize = (value&0x04)?1:0;
            _spriteOn = (value&0x02)?1:0;
            _bgOn = (value&0x01)?1:0;
            break;

        case 2:
            _yscrl = value;
            break;

        case 3:
            _xscrl = value;
            break;

        case 5:
            _raster = value;

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
