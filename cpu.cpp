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
            << (int)af.b.h
            << " "
            << ((af.b.l & Z_FLAG)?"1":"0")
            << ((af.b.l & N_FLAG)?"1":"0")
            << ((af.b.l & H_FLAG)?"1":"0")
            << ((af.b.l & C_FLAG)?"1":"0")
            << " "
            << std::setfill('0') << std::setw(4)
            << bc.w << " "
            << std::setfill('0') << std::setw(4)
            << de.w << " "
            << std::setfill('0') << std::setw(4)
            << hl.w << " "
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
