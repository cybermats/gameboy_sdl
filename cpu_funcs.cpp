#include "cpu.h"
#include <iostream>

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
    RESET_FLAG(N_FLAG); \
    SET_FLAG(H_FLAG | Z_FLAG); \
    af.b.l &= ~(((Reg & (1 << Bit)) >> Bit) << 7);

#define ROTATE_LEFT_THROUGH_CARRY(Reg) \
    unsigned char c = (af.b.l & 0x10 >> 4); /* Save carry */ \
    af.b.l = (Reg & 0x80) >> 3; /* Set C-flag (and reset all other) */ \
    Reg = (Reg << 1) | c; /* Rotate through carry */ \
    if(!Reg) SET_FLAG(Z_FLAG);

#define ROTATE_LEFT(Reg) \
    af.b.l = (Reg & 0x80) >> 3; /* Set carry (and reset all other) */ \
    Reg = (Reg << 1) | (Reg >> 7); /* Rotate */ \
    if(!Reg) SET_FLAG(Z_FLAG);

#define ROTATE_RIGHT_THROUGH_CARRY(Reg) \
    unsigned char c = (af.b.l & 0x10 << 3); /* Save carry */ \
    af.b.l = (Reg & 0x01) << 4; /* Set C-flag (and reset all other) */ \
    Reg = (Reg >> 1) | c; /* Rotate through carry */ \
    if(!Reg) SET_FLAG(Z_FLAG);

#define ROTATE_RIGHT(Reg) \
    af.b.l = (Reg & 0x01) << 4; /* Set carry (and reset all other) */ \
    Reg = (Reg >> 1) | (Reg << 7); /* Rotate */ \
    if(!Reg) SET_FLAG(Z_FLAG);



void Cpu::LDrr_bb() { bc.b.h = bc.b.h; m = 1; }
void Cpu::LDrr_bc() { bc.b.h = bc.b.l; m = 1; }
void Cpu::LDrr_bd() { bc.b.h = de.b.h; m = 1; }
void Cpu::LDrr_be() { bc.b.h = de.b.l; m = 1; }
void Cpu::LDrr_bh() { bc.b.h = hl.b.h; m = 1; }
void Cpu::LDrr_bl() { bc.b.h = hl.b.l; m = 1; }
void Cpu::LDrr_ba() { bc.b.h = af.b.h; m = 1; }
void Cpu::LDrr_cb() { bc.b.l = bc.b.h; m = 1; }
void Cpu::LDrr_cc() { bc.b.l = bc.b.l; m = 1; }
void Cpu::LDrr_cd() { bc.b.l = de.b.h; m = 1; }
void Cpu::LDrr_ce() { bc.b.l = de.b.l; m = 1; }
void Cpu::LDrr_ch() { bc.b.l = hl.b.h; m = 1; }
void Cpu::LDrr_cl() { bc.b.l = hl.b.l; m = 1; }
void Cpu::LDrr_ca() { bc.b.l = af.b.h; m = 1; }
void Cpu::LDrr_db() { de.b.h = bc.b.h; m = 1; }
void Cpu::LDrr_dc() { de.b.h = bc.b.l; m = 1; }
void Cpu::LDrr_dd() { de.b.h = de.b.h; m = 1; }
void Cpu::LDrr_de() { de.b.h = de.b.l; m = 1; }
void Cpu::LDrr_dh() { de.b.h = hl.b.h; m = 1; }
void Cpu::LDrr_dl() { de.b.h = hl.b.l; m = 1; }
void Cpu::LDrr_da() { de.b.h = af.b.h; m = 1; }
void Cpu::LDrr_eb() { de.b.l = bc.b.h; m = 1; }
void Cpu::LDrr_ec() { de.b.l = bc.b.l; m = 1; }
void Cpu::LDrr_ed() { de.b.l = de.b.h; m = 1; }
void Cpu::LDrr_ee() { de.b.l = de.b.l; m = 1; }
void Cpu::LDrr_eh() { de.b.l = hl.b.h; m = 1; }
void Cpu::LDrr_el() { de.b.l = hl.b.l; m = 1; }
void Cpu::LDrr_ea() { de.b.l = af.b.h; m = 1; }
void Cpu::LDrr_hb() { hl.b.h = bc.b.h; m = 1; }
void Cpu::LDrr_hc() { hl.b.h = bc.b.l; m = 1; }
void Cpu::LDrr_hd() { hl.b.h = de.b.h; m = 1; }
void Cpu::LDrr_he() { hl.b.h = de.b.l; m = 1; }
void Cpu::LDrr_hh() { hl.b.h = hl.b.h; m = 1; }
void Cpu::LDrr_hl() { hl.b.h = hl.b.l; m = 1; }
void Cpu::LDrr_ha() { hl.b.h = af.b.h; m = 1; }
void Cpu::LDrr_lb() { hl.b.l = bc.b.h; m = 1; }
void Cpu::LDrr_lc() { hl.b.l = bc.b.l; m = 1; }
void Cpu::LDrr_ld() { hl.b.l = de.b.h; m = 1; }
void Cpu::LDrr_le() { hl.b.l = de.b.l; m = 1; }
void Cpu::LDrr_lh() { hl.b.l = hl.b.h; m = 1; }
void Cpu::LDrr_ll() { hl.b.l = hl.b.l; m = 1; }
void Cpu::LDrr_la() { hl.b.l = af.b.h; m = 1; }
void Cpu::LDrr_ab() { af.b.h = bc.b.h; m = 1; }
void Cpu::LDrr_ac() { af.b.h = bc.b.l; m = 1; }
void Cpu::LDrr_ad() { af.b.h = de.b.h; m = 1; }
void Cpu::LDrr_ae() { af.b.h = de.b.l; m = 1; }
void Cpu::LDrr_ah() { af.b.h = hl.b.h; m = 1; }
void Cpu::LDrr_al() { af.b.h = hl.b.l; m = 1; }
void Cpu::LDrr_aa() { af.b.h = af.b.h; m = 1; }

