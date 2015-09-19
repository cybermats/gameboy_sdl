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

void Cpu::printStep(std::ostream& os)
{
    os << " AF  ZNHC  BC   DE   HL   SP   PC   Op  IME  IE   IF  Mne" << std::endl;
    os << std::hex
            << std::setfill('0') << std::setw(4)
            << (int)r.af
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
    os << std::hex ;
    os
            << "[" << std::setfill('0') << std::setw(2) << (int)opcode << "] "
			<< "[" << ((_interrupts->readIME()) ? "E" : " ") << "] "
			<< "[" << std::setfill('0')  << std::setw(2) << (int)_interrupts->readIE() << "] "
			<< "[" << std::setfill('0')  << std::setw(2) << (int)_interrupts->readIF() << "] "
            << opitem.name << " ";

    for(auto j = 1; j < opitem.size; ++j)
        os << std::setfill('0') << std::setw(2) << (int)_mbc->readByte(pc + j) << " ";
    os << std::endl;

}
