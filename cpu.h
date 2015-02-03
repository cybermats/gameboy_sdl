#pragma once

#include "mmu.h"

#include <stdint.h>
#include <iostream>
#include <iomanip>

#define SIGNED_ADD_WITH_FLAGS(t1, t2, s) RESET_FLAG(ALL_FLAGS); SET_FLAG(((s ^ t1) >> 4) & C_FLAG), SET_FLAG(((s ^ t1) << 1) & H_FLAG)

#define COMPUTE_CARRY_FLAGS(Term1, Term2, Sum, BitSize) \
    RESET_FLAG(ALL_FLAGS); \
    if(!Sum) SET_FLAG(Z_FLAG); \
    auto val = (Sum ^ Term1 ^ Term2); \
    auto c = (val & (1 << BitSize)) >> (BitSize - 5); \
    auto h = val & (1 << (BitSize - 4)) >> (BitSize - 4); \
    SET_FLAG(c | h);

#define INC_REG(Reg) \
    RESET_FLAG(Z_FLAG | H_FLAG | N_FLAG); \
    auto v = Reg++; \
    if(!Reg) SET_FLAG(Z_FLAG); \
    if((v ^ Reg) & 0x10) SET_FLAG(H_FLAG);

#define DEC_REG(Reg) \
    RESET_FLAG(Z_FLAG | H_FLAG); \
    SET_FLAG(N_FLAG); \
    auto v = Reg--; \
    if(!Reg) SET_FLAG(Z_FLAG); \
    if((v ^ Reg) & 0x10) SET_FLAG(H_FLAG);