void Cpu::LDrHLm_b() { bc.b.h = _mbc->readByte(hl.w); m = 2; }
void Cpu::LDrHLm_c() { bc.b.l = _mbc->readByte(hl.w); m = 2; }
void Cpu::LDrHLm_d() { de.b.h = _mbc->readByte(hl.w); m = 2; }
void Cpu::LDrHLm_e() { de.b.l = _mbc->readByte(hl.w); m = 2; }
void Cpu::LDrHLm_h() { hl.b.h = _mbc->readByte(hl.w); m = 2; }
void Cpu::LDrHLm_l() { hl.b.l = _mbc->readByte(hl.w); m = 2; }
void Cpu::LDrHLm_a() { af.b.h = _mbc->readByte(hl.w); m = 2; }

void Cpu::LDHLmr_b() { _mbc->writeByte(hl.w, bc.b.h); m = 2; }
void Cpu::LDHLmr_c() { _mbc->writeByte(hl.w, bc.b.l); m = 2; }
void Cpu::LDHLmr_d() { _mbc->writeByte(hl.w, de.b.h); m = 2; }
void Cpu::LDHLmr_e() { _mbc->writeByte(hl.w, de.b.l); m = 2; }
void Cpu::LDHLmr_h() { _mbc->writeByte(hl.w, hl.b.h); m = 2; }
void Cpu::LDHLmr_l() { _mbc->writeByte(hl.w, hl.b.l); m = 2; }
void Cpu::LDHLmr_a() { _mbc->writeByte(hl.w, af.b.h); m = 2; }

void Cpu::LDrn_b() { bc.b.h = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_c() { bc.b.l = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_d() { de.b.h = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_e() { de.b.l = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_h() { hl.b.h = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_l() { hl.b.l = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_a() { af.b.h = _mbc->readByte(pc++); m = 2; }

void Cpu::LDHLmn() { _mbc->writeByte(hl.w, _mbc->readByte(pc++)); m = 3; }

void Cpu::LDBCmA() { _mbc->writeByte(bc.w, af.b.h); m = 2; }
void Cpu::LDDEmA() { _mbc->writeByte(de.w, af.b.h); m = 2; }

void Cpu::LDmmA() { _mbc->writeByte(_mbc->readShort(pc++), af.b.h); pc++; m = 4; }

void Cpu::LDABCm() { af.b.h = _mbc->readByte(bc.w); m = 2; }
void Cpu::LDADEm() { af.b.h = _mbc->readByte(de.w); m = 2; }

void Cpu::LDAmm() { af.b.h = _mbc->readByte( _mbc->readShort(pc++)); pc++; m = 4; }

void Cpu::LDBCnn() { bc.b.l = _mbc->readByte(pc++); bc.b.h = _mbc->readByte(pc++); m = 3; }
void Cpu::LDDEnn() { de.b.l = _mbc->readByte(pc++); de.b.h = _mbc->readByte(pc++); m = 3; }
void Cpu::LDHLnn() { hl.b.l = _mbc->readByte(pc++); hl.b.h = _mbc->readByte(pc++); m = 3; }
void Cpu::LDSPnn() { sp = _mbc->readShort(pc++); pc++; m = 3; }

void Cpu::LDHLIA() { _mbc->writeByte(hl.w, af.b.h); hl.w++; m = 2; }
void Cpu::LDAHLI() { af.b.h = _mbc->readByte(hl.w); hl.w++; m = 2; }

void Cpu::LDHLDA() { _mbc->writeByte(hl.w, af.b.h); hl.w--; m = 2; }
void Cpu::LDAHLD() { af.b.h = _mbc->readByte(hl.w); hl.w--; m = 2; }

void Cpu::LDAIOn() { af.b.h = _mbc->readByte(0xFF00 + _mbc->readByte(pc++)); m = 3; }
void Cpu::LDIOnA() { _mbc->writeByte(0xFF00 + _mbc->readByte(pc++), af.b.h); m = 3; }
void Cpu::LDAIOC() { af.b.h = _mbc->readByte(0xFF00 + bc.b.l); m = 2; }
void Cpu::LDIOCA() { _mbc->writeByte(0xFF00 + bc.b.l, af.b.h); m = 2; }

void Cpu::LDHLSPn() { char c = (char)_mbc->readByte(pc++); int s = sp + c; hl.w = s; SIGNED_ADD_WITH_FLAGS(sp, c, s); m = 3; }

void Cpu::LDmmSP() { auto mm = _mbc->readShort(pc++); pc++; _mbc->writeShort(mm, sp); m = 5; }

void Cpu::SWAPr_b() { auto v = bc.b.h; bc.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_c() { auto v = bc.b.l; bc.b.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_d() { auto v = de.b.h; de.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_e() { auto v = de.b.l; de.b.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_h() { auto v = hl.b.h; hl.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_l() { auto v = hl.b.l; hl.b.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_a() { auto v = af.b.h; af.b.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPHL() { auto v = _mbc->readByte(hl.w); v = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); _mbc->writeByte(hl.w, v); m = 4; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }

/* Data processing */

void Cpu::ADDr_b() { unsigned short a = af.b.h; af.b.h += bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, af.b.h, 8); m = 1; }
void Cpu::ADDr_c() { unsigned short a = af.b.h; af.b.h += bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, af.b.h, 8); m = 1; }
void Cpu::ADDr_d() { unsigned short a = af.b.h; af.b.h += de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, af.b.h, 8); m = 1; }
void Cpu::ADDr_e() { unsigned short a = af.b.h; af.b.h += de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, af.b.h, 8); m = 1; }
void Cpu::ADDr_h() { unsigned short a = af.b.h; af.b.h += hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, af.b.h, 8); m = 1; }
void Cpu::ADDr_l() { unsigned short a = af.b.h; af.b.h += hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, af.b.h, 8); m = 1; }
void Cpu::ADDr_a() { unsigned short a = af.b.h; af.b.h += af.b.h; COMPUTE_CARRY_FLAGS(a, a, af.b.h, 8); m = 1; }
void Cpu::ADDHL() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(hl.w); af.b.h += v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); m = 2; }
void Cpu::ADDn() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(pc++); af.b.h += v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); m = 2; }
void Cpu::ADDHLBC() { unsigned int rhl = hl.w; hl.w += bc.w; COMPUTE_CARRY_FLAGS(rhl, bc.w, hl.w, 16); m = 2; }
void Cpu::ADDHLDE() { unsigned int rhl = hl.w; hl.w += de.w; COMPUTE_CARRY_FLAGS(rhl, de.w, hl.w, 16); m = 2; }
void Cpu::ADDHLHL() { unsigned int rhl = hl.w; hl.w += hl.w; COMPUTE_CARRY_FLAGS(rhl, rhl, hl.w, 16); m = 2; }
void Cpu::ADDHLSP() { unsigned int rhl = hl.w; hl.w += sp; COMPUTE_CARRY_FLAGS(rhl, sp, hl.w, 16); m = 2; }
void Cpu::ADDSPn() { unsigned int rsp = sp; char v = _mbc->readByte(pc++); sp += v; COMPUTE_CARRY_FLAGS(rsp, v, sp, 16); m = 4; }

void Cpu::ADCr_b() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, af.b.h, 8); m = 1; }
void Cpu::ADCr_c() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, af.b.h, 8); m = 1; }
void Cpu::ADCr_d() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, af.b.h, 8); m = 1; }
void Cpu::ADCr_e() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, af.b.h, 8); m = 1; }
void Cpu::ADCr_h() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, af.b.h, 8); m = 1; }
void Cpu::ADCr_l() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, af.b.h, 8); m = 1; }
void Cpu::ADCr_a() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); af.b.h = a + af.b.h; COMPUTE_CARRY_FLAGS(a, af.b.h, af.b.h, 8); m = 1; }
void Cpu::ADCHL() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); unsigned short v = _mbc->readByte(hl.w); af.b.h = a + v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); m = 2; }
void Cpu::ADCn() { unsigned short a = af.b.h + ((af.b.l & 0x10) >> 4); unsigned short v = _mbc->readByte(pc++); af.b.h = a + v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); m = 2; }

