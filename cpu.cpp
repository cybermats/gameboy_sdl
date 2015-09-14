#include "cpu.h"


void Cpu::printCode(size_t numCmds)
{
    auto localPc = pc;
    for(size_t i = 0; i < numCmds; ++i)
    {
        auto opcode = _mbc->readByte(localPc);
        auto opitem = opmap[opcode];
        bool cb = opcode == 0xcb;
        if(cb)
        {
            opcode = _mbc->readByte(localPc+1);
            opitem = cbopmap[opcode];
        }
        std::cout << std::hex ;
        std::cout
                << "[" << ((opitem.func != &Cpu::NOP) ? "X" : " ") << "] "
                << "[" << std::setfill('0') << std::setw(4) << localPc << "] "
                << "[" << std::setfill('0') << std::setw(2) << (int)opcode << "] "
                << opitem.name << " ";
        for(auto j = 1; j < opitem.size; ++j)
            std::cout << std::setfill('0') << std::setw(2) << (int)_mbc->readByte(localPc + j) << " ";
        std::cout << std::endl;
        localPc += opitem.size;
        if(cb)
            localPc++;
    }

}

void Cpu::printStep()
{
    std::cout << " A ZNHC  BC   DE   HL   SP   PC  Im   Op  Mne" << std::endl;
    std::cout << std::hex
            << std::setfill('0') << std::setw(2)
            << (int)r.a
            << " "
            << ((r.f & Z_FLAG)?"1":"0")
            << ((r.f & N_FLAG)?"1":"0")
            << ((r.f & H_FLAG)?"1":"0")
            << ((r.f & C_FLAG)?"1":"0")
            << " "
            << std::setfill('0') << std::setw(4)
            << r.bc << " "
            << std::setfill('0') << std::setw(4)
            << r.de << " "
            << std::setfill('0') << std::setw(4)
            << r.hl << " "
            << std::setfill('0') << std::setw(4)
            << sp << " "
            << std::setfill('0') << std::setw(4)
            << pc << " ";

    auto opcode = _mbc->readByte(pc);
    auto opitem = opmap[opcode];
    bool cb = opcode == 0xcb;
    if(cb)
    {
        opcode = _mbc->readByte(pc+1);
        opitem = cbopmap[opcode];
    }
    std::cout << std::hex ;
    std::cout
            << "[" << ((opitem.func != &Cpu::NOP) ? "X" : " ") << "] "
            << "[" << std::setfill('0') << std::setw(2) << (int)opcode << "] "
            << opitem.name << " ";
    for(auto j = 1; j < opitem.size; ++j)
        std::cout << std::setfill('0') << std::setw(2) << (int)_mbc->readByte(pc + j) << " ";
    std::cout << std::endl;

}