#define BIT_CHECK(Reg, Bit) \
    RESET_FLAG(N_FLAG | Z_FLAG); \
    SET_FLAG(H_FLAG); \
    af.b.l |= ((Reg & (1 << Bit)) >> Bit) << 7;


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

    void LDHLSPn() { char c = (char)_mbc->readByte(pc++); int s = sp + c; hl.w = s; SIGNED_ADD_WITH_FLAGS(sp, c, s); m = 3; }

    void SWAPr_b() { auto v = bc.b.h; bc.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_c() { auto v = bc.b.l; bc.b.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_d() { auto v = de.b.h; de.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_e() { auto v = de.b.l; de.b.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_h() { auto v = hl.b.h; hl.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_l() { auto v = hl.b.l; hl.b.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
    void SWAPr_a() { auto v = af.b.h; af.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 1; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }

    // Data processing

    void ADDr_b() { unsigned short a = af.b.h; af.b.h += bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, af.b.h, 8); m = 1; }
    void ADDr_c() { unsigned short a = af.b.h; af.b.h += bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, af.b.h, 8); m = 1; }
    void ADDr_d() { unsigned short a = af.b.h; af.b.h += de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, af.b.h, 8); m = 1; }
    void ADDr_e() { unsigned short a = af.b.h; af.b.h += de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, af.b.h, 8); m = 1; }
    void ADDr_h() { unsigned short a = af.b.h; af.b.h += hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, af.b.h, 8); m = 1; }
    void ADDr_l() { unsigned short a = af.b.h; af.b.h += hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, af.b.h, 8); m = 1; }
    void ADDr_a() { unsigned short a = af.b.h; af.b.h += af.b.h; COMPUTE_CARRY_FLAGS(a, a, af.b.h, 8); m = 1; }
    void ADDHL() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(hl.w); af.b.h += v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); m = 2; }
    void ADDn() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(pc++); af.b.h += v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); m = 2; }
    void ADDHLBC() { unsigned int rhl = hl.w; hl.w += bc.w; COMPUTE_CARRY_FLAGS(rhl, bc.w, hl.w, 16); m = 2; }
    void ADDHLDE() { unsigned int rhl = hl.w; hl.w += de.w; COMPUTE_CARRY_FLAGS(rhl, de.w, hl.w, 16); m = 2; }
    void ADDHLHL() { unsigned int rhl = hl.w; hl.w += hl.w; COMPUTE_CARRY_FLAGS(rhl, rhl, hl.w, 16); m = 2; }
    void ADDHLSP() { unsigned int rhl = hl.w; hl.w += sp; COMPUTE_CARRY_FLAGS(rhl, sp, hl.w, 16); m = 2; }
    void ADDSPn() { unsigned int rsp = sp; char v = _mbc->readByte(pc++); sp += v; COMPUTE_CARRY_FLAGS(rsp, v, sp, 16); m = 4; }

    void ADCr_b() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, af.b.h, 8); m = 1; }
    void ADCr_c() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, af.b.h, 8); m = 1; }
    void ADCr_d() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, af.b.h, 8); m = 1; }
    void ADCr_e() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, af.b.h, 8); m = 1; }
    void ADCr_h() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, af.b.h, 8); m = 1; }
    void ADCr_l() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, af.b.h, 8); m = 1; }
    void ADCr_a() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + af.b.h; COMPUTE_CARRY_FLAGS(a, af.b.h, af.b.h, 8); m = 1; }
    void ADCHL() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); unsigned short v = _mbc->readByte(hl.w); af.b.h = a + v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); m = 2; }
    void ADCn() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); unsigned short v = _mbc->readByte(pc++); af.b.h = a + v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); m = 2; }

    void SUBr_b() { unsigned short a = af.b.h; af.b.h -= bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SUBr_c() { unsigned short a = af.b.h; af.b.h -= bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SUBr_d() { unsigned short a = af.b.h; af.b.h -= de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SUBr_e() { unsigned short a = af.b.h; af.b.h -= de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SUBr_h() { unsigned short a = af.b.h; af.b.h -= hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SUBr_l() { unsigned short a = af.b.h; af.b.h -= hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SUBr_a() { unsigned short a = af.b.h; af.b.h -= af.b.h; COMPUTE_CARRY_FLAGS(a, a, af.b.h, 8); SET_FLAG(Z_FLAG); m = 1; }
    void SUBHL() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(hl.w); af.b.h -= v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); SET_FLAG(N_FLAG); m = 2; }
    void SUBn() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(pc++); af.b.h -= v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); SET_FLAG(N_FLAG); m = 2; }

    void SBCr_b() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SBCr_c() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SBCr_d() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SBCr_e() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SBCr_h() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SBCr_l() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SBCr_a() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - af.b.h; COMPUTE_CARRY_FLAGS(a, af.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
    void SBCHL() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); unsigned short v = _mbc->readByte(hl.w); af.b.h = a - v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); SET_FLAG(N_FLAG); m = 2; }
    void SBCn() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); unsigned short v = _mbc->readByte(pc++); af.b.h = a - v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); SET_FLAG(N_FLAG); m = 2; }

    void CPr_b() { unsigned short a = af.b.h; unsigned short s = a - bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, s, 8); SET_FLAG(N_FLAG); m = 1; }
    void CPr_c() { unsigned short a = af.b.h; unsigned short s = a - bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, s, 8); SET_FLAG(N_FLAG); m = 1; }
    void CPr_d() { unsigned short a = af.b.h; unsigned short s = a - de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, s, 8); SET_FLAG(N_FLAG); m = 1; }
    void CPr_e() { unsigned short a = af.b.h; unsigned short s = a - de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, s, 8); SET_FLAG(N_FLAG); m = 1; }
    void CPr_h() { unsigned short a = af.b.h; unsigned short s = a - hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, s, 8); SET_FLAG(N_FLAG); m = 1; }
    void CPr_l() { unsigned short a = af.b.h; unsigned short s = a - hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, s, 8); SET_FLAG(N_FLAG); m = 1; }
    void CPr_a() { unsigned short a = af.b.h; unsigned short s = a - af.b.h; COMPUTE_CARRY_FLAGS(a, a, s, 8); SET_FLAG(Z_FLAG); m = 1; }
    void CPHL() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(hl.w); unsigned short s = a - v; COMPUTE_CARRY_FLAGS(a, v, s, 8); SET_FLAG(N_FLAG); m = 2; }
    void CPn() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(pc++); unsigned short s = a - v; COMPUTE_CARRY_FLAGS(a, v, s, 8); SET_FLAG(N_FLAG); m = 2; }

    void ANDr_b() { af.b.h &= bc.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
    void ANDr_c() { af.b.h &= bc.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
    void ANDr_d() { af.b.h &= de.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
    void ANDr_e() { af.b.h &= de.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
    void ANDr_h() { af.b.h &= hl.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
    void ANDr_l() { af.b.h &= hl.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
    void ANDr_a() { af.b.h &= af.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
    void ANDHL() { af.b.h &= _mbc->readByte(hl.w); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 2; }
    void ANDn() { af.b.h &= _mbc->readByte(pc++); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 2; }

    void ORr_b() { af.b.h |= bc.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void ORr_c() { af.b.h |= bc.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void ORr_d() { af.b.h |= de.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void ORr_e() { af.b.h |= de.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void ORr_h() { af.b.h |= hl.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void ORr_l() { af.b.h |= hl.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void ORr_a() { af.b.h |= af.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void ORHL() { af.b.h |= _mbc->readByte(hl.w); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }
    void ORn() { af.b.h |= _mbc->readByte(pc++); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }

    void XORr_b() { af.b.h ^= bc.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void XORr_c() { af.b.h ^= bc.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void XORr_d() { af.b.h ^= de.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void XORr_e() { af.b.h ^= de.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void XORr_h() { af.b.h ^= hl.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void XORr_l() { af.b.h ^= hl.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void XORr_a() { af.b.h ^= af.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
    void XORHL() { af.b.h ^= _mbc->readByte(hl.w); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }
    void XORn() { af.b.h ^= _mbc->readByte(pc++); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }

    void INCr_b() { INC_REG(bc.b.h); m = 1; }
    void INCr_c() { INC_REG(bc.b.l); m = 1; }
    void INCr_d() { INC_REG(de.b.h); m = 1; }
    void INCr_e() { INC_REG(de.b.l); m = 1; }
    void INCr_h() { INC_REG(hl.b.h); m = 1; }
    void INCr_l() { INC_REG(hl.b.l); m = 1; }
    void INCr_a() { INC_REG(af.b.h); m = 1; }
    void INCHLm() { auto mem = _mbc->readByte(hl.w); INC_REG(mem); _mbc->writeByte(hl.w, mem); m = 3; }

    void DECr_b() { DEC_REG(bc.b.h); m = 1; }
    void DECr_c() { DEC_REG(bc.b.l); m = 1; }
    void DECr_d() { DEC_REG(de.b.h); m = 1; }
    void DECr_e() { DEC_REG(de.b.l); m = 1; }
    void DECr_h() { DEC_REG(hl.b.h); m = 1; }
    void DECr_l() { DEC_REG(hl.b.l); m = 1; }
    void DECr_a() { DEC_REG(af.b.h); m = 1; }
    void DECHLm() { auto mem = _mbc->readByte(hl.w); DEC_REG(mem); _mbc->writeByte(hl.w, mem); m = 3; }

    void INCBC() { bc.w++; m = 2; }
    void INCDE() { de.w++; m = 2; }
    void INCHL() { hl.w++; m = 2; }
    void INCSP() { sp++; m = 2; }

    void DECBC() { bc.w--; m = 2; }
    void DECDE() { de.w--; m = 2; }
    void DECHL() { hl.w--; m = 2; }
    void DECSP() { sp--; m = 2; }

    /* Bit manipulation */
    void BIT0b() { BIT_CHECK(bc.b.h, 0); m = 2; }
    void BIT0c() { BIT_CHECK(bc.b.l, 0); m = 2; }
    void BIT0d() { BIT_CHECK(de.b.h, 0); m = 2; }
    void BIT0e() { BIT_CHECK(de.b.l, 0); m = 2; }
    void BIT0h() { BIT_CHECK(hl.b.h, 0); m = 2; }
    void BIT0l() { BIT_CHECK(hl.b.l, 0); m = 2; }
    void BIT0a() { BIT_CHECK(af.b.h, 0); m = 2; }
    void BIT0m() { BIT_CHECK(_mbc->readByte(hl.w), 0); m = 4; }

    void RES0b() { bc.b.h &= (0xFF ^ (1 << 0)); m = 2; }
    void RES0c() { bc.b.l &= (0xFF ^ (1 << 0)); m = 2; }
    void RES0d() { de.b.h &= (0xFF ^ (1 << 0)); m = 2; }
    void RES0e() { de.b.l &= (0xFF ^ (1 << 0)); m = 2; }
    void RES0h() { hl.b.h &= (0xFF ^ (1 << 0)); m = 2; }
    void RES0l() { hl.b.l &= (0xFF ^ (1 << 0)); m = 2; }
    void RES0a() { af.b.h &= (0xFF ^ (1 << 0)); m = 2; }
    void RES0m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 0)); _mbc->writeByte(hl.w, mem); m = 4; }

    void SET0b() { bc.b.h |= (1 << 0); m = 2; }
    void SET0c() { bc.b.l |= (1 << 0); m = 2; }
    void SET0d() { de.b.h |= (1 << 0); m = 2; }
    void SET0e() { de.b.l |= (1 << 0); m = 2; }
    void SET0h() { hl.b.h |= (1 << 0); m = 2; }
    void SET0l() { hl.b.l |= (1 << 0); m = 2; }
    void SET0a() { af.b.h |= (1 << 0); m = 2; }
    void SET0m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 0); _mbc->writeByte(hl.w, mem); m = 4; }

    void BIT1b() { BIT_CHECK(bc.b.h, 1); m = 2; }
    void BIT1c() { BIT_CHECK(bc.b.l, 1); m = 2; }
    void BIT1d() { BIT_CHECK(de.b.h, 1); m = 2; }
    void BIT1e() { BIT_CHECK(de.b.l, 1); m = 2; }
    void BIT1h() { BIT_CHECK(hl.b.h, 1); m = 2; }
    void BIT1l() { BIT_CHECK(hl.b.l, 1); m = 2; }
    void BIT1a() { BIT_CHECK(af.b.h, 1); m = 2; }
    void BIT1m() { BIT_CHECK(_mbc->readByte(hl.w), 1); m = 4; }

    void RES1b() { bc.b.h &= (0xFF ^ (1 << 1)); m = 2; }
    void RES1c() { bc.b.l &= (0xFF ^ (1 << 1)); m = 2; }
    void RES1d() { de.b.h &= (0xFF ^ (1 << 1)); m = 2; }
    void RES1e() { de.b.l &= (0xFF ^ (1 << 1)); m = 2; }
    void RES1h() { hl.b.h &= (0xFF ^ (1 << 1)); m = 2; }
    void RES1l() { hl.b.l &= (0xFF ^ (1 << 1)); m = 2; }
    void RES1a() { af.b.h &= (0xFF ^ (1 << 1)); m = 2; }
    void RES1m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 1)); _mbc->writeByte(hl.w, mem); m = 4; }

    void SET1b() { bc.b.h |= (1 << 1); m = 2; }
    void SET1c() { bc.b.l |= (1 << 1); m = 2; }
    void SET1d() { de.b.h |= (1 << 1); m = 2; }
    void SET1e() { de.b.l |= (1 << 1); m = 2; }
    void SET1h() { hl.b.h |= (1 << 1); m = 2; }
    void SET1l() { hl.b.l |= (1 << 1); m = 2; }
    void SET1a() { af.b.h |= (1 << 1); m = 2; }
    void SET1m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 1); _mbc->writeByte(hl.w, mem); m = 4; }

    void BIT2b() { BIT_CHECK(bc.b.h, 2); m = 2; }
    void BIT2c() { BIT_CHECK(bc.b.l, 2); m = 2; }
    void BIT2d() { BIT_CHECK(de.b.h, 2); m = 2; }
    void BIT2e() { BIT_CHECK(de.b.l, 2); m = 2; }
    void BIT2h() { BIT_CHECK(hl.b.h, 2); m = 2; }
    void BIT2l() { BIT_CHECK(hl.b.l, 2); m = 2; }
    void BIT2a() { BIT_CHECK(af.b.h, 2); m = 2; }
    void BIT2m() { BIT_CHECK(_mbc->readByte(hl.w), 2); m = 4; }

    void RES2b() { bc.b.h &= (0xFF ^ (1 << 2)); m = 2; }
    void RES2c() { bc.b.l &= (0xFF ^ (1 << 2)); m = 2; }
    void RES2d() { de.b.h &= (0xFF ^ (1 << 2)); m = 2; }
    void RES2e() { de.b.l &= (0xFF ^ (1 << 2)); m = 2; }
    void RES2h() { hl.b.h &= (0xFF ^ (1 << 2)); m = 2; }
    void RES2l() { hl.b.l &= (0xFF ^ (1 << 2)); m = 2; }
    void RES2a() { af.b.h &= (0xFF ^ (1 << 2)); m = 2; }
    void RES2m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 2)); _mbc->writeByte(hl.w, mem); m = 4; }

    void SET2b() { bc.b.h |= (1 << 2); m = 2; }
    void SET2c() { bc.b.l |= (1 << 2); m = 2; }
    void SET2d() { de.b.h |= (1 << 2); m = 2; }
    void SET2e() { de.b.l |= (1 << 2); m = 2; }
    void SET2h() { hl.b.h |= (1 << 2); m = 2; }
    void SET2l() { hl.b.l |= (1 << 2); m = 2; }
    void SET2a() { af.b.h |= (1 << 2); m = 2; }
    void SET2m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 2); _mbc->writeByte(hl.w, mem); m = 4; }

    void BIT3b() { BIT_CHECK(bc.b.h, 3); m = 2; }
    void BIT3c() { BIT_CHECK(bc.b.l, 3); m = 2; }
    void BIT3d() { BIT_CHECK(de.b.h, 3); m = 2; }
    void BIT3e() { BIT_CHECK(de.b.l, 3); m = 2; }
    void BIT3h() { BIT_CHECK(hl.b.h, 3); m = 2; }
    void BIT3l() { BIT_CHECK(hl.b.l, 3); m = 2; }
    void BIT3a() { BIT_CHECK(af.b.h, 3); m = 2; }
    void BIT3m() { BIT_CHECK(_mbc->readByte(hl.w), 3); m = 4; }

    void RES3b() { bc.b.h &= (0xFF ^ (1 << 3)); m = 2; }
    void RES3c() { bc.b.l &= (0xFF ^ (1 << 3)); m = 2; }
    void RES3d() { de.b.h &= (0xFF ^ (1 << 3)); m = 2; }
    void RES3e() { de.b.l &= (0xFF ^ (1 << 3)); m = 2; }
    void RES3h() { hl.b.h &= (0xFF ^ (1 << 3)); m = 2; }
    void RES3l() { hl.b.l &= (0xFF ^ (1 << 3)); m = 2; }
    void RES3a() { af.b.h &= (0xFF ^ (1 << 3)); m = 2; }
    void RES3m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 3)); _mbc->writeByte(hl.w, mem); m = 4; }

    void SET3b() { bc.b.h |= (1 << 3); m = 2; }
    void SET3c() { bc.b.l |= (1 << 3); m = 2; }
    void SET3d() { de.b.h |= (1 << 3); m = 2; }
    void SET3e() { de.b.l |= (1 << 3); m = 2; }
    void SET3h() { hl.b.h |= (1 << 3); m = 2; }
    void SET3l() { hl.b.l |= (1 << 3); m = 2; }
    void SET3a() { af.b.h |= (1 << 3); m = 2; }
    void SET3m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 3); _mbc->writeByte(hl.w, mem); m = 4; }

    void BIT4b() { BIT_CHECK(bc.b.h, 4); m = 2; }
    void BIT4c() { BIT_CHECK(bc.b.l, 4); m = 2; }
    void BIT4d() { BIT_CHECK(de.b.h, 4); m = 2; }
    void BIT4e() { BIT_CHECK(de.b.l, 4); m = 2; }
    void BIT4h() { BIT_CHECK(hl.b.h, 4); m = 2; }
    void BIT4l() { BIT_CHECK(hl.b.l, 4); m = 2; }
    void BIT4a() { BIT_CHECK(af.b.h, 4); m = 2; }
    void BIT4m() { BIT_CHECK(_mbc->readByte(hl.w), 4); m = 4; }

    void RES4b() { bc.b.h &= (0xFF ^ (1 << 4)); m = 2; }
    void RES4c() { bc.b.l &= (0xFF ^ (1 << 4)); m = 2; }
    void RES4d() { de.b.h &= (0xFF ^ (1 << 4)); m = 2; }
    void RES4e() { de.b.l &= (0xFF ^ (1 << 4)); m = 2; }
    void RES4h() { hl.b.h &= (0xFF ^ (1 << 4)); m = 2; }
    void RES4l() { hl.b.l &= (0xFF ^ (1 << 4)); m = 2; }
    void RES4a() { af.b.h &= (0xFF ^ (1 << 4)); m = 2; }
    void RES4m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 4)); _mbc->writeByte(hl.w, mem); m = 4; }

    void SET4b() { bc.b.h |= (1 << 4); m = 2; }
    void SET4c() { bc.b.l |= (1 << 4); m = 2; }
    void SET4d() { de.b.h |= (1 << 4); m = 2; }
    void SET4e() { de.b.l |= (1 << 4); m = 2; }
    void SET4h() { hl.b.h |= (1 << 4); m = 2; }
    void SET4l() { hl.b.l |= (1 << 4); m = 2; }
    void SET4a() { af.b.h |= (1 << 4); m = 2; }
    void SET4m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 4); _mbc->writeByte(hl.w, mem); m = 4; }

    void BIT5b() { BIT_CHECK(bc.b.h, 5); m = 2; }
    void BIT5c() { BIT_CHECK(bc.b.l, 5); m = 2; }
    void BIT5d() { BIT_CHECK(de.b.h, 5); m = 2; }
    void BIT5e() { BIT_CHECK(de.b.l, 5); m = 2; }
    void BIT5h() { BIT_CHECK(hl.b.h, 5); m = 2; }
    void BIT5l() { BIT_CHECK(hl.b.l, 5); m = 2; }
    void BIT5a() { BIT_CHECK(af.b.h, 5); m = 2; }
    void BIT5m() { BIT_CHECK(_mbc->readByte(hl.w), 5); m = 4; }

    void RES5b() { bc.b.h &= (0xFF ^ (1 << 5)); m = 2; }
    void RES5c() { bc.b.l &= (0xFF ^ (1 << 5)); m = 2; }
    void RES5d() { de.b.h &= (0xFF ^ (1 << 5)); m = 2; }
    void RES5e() { de.b.l &= (0xFF ^ (1 << 5)); m = 2; }
    void RES5h() { hl.b.h &= (0xFF ^ (1 << 5)); m = 2; }
    void RES5l() { hl.b.l &= (0xFF ^ (1 << 5)); m = 2; }
    void RES5a() { af.b.h &= (0xFF ^ (1 << 5)); m = 2; }
    void RES5m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 5)); _mbc->writeByte(hl.w, mem); m = 4; }

    void SET5b() { bc.b.h |= (1 << 5); m = 2; }
    void SET5c() { bc.b.l |= (1 << 5); m = 2; }
    void SET5d() { de.b.h |= (1 << 5); m = 2; }
    void SET5e() { de.b.l |= (1 << 5); m = 2; }
    void SET5h() { hl.b.h |= (1 << 5); m = 2; }
    void SET5l() { hl.b.l |= (1 << 5); m = 2; }
    void SET5a() { af.b.h |= (1 << 5); m = 2; }
    void SET5m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 5); _mbc->writeByte(hl.w, mem); m = 4; }

    void BIT6b() { BIT_CHECK(bc.b.h, 6); m = 2; }
    void BIT6c() { BIT_CHECK(bc.b.l, 6); m = 2; }
    void BIT6d() { BIT_CHECK(de.b.h, 6); m = 2; }
    void BIT6e() { BIT_CHECK(de.b.l, 6); m = 2; }
    void BIT6h() { BIT_CHECK(hl.b.h, 6); m = 2; }
    void BIT6l() { BIT_CHECK(hl.b.l, 6); m = 2; }
    void BIT6a() { BIT_CHECK(af.b.h, 6); m = 2; }
    void BIT6m() { BIT_CHECK(_mbc->readByte(hl.w), 6); m = 4; }

    void RES6b() { bc.b.h &= (0xFF ^ (1 << 6)); m = 2; }
    void RES6c() { bc.b.l &= (0xFF ^ (1 << 6)); m = 2; }
    void RES6d() { de.b.h &= (0xFF ^ (1 << 6)); m = 2; }
    void RES6e() { de.b.l &= (0xFF ^ (1 << 6)); m = 2; }
    void RES6h() { hl.b.h &= (0xFF ^ (1 << 6)); m = 2; }
    void RES6l() { hl.b.l &= (0xFF ^ (1 << 6)); m = 2; }
    void RES6a() { af.b.h &= (0xFF ^ (1 << 6)); m = 2; }
    void RES6m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 6)); _mbc->writeByte(hl.w, mem); m = 4; }

    void SET6b() { bc.b.h |= (1 << 6); m = 2; }
    void SET6c() { bc.b.l |= (1 << 6); m = 2; }
    void SET6d() { de.b.h |= (1 << 6); m = 2; }
    void SET6e() { de.b.l |= (1 << 6); m = 2; }
    void SET6h() { hl.b.h |= (1 << 6); m = 2; }
    void SET6l() { hl.b.l |= (1 << 6); m = 2; }
    void SET6a() { af.b.h |= (1 << 6); m = 2; }
    void SET6m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 6); _mbc->writeByte(hl.w, mem); m = 4; }

    void BIT7b() { BIT_CHECK(bc.b.h, 7); m = 2; }
    void BIT7c() { BIT_CHECK(bc.b.l, 7); m = 2; }
    void BIT7d() { BIT_CHECK(de.b.h, 7); m = 2; }
    void BIT7e() { BIT_CHECK(de.b.l, 7); m = 2; }
    void BIT7h() { BIT_CHECK(hl.b.h, 7); m = 2; }
    void BIT7l() { BIT_CHECK(hl.b.l, 7); m = 2; }
    void BIT7a() { BIT_CHECK(af.b.h, 7); m = 2; }
    void BIT7m() { BIT_CHECK(_mbc->readByte(hl.w), 7); m = 4; }

    void RES7b() { bc.b.h &= (0xFF ^ (1 << 7)); m = 2; }
    void RES7c() { bc.b.l &= (0xFF ^ (1 << 7)); m = 2; }
    void RES7d() { de.b.h &= (0xFF ^ (1 << 7)); m = 2; }
    void RES7e() { de.b.l &= (0xFF ^ (1 << 7)); m = 2; }
    void RES7h() { hl.b.h &= (0xFF ^ (1 << 7)); m = 2; }
    void RES7l() { hl.b.l &= (0xFF ^ (1 << 7)); m = 2; }
    void RES7a() { af.b.h &= (0xFF ^ (1 << 7)); m = 2; }
    void RES7m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 7)); _mbc->writeByte(hl.w, mem); m = 4; }

    void SET7b() { bc.b.h |= (1 << 7); m = 2; }
    void SET7c() { bc.b.l |= (1 << 7); m = 2; }
    void SET7d() { de.b.h |= (1 << 7); m = 2; }
    void SET7e() { de.b.l |= (1 << 7); m = 2; }
    void SET7h() { hl.b.h |= (1 << 7); m = 2; }
    void SET7l() { hl.b.l |= (1 << 7); m = 2; }
    void SET7a() { af.b.h |= (1 << 7); m = 2; }
    void SET7m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 7); _mbc->writeByte(hl.w, mem); m = 4; }

    static opinfo_t opmap[];
    static opinfo_t cbopmap[];

    static unsigned char _add_flags[];
};