void Cpu::SUBr_b() { unsigned short a = af.b.h; af.b.h -= bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_c() { unsigned short a = af.b.h; af.b.h -= bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_d() { unsigned short a = af.b.h; af.b.h -= de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_e() { unsigned short a = af.b.h; af.b.h -= de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_h() { unsigned short a = af.b.h; af.b.h -= hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_l() { unsigned short a = af.b.h; af.b.h -= hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_a() { unsigned short a = af.b.h; af.b.h -= af.b.h; COMPUTE_CARRY_FLAGS(a, a, af.b.h, 8); SET_FLAG(Z_FLAG); m = 1; }
void Cpu::SUBHL() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(hl.w); af.b.h -= v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); SET_FLAG(N_FLAG); m = 2; }
void Cpu::SUBn() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(pc++); af.b.h -= v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); SET_FLAG(N_FLAG); m = 2; }

void Cpu::SBCr_b() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SBCr_c() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SBCr_d() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SBCr_e() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SBCr_h() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SBCr_l() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SBCr_a() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); af.b.h = a - af.b.h; COMPUTE_CARRY_FLAGS(a, af.b.h, af.b.h, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::SBCHL() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); unsigned short v = _mbc->readByte(hl.w); af.b.h = a - v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); SET_FLAG(N_FLAG); m = 2; }
void Cpu::SBCn() { unsigned short a = af.b.h - ((af.b.l & 0x10) >> 4); unsigned short v = _mbc->readByte(pc++); af.b.h = a - v; COMPUTE_CARRY_FLAGS(a, v, af.b.h, 8); SET_FLAG(N_FLAG); m = 2; }

