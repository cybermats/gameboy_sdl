#pragma once

#include "mmu.h"

#include <stdint.h>
#include <iostream>
#include <iomanip>

#define SIGNED_ADD(a, b) (SET_FLAG((((a + b) ^ a) >> 4) & C_FLAG), SET_FLAG((((a + b) ^ a) << 1) & H_FLAG), a + b)
#define ADD_WITH_FLAGS(t1, t2, s) ()

#define Z_FLAG 0x80 // Zero
#define N_FLAG 0x40 // Subtract flag
#define H_FLAG 0x20 // Half-carry flag
#define C_FLAG 0x10 // Carry flag
#define ALL_FLAGS 0xF0

#define SET_FLAG(flag) af.b.l |= (flag)
#define RESET_FLAG(flag) af.b.l &= (0xFF ^ (flag))


class Cpu {
private:
    using z80Reg = union {
        uint16_t w;
        struct {
            uint8_t l, h;
        } b;
    };
public:

    Cpu(MMU* mbc)
        : _mbc(mbc)
        , pc(0)
        , sp(0)
        , m(0)
    {
        af.w = 0;
        bc.w = 0;
        de.w = 0;
        hl.w = 0;

    }

    void execute()
    {
        // Fetch
        uint8_t opcode = _mbc->readByte(pc++);
        // Decode
        (this->*opmap[opcode].func)();
    }

