#pragma once

#include "cpu.h"
#include "gpu.h"
#include "mmu.h"
#include "mbc.h"
#include "cartridge.h"

#include <memory>
#include <iostream>
#include <iomanip>

class Gbu
{
public:
    Gbu(const char* filename)
    {
        Cartridge cartridge(filename);
        _mbc = cartridge.getMBC();
        _mmu = std::unique_ptr<MMU>(new MMU(_mbc.get()));
        _gpu = std::unique_ptr<Gpu>(new Gpu(_mmu.get()));
        _mmu->setGpu(_gpu.get());
        _cpu = std::unique_ptr<Cpu>(new Cpu(_mmu.get()));
    }

    void runFrame()
    {
        while(!_gpu->hasImage())
        {
            _cpu->execute();
            _gpu->checkline(_cpu->m);
        }
    }

    std::vector<uint8_t>& getBuffer()
    {
        return _gpu->getFrame();
    }


private:
    std::unique_ptr<IMBC> _mbc;
    std::unique_ptr<MMU> _mmu;
    std::unique_ptr<Gpu> _gpu;
    std::unique_ptr<Cpu> _cpu;
};