void Cpu::CPr_b() { unsigned short a = af.b.h; unsigned short s = a - bc.b.h; COMPUTE_CARRY_FLAGS(a, bc.b.h, s, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_c() { unsigned short a = af.b.h; unsigned short s = a - bc.b.l; COMPUTE_CARRY_FLAGS(a, bc.b.l, s, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_d() { unsigned short a = af.b.h; unsigned short s = a - de.b.h; COMPUTE_CARRY_FLAGS(a, de.b.h, s, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_e() { unsigned short a = af.b.h; unsigned short s = a - de.b.l; COMPUTE_CARRY_FLAGS(a, de.b.l, s, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_h() { unsigned short a = af.b.h; unsigned short s = a - hl.b.h; COMPUTE_CARRY_FLAGS(a, hl.b.h, s, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_l() { unsigned short a = af.b.h; unsigned short s = a - hl.b.l; COMPUTE_CARRY_FLAGS(a, hl.b.l, s, 8); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_a() { unsigned short a = af.b.h; unsigned short s = a - af.b.h; COMPUTE_CARRY_FLAGS(a, a, s, 8); SET_FLAG(Z_FLAG); m = 1; }
void Cpu::CPHL() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(hl.w); unsigned short s = a - v; COMPUTE_CARRY_FLAGS(a, v, s, 8); SET_FLAG(N_FLAG); m = 2; }
void Cpu::CPn() { unsigned short a = af.b.h; unsigned short v = _mbc->readByte(pc++); unsigned short s = a - v; COMPUTE_CARRY_FLAGS(a, v, s, 8); SET_FLAG(N_FLAG); m = 2; }

void Cpu::ANDr_b() { af.b.h &= bc.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_c() { af.b.h &= bc.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_d() { af.b.h &= de.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_e() { af.b.h &= de.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_h() { af.b.h &= hl.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_l() { af.b.h &= hl.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_a() { af.b.h &= af.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDHL() { af.b.h &= _mbc->readByte(hl.w); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 2; }
void Cpu::ANDn() { af.b.h &= _mbc->readByte(pc++); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 2; }

void Cpu::ORr_b() { af.b.h |= bc.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_c() { af.b.h |= bc.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_d() { af.b.h |= de.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_e() { af.b.h |= de.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_h() { af.b.h |= hl.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_l() { af.b.h |= hl.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_a() { af.b.h |= af.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORHL() { af.b.h |= _mbc->readByte(hl.w); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::ORn() { af.b.h |= _mbc->readByte(pc++); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }

void Cpu::XORr_b() { af.b.h ^= bc.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_c() { af.b.h ^= bc.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_d() { af.b.h ^= de.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_e() { af.b.h ^= de.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_h() { af.b.h ^= hl.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_l() { af.b.h ^= hl.b.l; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_a() { af.b.h ^= af.b.h; RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORHL() { af.b.h ^= _mbc->readByte(hl.w); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::XORn() { af.b.h ^= _mbc->readByte(pc++); RESET_FLAG(ALL_FLAGS); if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }

void Cpu::INCr_b() { INC_REG(bc.b.h); m = 1; }
void Cpu::INCr_c() { INC_REG(bc.b.l); m = 1; }
void Cpu::INCr_d() { INC_REG(de.b.h); m = 1; }
void Cpu::INCr_e() { INC_REG(de.b.l); m = 1; }
void Cpu::INCr_h() { INC_REG(hl.b.h); m = 1; }
void Cpu::INCr_l() { INC_REG(hl.b.l); m = 1; }
void Cpu::INCr_a() { INC_REG(af.b.h); m = 1; }
void Cpu::INCHLm() { auto mem = _mbc->readByte(hl.w); INC_REG(mem); _mbc->writeByte(hl.w, mem); m = 3; }

void Cpu::DECr_b() { DEC_REG(bc.b.h); m = 1; }
void Cpu::DECr_c() { DEC_REG(bc.b.l); m = 1; }
void Cpu::DECr_d() { DEC_REG(de.b.h); m = 1; }
void Cpu::DECr_e() { DEC_REG(de.b.l); m = 1; }
void Cpu::DECr_h() { DEC_REG(hl.b.h); m = 1; }
void Cpu::DECr_l() { DEC_REG(hl.b.l); m = 1; }
void Cpu::DECr_a() { DEC_REG(af.b.h); m = 1; }
void Cpu::DECHLm() { auto mem = _mbc->readByte(hl.w); DEC_REG(mem); _mbc->writeByte(hl.w, mem); m = 3; }

void Cpu::INCBC() { bc.w++; m = 2; }
void Cpu::INCDE() { de.w++; m = 2; }
void Cpu::INCHL() { hl.w++; m = 2; }
void Cpu::INCSP() { sp++; m = 2; }

void Cpu::DECBC() { bc.w--; m = 2; }
void Cpu::DECDE() { de.w--; m = 2; }
void Cpu::DECHL() { hl.w--; m = 2; }
void Cpu::DECSP() { sp--; m = 2; }

/* Bit manipulation */
void Cpu::BIT0b() { BIT_CHECK(bc.b.h, 0); m = 2; }
void Cpu::BIT0c() { BIT_CHECK(bc.b.l, 0); m = 2; }
void Cpu::BIT0d() { BIT_CHECK(de.b.h, 0); m = 2; }
void Cpu::BIT0e() { BIT_CHECK(de.b.l, 0); m = 2; }
void Cpu::BIT0h() { BIT_CHECK(hl.b.h, 0); m = 2; }
void Cpu::BIT0l() { BIT_CHECK(hl.b.l, 0); m = 2; }
void Cpu::BIT0a() { BIT_CHECK(af.b.h, 0); m = 2; }
void Cpu::BIT0m() { BIT_CHECK(_mbc->readByte(hl.w), 0); m = 4; }

void Cpu::RES0b() { bc.b.h &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0c() { bc.b.l &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0d() { de.b.h &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0e() { de.b.l &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0h() { hl.b.h &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0l() { hl.b.l &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0a() { af.b.h &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 0)); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::SET0b() { bc.b.h |= (1 << 0); m = 2; }
void Cpu::SET0c() { bc.b.l |= (1 << 0); m = 2; }
void Cpu::SET0d() { de.b.h |= (1 << 0); m = 2; }
void Cpu::SET0e() { de.b.l |= (1 << 0); m = 2; }
void Cpu::SET0h() { hl.b.h |= (1 << 0); m = 2; }
void Cpu::SET0l() { hl.b.l |= (1 << 0); m = 2; }
void Cpu::SET0a() { af.b.h |= (1 << 0); m = 2; }
void Cpu::SET0m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 0); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::BIT1b() { BIT_CHECK(bc.b.h, 1); m = 2; }
void Cpu::BIT1c() { BIT_CHECK(bc.b.l, 1); m = 2; }
void Cpu::BIT1d() { BIT_CHECK(de.b.h, 1); m = 2; }
void Cpu::BIT1e() { BIT_CHECK(de.b.l, 1); m = 2; }
void Cpu::BIT1h() { BIT_CHECK(hl.b.h, 1); m = 2; }
void Cpu::BIT1l() { BIT_CHECK(hl.b.l, 1); m = 2; }
void Cpu::BIT1a() { BIT_CHECK(af.b.h, 1); m = 2; }
void Cpu::BIT1m() { BIT_CHECK(_mbc->readByte(hl.w), 1); m = 4; }

void Cpu::RES1b() { bc.b.h &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1c() { bc.b.l &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1d() { de.b.h &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1e() { de.b.l &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1h() { hl.b.h &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1l() { hl.b.l &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1a() { af.b.h &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 1)); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::SET1b() { bc.b.h |= (1 << 1); m = 2; }
void Cpu::SET1c() { bc.b.l |= (1 << 1); m = 2; }
void Cpu::SET1d() { de.b.h |= (1 << 1); m = 2; }
void Cpu::SET1e() { de.b.l |= (1 << 1); m = 2; }
void Cpu::SET1h() { hl.b.h |= (1 << 1); m = 2; }
void Cpu::SET1l() { hl.b.l |= (1 << 1); m = 2; }
void Cpu::SET1a() { af.b.h |= (1 << 1); m = 2; }
void Cpu::SET1m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 1); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::BIT2b() { BIT_CHECK(bc.b.h, 2); m = 2; }
void Cpu::BIT2c() { BIT_CHECK(bc.b.l, 2); m = 2; }
void Cpu::BIT2d() { BIT_CHECK(de.b.h, 2); m = 2; }
void Cpu::BIT2e() { BIT_CHECK(de.b.l, 2); m = 2; }
void Cpu::BIT2h() { BIT_CHECK(hl.b.h, 2); m = 2; }
void Cpu::BIT2l() { BIT_CHECK(hl.b.l, 2); m = 2; }
void Cpu::BIT2a() { BIT_CHECK(af.b.h, 2); m = 2; }
void Cpu::BIT2m() { BIT_CHECK(_mbc->readByte(hl.w), 2); m = 4; }

void Cpu::RES2b() { bc.b.h &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2c() { bc.b.l &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2d() { de.b.h &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2e() { de.b.l &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2h() { hl.b.h &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2l() { hl.b.l &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2a() { af.b.h &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 2)); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::SET2b() { bc.b.h |= (1 << 2); m = 2; }
void Cpu::SET2c() { bc.b.l |= (1 << 2); m = 2; }
void Cpu::SET2d() { de.b.h |= (1 << 2); m = 2; }
void Cpu::SET2e() { de.b.l |= (1 << 2); m = 2; }
void Cpu::SET2h() { hl.b.h |= (1 << 2); m = 2; }
void Cpu::SET2l() { hl.b.l |= (1 << 2); m = 2; }
void Cpu::SET2a() { af.b.h |= (1 << 2); m = 2; }
void Cpu::SET2m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 2); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::BIT3b() { BIT_CHECK(bc.b.h, 3); m = 2; }
void Cpu::BIT3c() { BIT_CHECK(bc.b.l, 3); m = 2; }
void Cpu::BIT3d() { BIT_CHECK(de.b.h, 3); m = 2; }
void Cpu::BIT3e() { BIT_CHECK(de.b.l, 3); m = 2; }
void Cpu::BIT3h() { BIT_CHECK(hl.b.h, 3); m = 2; }
void Cpu::BIT3l() { BIT_CHECK(hl.b.l, 3); m = 2; }
void Cpu::BIT3a() { BIT_CHECK(af.b.h, 3); m = 2; }
void Cpu::BIT3m() { BIT_CHECK(_mbc->readByte(hl.w), 3); m = 4; }

void Cpu::RES3b() { bc.b.h &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3c() { bc.b.l &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3d() { de.b.h &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3e() { de.b.l &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3h() { hl.b.h &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3l() { hl.b.l &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3a() { af.b.h &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 3)); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::SET3b() { bc.b.h |= (1 << 3); m = 2; }
void Cpu::SET3c() { bc.b.l |= (1 << 3); m = 2; }
void Cpu::SET3d() { de.b.h |= (1 << 3); m = 2; }
void Cpu::SET3e() { de.b.l |= (1 << 3); m = 2; }
void Cpu::SET3h() { hl.b.h |= (1 << 3); m = 2; }
void Cpu::SET3l() { hl.b.l |= (1 << 3); m = 2; }
void Cpu::SET3a() { af.b.h |= (1 << 3); m = 2; }
void Cpu::SET3m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 3); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::BIT4b() { BIT_CHECK(bc.b.h, 4); m = 2; }
void Cpu::BIT4c() { BIT_CHECK(bc.b.l, 4); m = 2; }
void Cpu::BIT4d() { BIT_CHECK(de.b.h, 4); m = 2; }
void Cpu::BIT4e() { BIT_CHECK(de.b.l, 4); m = 2; }
void Cpu::BIT4h() { BIT_CHECK(hl.b.h, 4); m = 2; }
void Cpu::BIT4l() { BIT_CHECK(hl.b.l, 4); m = 2; }
void Cpu::BIT4a() { BIT_CHECK(af.b.h, 4); m = 2; }
void Cpu::BIT4m() { BIT_CHECK(_mbc->readByte(hl.w), 4); m = 4; }

void Cpu::RES4b() { bc.b.h &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4c() { bc.b.l &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4d() { de.b.h &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4e() { de.b.l &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4h() { hl.b.h &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4l() { hl.b.l &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4a() { af.b.h &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 4)); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::SET4b() { bc.b.h |= (1 << 4); m = 2; }
void Cpu::SET4c() { bc.b.l |= (1 << 4); m = 2; }
void Cpu::SET4d() { de.b.h |= (1 << 4); m = 2; }
void Cpu::SET4e() { de.b.l |= (1 << 4); m = 2; }
void Cpu::SET4h() { hl.b.h |= (1 << 4); m = 2; }
void Cpu::SET4l() { hl.b.l |= (1 << 4); m = 2; }
void Cpu::SET4a() { af.b.h |= (1 << 4); m = 2; }
void Cpu::SET4m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 4); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::BIT5b() { BIT_CHECK(bc.b.h, 5); m = 2; }
void Cpu::BIT5c() { BIT_CHECK(bc.b.l, 5); m = 2; }
void Cpu::BIT5d() { BIT_CHECK(de.b.h, 5); m = 2; }
void Cpu::BIT5e() { BIT_CHECK(de.b.l, 5); m = 2; }
void Cpu::BIT5h() { BIT_CHECK(hl.b.h, 5); m = 2; }
void Cpu::BIT5l() { BIT_CHECK(hl.b.l, 5); m = 2; }
void Cpu::BIT5a() { BIT_CHECK(af.b.h, 5); m = 2; }
void Cpu::BIT5m() { BIT_CHECK(_mbc->readByte(hl.w), 5); m = 4; }

void Cpu::RES5b() { bc.b.h &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5c() { bc.b.l &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5d() { de.b.h &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5e() { de.b.l &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5h() { hl.b.h &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5l() { hl.b.l &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5a() { af.b.h &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 5)); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::SET5b() { bc.b.h |= (1 << 5); m = 2; }
void Cpu::SET5c() { bc.b.l |= (1 << 5); m = 2; }
void Cpu::SET5d() { de.b.h |= (1 << 5); m = 2; }
void Cpu::SET5e() { de.b.l |= (1 << 5); m = 2; }
void Cpu::SET5h() { hl.b.h |= (1 << 5); m = 2; }
void Cpu::SET5l() { hl.b.l |= (1 << 5); m = 2; }
void Cpu::SET5a() { af.b.h |= (1 << 5); m = 2; }
void Cpu::SET5m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 5); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::BIT6b() { BIT_CHECK(bc.b.h, 6); m = 2; }
void Cpu::BIT6c() { BIT_CHECK(bc.b.l, 6); m = 2; }
void Cpu::BIT6d() { BIT_CHECK(de.b.h, 6); m = 2; }
void Cpu::BIT6e() { BIT_CHECK(de.b.l, 6); m = 2; }
void Cpu::BIT6h() { BIT_CHECK(hl.b.h, 6); m = 2; }
void Cpu::BIT6l() { BIT_CHECK(hl.b.l, 6); m = 2; }
void Cpu::BIT6a() { BIT_CHECK(af.b.h, 6); m = 2; }
void Cpu::BIT6m() { BIT_CHECK(_mbc->readByte(hl.w), 6); m = 4; }

void Cpu::RES6b() { bc.b.h &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6c() { bc.b.l &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6d() { de.b.h &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6e() { de.b.l &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6h() { hl.b.h &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6l() { hl.b.l &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6a() { af.b.h &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 6)); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::SET6b() { bc.b.h |= (1 << 6); m = 2; }
void Cpu::SET6c() { bc.b.l |= (1 << 6); m = 2; }
void Cpu::SET6d() { de.b.h |= (1 << 6); m = 2; }
void Cpu::SET6e() { de.b.l |= (1 << 6); m = 2; }
void Cpu::SET6h() { hl.b.h |= (1 << 6); m = 2; }
void Cpu::SET6l() { hl.b.l |= (1 << 6); m = 2; }
void Cpu::SET6a() { af.b.h |= (1 << 6); m = 2; }
void Cpu::SET6m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 6); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::BIT7b() { BIT_CHECK(bc.b.h, 7); m = 2; }
void Cpu::BIT7c() { BIT_CHECK(bc.b.l, 7); m = 2; }
void Cpu::BIT7d() { BIT_CHECK(de.b.h, 7); m = 2; }
void Cpu::BIT7e() { BIT_CHECK(de.b.l, 7); m = 2; }
void Cpu::BIT7h() { BIT_CHECK(hl.b.h, 7); m = 2; }
void Cpu::BIT7l() { BIT_CHECK(hl.b.l, 7); m = 2; }
void Cpu::BIT7a() { BIT_CHECK(af.b.h, 7); m = 2; }
void Cpu::BIT7m() { BIT_CHECK(_mbc->readByte(hl.w), 7); m = 4; }

void Cpu::RES7b() { bc.b.h &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7c() { bc.b.l &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7d() { de.b.h &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7e() { de.b.l &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7h() { hl.b.h &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7l() { hl.b.l &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7a() { af.b.h &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7m() { auto mem = _mbc->readByte(hl.w); mem &= (0xFF ^ (1 << 7)); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::SET7b() { bc.b.h |= (1 << 7); m = 2; }
void Cpu::SET7c() { bc.b.l |= (1 << 7); m = 2; }
void Cpu::SET7d() { de.b.h |= (1 << 7); m = 2; }
void Cpu::SET7e() { de.b.l |= (1 << 7); m = 2; }
void Cpu::SET7h() { hl.b.h |= (1 << 7); m = 2; }
void Cpu::SET7l() { hl.b.l |= (1 << 7); m = 2; }
void Cpu::SET7a() { af.b.h |= (1 << 7); m = 2; }
void Cpu::SET7m() { auto mem = _mbc->readByte(hl.w); mem |= (1 << 7); _mbc->writeByte(hl.w, mem); m = 4; }

void Cpu::RLA() { ROTATE_LEFT_THROUGH_CARRY(af.b.h); m = 1; }
void Cpu::RLCA() { ROTATE_LEFT(af.b.h); m = 1; }
void Cpu::RRA() { ROTATE_RIGHT_THROUGH_CARRY(af.b.h); m = 1;  }
void Cpu::RRCA() { ROTATE_RIGHT(af.b.h); m = 1; }

void Cpu::RLr_b() { ROTATE_LEFT_THROUGH_CARRY(bc.b.h); m = 2; }
void Cpu::RLr_c() { ROTATE_LEFT_THROUGH_CARRY(bc.b.l); m = 2; }
void Cpu::RLr_d() { ROTATE_LEFT_THROUGH_CARRY(de.b.h); m = 2; }
void Cpu::RLr_e() { ROTATE_LEFT_THROUGH_CARRY(de.b.l); m = 2; }
void Cpu::RLr_h() { ROTATE_LEFT_THROUGH_CARRY(hl.b.h); m = 2; }
void Cpu::RLr_l() { ROTATE_LEFT_THROUGH_CARRY(hl.b.l); m = 2; }
void Cpu::RLr_a() { ROTATE_LEFT_THROUGH_CARRY(af.b.h); m = 2; }
void Cpu::RLHL() { unsigned char v = _mbc->readByte(hl.w); ROTATE_LEFT_THROUGH_CARRY(v); _mbc->writeByte(hl.w, v); m = 4; }

void Cpu::RLCr_b() { ROTATE_LEFT(bc.b.h); m = 2; }
void Cpu::RLCr_c() { ROTATE_LEFT(bc.b.l); m = 2; }
void Cpu::RLCr_d() { ROTATE_LEFT(de.b.h); m = 2; }
void Cpu::RLCr_e() { ROTATE_LEFT(de.b.l); m = 2; }
void Cpu::RLCr_h() { ROTATE_LEFT(hl.b.h); m = 2; }
void Cpu::RLCr_l() { ROTATE_LEFT(hl.b.l); m = 2; }
void Cpu::RLCr_a() { ROTATE_LEFT(af.b.h); m = 2; }
void Cpu::RLCHL() { unsigned char v = _mbc->readByte(hl.w); ROTATE_LEFT(v); _mbc->writeByte(hl.w, v); m = 4; }

void Cpu::RRr_b() { ROTATE_RIGHT_THROUGH_CARRY(bc.b.h); m = 2; }
void Cpu::RRr_c() { ROTATE_RIGHT_THROUGH_CARRY(bc.b.l); m = 2; }
void Cpu::RRr_d() { ROTATE_RIGHT_THROUGH_CARRY(de.b.h); m = 2; }
void Cpu::RRr_e() { ROTATE_RIGHT_THROUGH_CARRY(de.b.l); m = 2; }
void Cpu::RRr_h() { ROTATE_RIGHT_THROUGH_CARRY(hl.b.h); m = 2; }
void Cpu::RRr_l() { ROTATE_RIGHT_THROUGH_CARRY(hl.b.l); m = 2; }
void Cpu::RRr_a() { ROTATE_RIGHT_THROUGH_CARRY(af.b.h); m = 2; }
void Cpu::RRHL() { unsigned char v = _mbc->readByte(hl.w); ROTATE_RIGHT_THROUGH_CARRY(v); _mbc->writeByte(hl.w, v); m = 4; }

void Cpu::RRCr_b() { ROTATE_RIGHT(bc.b.h); m = 2; }
void Cpu::RRCr_c() { ROTATE_RIGHT(bc.b.l); m = 2; }
void Cpu::RRCr_d() { ROTATE_RIGHT(de.b.h); m = 2; }
void Cpu::RRCr_e() { ROTATE_RIGHT(de.b.l); m = 2; }
void Cpu::RRCr_h() { ROTATE_RIGHT(hl.b.h); m = 2; }
void Cpu::RRCr_l() { ROTATE_RIGHT(hl.b.l); m = 2; }
void Cpu::RRCr_a() { ROTATE_RIGHT(af.b.h); m = 2; }
void Cpu::RRCHL() { unsigned char v = _mbc->readByte(hl.w); ROTATE_RIGHT(v); _mbc->writeByte(hl.w, v); m = 4; }

void Cpu::SLAr_b() { af.b.l = (bc.b.h & 0x80) >> 3; bc.b.h <<= 1; if(!bc.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_c() { af.b.l = (bc.b.l & 0x80) >> 3; bc.b.l <<= 1; if(!bc.b.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_d() { af.b.l = (de.b.h & 0x80) >> 3; de.b.h <<= 1; if(!de.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_e() { af.b.l = (de.b.l & 0x80) >> 3; de.b.l <<= 1; if(!de.b.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_h() { af.b.l = (hl.b.h & 0x80) >> 3; hl.b.h <<= 1; if(!hl.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_l() { af.b.l = (hl.b.l & 0x80) >> 3; hl.b.l <<= 1; if(!hl.b.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_a() { af.b.l = (af.b.h & 0x80) >> 3; af.b.h <<= 1; if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAHL() { auto v = _mbc->readByte(hl.w); af.b.l = (v & 0x80) >> 3; v <<= 1; _mbc->writeByte(hl.w, v); if(!v) SET_FLAG(Z_FLAG); m = 4; }

void Cpu::SRAr_b() { af.b.l = (bc.b.h & 0x01) << 4; bc.b.h = (bc.b.h >> 1) | (bc.b.h & 0x80); if(!bc.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_c() { af.b.l = (bc.b.l & 0x01) << 4; bc.b.l = (bc.b.l >> 1) | (bc.b.l & 0x80); if(!bc.b.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_d() { af.b.l = (de.b.h & 0x01) << 4; de.b.h = (de.b.h >> 1) | (de.b.h & 0x80); if(!de.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_e() { af.b.l = (de.b.l & 0x01) << 4; de.b.l = (de.b.l >> 1) | (de.b.l & 0x80); if(!de.b.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_h() { af.b.l = (hl.b.h & 0x01) << 4; hl.b.h = (hl.b.h >> 1) | (hl.b.h & 0x80); if(!hl.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_l() { af.b.l = (hl.b.l & 0x01) << 4; hl.b.l = (hl.b.l >> 1) | (hl.b.l & 0x80); if(!hl.b.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_a() { af.b.l = (af.b.h & 0x01) << 4; af.b.h = (af.b.h >> 1) | (af.b.h & 0x80); if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAHL() { auto v = _mbc->readByte(hl.w); af.b.l = (v & 0x01) << 4; v = (v >> 1) | (v & 0x80); _mbc->writeByte(hl.w, v); if(!v) SET_FLAG(Z_FLAG); m = 4; }

void Cpu::SRLr_b() { af.b.l = (bc.b.h & 0x01) << 4; bc.b.h >>= 1; if(!bc.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_c() { af.b.l = (bc.b.l & 0x01) << 4; bc.b.l >>= 1; if(!bc.b.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_d() { af.b.l = (de.b.h & 0x01) << 4; de.b.h >>= 1; if(!de.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_e() { af.b.l = (de.b.l & 0x01) << 4; de.b.l >>= 1; if(!de.b.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_h() { af.b.l = (hl.b.h & 0x01) << 4; hl.b.h >>= 1; if(!hl.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_l() { af.b.l = (hl.b.l & 0x01) << 4; hl.b.l >>= 1; if(!hl.b.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_a() { af.b.l = (af.b.h & 0x01) << 4; af.b.h >>= 1; if(!af.b.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLHL() { auto v = _mbc->readByte(hl.w); af.b.l = (v & 0x01) << 4; v >>= 1; _mbc->writeByte(hl.w, v); if(!v) SET_FLAG(Z_FLAG); m = 4; }

void Cpu::CPL() { af.b.h = ~af.b.h; SET_FLAG(N_FLAG | H_FLAG); m = 1; }
void Cpu::CCF() { af.b.l ^= C_FLAG; RESET_FLAG(N_FLAG | H_FLAG); m = 1; }
void Cpu::SCF() { SET_FLAG(C_FLAG); RESET_FLAG(N_FLAG | H_FLAG); m = 1; }

/* Pushs and pops */

void Cpu::PUSHBC() { sp-=2; bc.w = _mbc->readShort(sp); m = 4;}
void Cpu::PUSHDE() { sp-=2; de.w = _mbc->readShort(sp); m = 4;}
void Cpu::PUSHHL() { sp-=2; hl.w = _mbc->readShort(sp); m = 4;}
void Cpu::PUSHAF() { sp-=2; af.w = _mbc->readShort(sp); m = 4;}

void Cpu::POPBC() { _mbc->writeShort(sp, bc.w); sp += 2; m = 3;}
void Cpu::POPDE() { _mbc->writeShort(sp, de.w); sp += 2; m = 3;}
void Cpu::POPHL() { _mbc->writeShort(sp, hl.w); sp += 2; m = 3;}
void Cpu::POPAF() { _mbc->writeShort(sp, af.w); sp += 2; m = 3;}


/* Jumps */

void Cpu::JPnn() { pc = _mbc->readShort(pc++); pc++; m = 3; }
void Cpu::JPHL() { pc = hl.w; m = 1; }
void Cpu::JPNZnn() { auto nn = _mbc->readShort(pc++); pc++; if(!(af.b.l & Z_FLAG)) pc = nn; m = 3; }
void Cpu::JPZnn() { auto nn = _mbc->readShort(pc++); pc++; if(af.b.l & Z_FLAG) pc = nn; m = 3; }
void Cpu::JPNCnn() { auto nn = _mbc->readShort(pc++); pc++; if(!(af.b.l & C_FLAG)) pc = nn; m = 3; }
void Cpu::JPCnn() { auto nn = _mbc->readShort(pc++); pc++; if(af.b.l & C_FLAG) pc = nn; m = 3; }

void Cpu::JRn() { auto n = (char)_mbc->readByte(pc++); pc += n; m = 2; }
void Cpu::JRNZn() { auto n = (char)_mbc->readByte(pc++); if(!(af.b.l & Z_FLAG)) pc += n; m = 3; }
void Cpu::JRZn() { auto n = (char)_mbc->readByte(pc++); if(af.b.l & Z_FLAG) pc += n; m = 3; }
void Cpu::JRNCn() { auto n = (char)_mbc->readByte(pc++); if(!(af.b.l & C_FLAG)) pc += n; m = 3; }
void Cpu::JRCn() { auto n = (char)_mbc->readByte(pc++); if(af.b.l & C_FLAG) pc += n; m = 3; }

void Cpu::CALLnn() { auto nn = _mbc->readShort(pc++); pc++; sp -= 2; _mbc->writeShort(sp, pc); pc = nn; m = 3; }
void Cpu::CALLNZnn() { auto nn = _mbc->readShort(pc++); pc++; if(!(af.b.l & Z_FLAG)) { sp -= 2; _mbc->writeShort(sp, pc); pc = nn; } m = 3; }
void Cpu::CALLZnn() { auto nn = _mbc->readShort(pc++); pc++; if(af.b.l & Z_FLAG) { sp -= 2; _mbc->writeShort(sp, pc); pc = nn; } m = 3; }
void Cpu::CALLNCnn() { auto nn = _mbc->readShort(pc++); pc++; if(!(af.b.l & C_FLAG)) { sp -= 2; _mbc->writeShort(sp, pc); pc = nn; } m = 3; }
void Cpu::CALLCnn() { auto nn = _mbc->readShort(pc++); pc++; if(af.b.l & C_FLAG) { sp -= 2; _mbc->writeShort(sp, pc); pc = nn; } m = 3; }

void Cpu::RET() { pc = _mbc->readShort(sp); sp += 2; m = 2; }
void Cpu::RETI() { ime = true; pc = _mbc->readShort(sp); sp += 2; m = 2; }
void Cpu::RETNZ() { if(!(af.b.l & Z_FLAG)) { pc = _mbc->readShort(sp); sp += 2; } m = 2; }
void Cpu::RETZ() { if(af.b.l & Z_FLAG) { pc = _mbc->readShort(sp); sp += 2; } m = 2; }
void Cpu::RETNC() { if(!(af.b.l & C_FLAG)) { pc = _mbc->readShort(sp); sp += 2; } m = 2; }
void Cpu::RETC() { if(af.b.l & C_FLAG) { pc = _mbc->readShort(sp); sp += 2; } m = 2; }

void Cpu::RST00() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x00; m = 8; }
void Cpu::RST08() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x08; m = 8; }
void Cpu::RST10() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x10; m = 8; }
void Cpu::RST18() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x18; m = 8; }
void Cpu::RST20() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x20; m = 8; }
void Cpu::RST28() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x28; m = 8; }
void Cpu::RST30() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x30; m = 8; }
void Cpu::RST38() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x38; m = 8; }

void Cpu::STOP() { halt = true; pc++; m = 1; }
void Cpu::HALT() { halt = true; m = 1; }
void Cpu::DI() { ime = false; m = 1; }
void Cpu::EI() { ime = true; m = 1; }