    void printCode(size_t numCmds)
    {
        std::cout << " PC   OP  " << std::endl;
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
                    << "[" << std::setfill('0') << std::setw(2) << localPc << "] "
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


private:

    void decode();


    MMU* _mbc;

    uint16_t pc;
    uint16_t sp;

    z80Reg af;
    z80Reg bc;
    z80Reg de;
    z80Reg hl;

    // Clock
    uint64_t m;

    typedef void(Cpu::*opfunc_t)();

    struct opinfo_t {
        opfunc_t func;
        const char* name;
        unsigned char size;
    };

    void NOP() { m = 4; }

    // Load/Store

    void LDrr_bb() { bc.b.h = bc.b.h; m = 1; }
    void LDrr_bc() { bc.b.h = bc.b.l; m = 1; }
    void LDrr_bd() { bc.b.h = de.b.h; m = 1; }
    void LDrr_be() { bc.b.h = de.b.l; m = 1; }
    void LDrr_bh() { bc.b.h = hl.b.h; m = 1; }
    void LDrr_bl() { bc.b.h = hl.b.l; m = 1; }
    void LDrr_ba() { bc.b.h = af.b.h; m = 1; }
    void LDrr_cb() { bc.b.l = bc.b.h; m = 1; }
    void LDrr_cc() { bc.b.l = bc.b.l; m = 1; }
    void LDrr_cd() { bc.b.l = de.b.h; m = 1; }
    void LDrr_ce() { bc.b.l = de.b.l; m = 1; }
    void LDrr_ch() { bc.b.l = hl.b.h; m = 1; }
    void LDrr_cl() { bc.b.l = hl.b.l; m = 1; }
    void LDrr_ca() { bc.b.l = af.b.h; m = 1; }
    void LDrr_db() { de.b.h = bc.b.h; m = 1; }
    void LDrr_dc() { de.b.h = bc.b.l; m = 1; }
    void LDrr_dd() { de.b.h = de.b.h; m = 1; }
    void LDrr_de() { de.b.h = de.b.l; m = 1; }
    void LDrr_dh() { de.b.h = hl.b.h; m = 1; }
    void LDrr_dl() { de.b.h = hl.b.l; m = 1; }
    void LDrr_da() { de.b.h = af.b.h; m = 1; }
    void LDrr_eb() { de.b.l = bc.b.h; m = 1; }
    void LDrr_ec() { de.b.l = bc.b.l; m = 1; }
    void LDrr_ed() { de.b.l = de.b.h; m = 1; }
    void LDrr_ee() { de.b.l = de.b.l; m = 1; }
    void LDrr_eh() { de.b.l = hl.b.h; m = 1; }
    void LDrr_el() { de.b.l = hl.b.l; m = 1; }
    void LDrr_ea() { de.b.l = af.b.h; m = 1; }
    void LDrr_hb() { hl.b.h = bc.b.h; m = 1; }
    void LDrr_hc() { hl.b.h = bc.b.l; m = 1; }
    void LDrr_hd() { hl.b.h = de.b.h; m = 1; }
    void LDrr_he() { hl.b.h = de.b.l; m = 1; }
    void LDrr_hh() { hl.b.h = hl.b.h; m = 1; }
    void LDrr_hl() { hl.b.h = hl.b.l; m = 1; }
    void LDrr_ha() { hl.b.h = af.b.h; m = 1; }
    void LDrr_lb() { hl.b.l = bc.b.h; m = 1; }
    void LDrr_lc() { hl.b.l = bc.b.l; m = 1; }
    void LDrr_ld() { hl.b.l = de.b.h; m = 1; }
    void LDrr_le() { hl.b.l = de.b.l; m = 1; }
    void LDrr_lh() { hl.b.l = hl.b.h; m = 1; }
    void LDrr_ll() { hl.b.l = hl.b.l; m = 1; }
    void LDrr_la() { hl.b.l = af.b.h; m = 1; }
    void LDrr_ab() { af.b.h = bc.b.h; m = 1; }
    void LDrr_ac() { af.b.h = bc.b.l; m = 1; }
    void LDrr_ad() { af.b.h = de.b.h; m = 1; }
    void LDrr_ae() { af.b.h = de.b.l; m = 1; }
    void LDrr_ah() { af.b.h = hl.b.h; m = 1; }
    void LDrr_al() { af.b.h = hl.b.l; m = 1; }
    void LDrr_aa() { af.b.h = af.b.h; m = 1; }

    void LDrHLm_b() { bc.b.h = _mbc->readByte(hl.w); m = 2; }
    void LDrHLm_c() { bc.b.l = _mbc->readByte(hl.w); m = 2; }
    void LDrHLm_d() { de.b.h = _mbc->readByte(hl.w); m = 2; }
    void LDrHLm_e() { de.b.l = _mbc->readByte(hl.w); m = 2; }
    void LDrHLm_h() { hl.b.h = _mbc->readByte(hl.w); m = 2; }
    void LDrHLm_l() { hl.b.l = _mbc->readByte(hl.w); m = 2; }
    void LDrHLm_a() { af.b.h = _mbc->readByte(hl.w); m = 2; }

    void LDHLmr_b() { _mbc->writeByte(hl.w, bc.b.h); m = 2; }
    void LDHLmr_c() { _mbc->writeByte(hl.w, bc.b.l); m = 2; }
    void LDHLmr_d() { _mbc->writeByte(hl.w, de.b.h); m = 2; }
    void LDHLmr_e() { _mbc->writeByte(hl.w, de.b.l); m = 2; }
    void LDHLmr_h() { _mbc->writeByte(hl.w, hl.b.h); m = 2; }
    void LDHLmr_l() { _mbc->writeByte(hl.w, hl.b.l); m = 2; }
    void LDHLmr_a() { _mbc->writeByte(hl.w, af.b.h); m = 2; }

    void LDrn_b() { bc.b.h = _mbc->readByte(pc++); m = 2; }
    void LDrn_c() { bc.b.l = _mbc->readByte(pc++); m = 2; }
    void LDrn_d() { de.b.h = _mbc->readByte(pc++); m = 2; }
    void LDrn_e() { de.b.l = _mbc->readByte(pc++); m = 2; }
    void LDrn_h() { hl.b.h = _mbc->readByte(pc++); m = 2; }
    void LDrn_l() { hl.b.l = _mbc->readByte(pc++); m = 2; }
    void LDrn_a() { af.b.h = _mbc->readByte(pc++); m = 2; }

    void LDHLmn() { _mbc->writeByte(hl.w, _mbc->readByte(pc++)); m = 3; }

    void LDBCmA() { _mbc->writeByte(bc.w, af.b.h); m = 2; }
    void LDDEmA() { _mbc->writeByte(de.w, af.b.h); m = 2; }

    void LDmmA() { _mbc->writeByte(_mbc->readByte(pc++) + ((unsigned short)_mbc->readByte(pc++) << 8), af.b.h); m = 4; }

    void LDABCm() { af.b.h = _mbc->readByte(bc.w); m = 2; }
    void LDADEm() { af.b.h = _mbc->readByte(de.w); m = 2; }

    void LDAmm() { af.b.h = _mbc->readByte( _mbc->readByte(pc++) + (unsigned short)(_mbc->readByte(pc++) << 8)); m = 4; }

    void LDBCnn() { bc.b.l = _mbc->readByte(pc++); bc.b.h = _mbc->readByte(pc++); m = 3; }
    void LDDEnn() { de.b.l = _mbc->readByte(pc++); de.b.h = _mbc->readByte(pc++); m = 3; }
    void LDHLnn() { hl.b.l = _mbc->readByte(pc++); hl.b.h = _mbc->readByte(pc++); m = 3; }
    void LDSPnn() { sp = _mbc->readByte(pc++) + (unsigned short)(_mbc->readByte(pc++) << 8); m = 3; }

    void LDHLIA() { _mbc->writeByte(hl.w, af.b.h); hl.w++; m = 2; }
    void LDAHLI() { af.b.h = _mbc->readByte(hl.w); hl.w++; m = 2; }

    void LDHLDA() { _mbc->writeByte(hl.w, af.b.h); hl.w--; m = 2; }
    void LDAHLD() { af.b.h = _mbc->readByte(hl.w); hl.w--; m = 2; }

    void LDAIOn() { af.b.h = _mbc->readByte(0xFF00 + _mbc->readByte(pc++)); m = 3; }
    void LDIOnA() { _mbc->writeByte(0xFF00 + _mbc->readByte(pc++), af.b.h); m = 3; }
    void LDAIOC() { af.b.h = _mbc->readByte(0xFF00 + bc.b.l); m = 2; }
    void LDIOCA() { _mbc->writeByte(0xFF00 + bc.b.l, af.b.h); m = 2; }

    void LDHLSPn() { char c = (char)_mbc->readByte(pc++); hl.w = SIGNED_ADD(sp, c); m = 3; RESET_FLAG(Z_FLAG | N_FLAG); }

    void SWAPr_b() { auto v = bc.b.h; bc.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_c() { auto v = bc.b.l; bc.b.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_d() { auto v = de.b.h; de.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_e() { auto v = de.b.l; de.b.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_h() { auto v = hl.b.h; hl.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_l() { auto v = hl.b.l; hl.b.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_a() { auto v = af.b.h; af.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }

    // Data processing

    void ADDr_b() { auto a = af.b.h; af.b.h +=  }

    static opinfo_t opmap[];
    static opinfo_t cbopmap[];

};