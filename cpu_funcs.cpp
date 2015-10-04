#include "cpu.h"
#include <iostream>

#define COMPUTE_CARRY_FLAGS_8(Term1, Term2, Sum) \
    RESET_FLAG(ALL_FLAGS); \
    if(!(Sum & 0xFF)) SET_FLAG(Z_FLAG); \
    auto val = (Sum ^ Term1 ^ Term2); \
    auto c = (val & (1 << 8)) >> 4; \
    auto h = (val & (1 << 4)) << 1; \
    SET_FLAG(c | h);


#define COMPUTE_CARRY_FLAGS_16(Term1, Term2, Sum) \
    RESET_FLAG(N_FLAG | H_FLAG | C_FLAG ); \
    auto val = (Sum ^ Term1 ^ Term2); \
    auto c = (val & (1 << 16)) >> (16 - 4); \
    auto h = (val & (1 << 12)) >> (12 - 5); \
    SET_FLAG(c | h);

#define COMPUTE_CARRY_FLAGS_16_8(Term1, Term2, Sum) \
    RESET_FLAG(N_FLAG | H_FLAG | C_FLAG ); \
    auto val = (Sum ^ Term1 ^ Term2); \
    auto c = (val & (1 << 8)) >> 4; \
    auto h = (val & (1 << 4)) << 1; \
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
    r.f &= ~(((Reg & (1 << Bit)) >> Bit) << 7);

#define ROTATE_LEFT_THROUGH_CARRY(Reg) \
    unsigned char c = ((r.f & 0x10) >> 4); /* Save carry */ \
    r.f = (Reg & 0x80) >> 3; /* Set C-flag (and reset all other) */ \
    Reg = (Reg << 1) | c; /* Rotate through carry */ 

#define ROTATE_LEFT(Reg) \
    r.f = (Reg & 0x80) >> 3; /* Set carry (and reset all other) */ \
    Reg = (Reg << 1) | (Reg >> 7); /* Rotate */ 

#define ROTATE_RIGHT_THROUGH_CARRY(Reg) \
    unsigned char c = ((r.f & 0x10) << 3); /* Save carry */ \
    r.f = (Reg & 0x01) << 4; /* Set C-flag (and reset all other) */ \
    Reg = (Reg >> 1) | c; /* Rotate through carry */ 

#define ROTATE_RIGHT(Reg) \
    r.f = (Reg & 0x01) << 4; /* Set carry (and reset all other) */ \
    Reg = (Reg >> 1) | (Reg << 7); /* Rotate */ 



void Cpu::LDrr_bb() { r.b = r.b; m = 1; }
void Cpu::LDrr_bc() { r.b = r.c; m = 1; }
void Cpu::LDrr_bd() { r.b = r.d; m = 1; }
void Cpu::LDrr_be() { r.b = r.e; m = 1; }
void Cpu::LDrr_bh() { r.b = r.h; m = 1; }
void Cpu::LDrr_bl() { r.b = r.l; m = 1; }
void Cpu::LDrr_ba() { r.b = r.a; m = 1; }
void Cpu::LDrr_cb() { r.c = r.b; m = 1; }
void Cpu::LDrr_cc() { r.c = r.c; m = 1; }
void Cpu::LDrr_cd() { r.c = r.d; m = 1; }
void Cpu::LDrr_ce() { r.c = r.e; m = 1; }
void Cpu::LDrr_ch() { r.c = r.h; m = 1; }
void Cpu::LDrr_cl() { r.c = r.l; m = 1; }
void Cpu::LDrr_ca() { r.c = r.a; m = 1; }
void Cpu::LDrr_db() { r.d = r.b; m = 1; }
void Cpu::LDrr_dc() { r.d = r.c; m = 1; }
void Cpu::LDrr_dd() { r.d = r.d; m = 1; }
void Cpu::LDrr_de() { r.d = r.e; m = 1; }
void Cpu::LDrr_dh() { r.d = r.h; m = 1; }
void Cpu::LDrr_dl() { r.d = r.l; m = 1; }
void Cpu::LDrr_da() { r.d = r.a; m = 1; }
void Cpu::LDrr_eb() { r.e = r.b; m = 1; }
void Cpu::LDrr_ec() { r.e = r.c; m = 1; }
void Cpu::LDrr_ed() { r.e = r.d; m = 1; }
void Cpu::LDrr_ee() { r.e = r.e; m = 1; }
void Cpu::LDrr_eh() { r.e = r.h; m = 1; }
void Cpu::LDrr_el() { r.e = r.l; m = 1; }
void Cpu::LDrr_ea() { r.e = r.a; m = 1; }
void Cpu::LDrr_hb() { r.h = r.b; m = 1; }
void Cpu::LDrr_hc() { r.h = r.c; m = 1; }
void Cpu::LDrr_hd() { r.h = r.d; m = 1; }
void Cpu::LDrr_he() { r.h = r.e; m = 1; }
void Cpu::LDrr_hh() { r.h = r.h; m = 1; }
void Cpu::LDrr_hl() { r.h = r.l; m = 1; }
void Cpu::LDrr_ha() { r.h = r.a; m = 1; }
void Cpu::LDrr_lb() { r.l = r.b; m = 1; }
void Cpu::LDrr_lc() { r.l = r.c; m = 1; }
void Cpu::LDrr_ld() { r.l = r.d; m = 1; }
void Cpu::LDrr_le() { r.l = r.e; m = 1; }
void Cpu::LDrr_lh() { r.l = r.h; m = 1; }
void Cpu::LDrr_ll() { r.l = r.l; m = 1; }
void Cpu::LDrr_la() { r.l = r.a; m = 1; }
void Cpu::LDrr_ab() { r.a = r.b; m = 1; }
void Cpu::LDrr_ac() { r.a = r.c; m = 1; }
void Cpu::LDrr_ad() { r.a = r.d; m = 1; }
void Cpu::LDrr_ae() { r.a = r.e; m = 1; }
void Cpu::LDrr_ah() { r.a = r.h; m = 1; }
void Cpu::LDrr_al() { r.a = r.l; m = 1; }
void Cpu::LDrr_aa() { r.a = r.a; m = 1; }

void Cpu::LDrHLm_b() { r.b = _mbc->readByte(r.hl); m = 2; }
void Cpu::LDrHLm_c() { r.c = _mbc->readByte(r.hl); m = 2; }
void Cpu::LDrHLm_d() { r.d = _mbc->readByte(r.hl); m = 2; }
void Cpu::LDrHLm_e() { r.e = _mbc->readByte(r.hl); m = 2; }
void Cpu::LDrHLm_h() { r.h = _mbc->readByte(r.hl); m = 2; }
void Cpu::LDrHLm_l() { r.l = _mbc->readByte(r.hl); m = 2; }
void Cpu::LDrHLm_a() { r.a = _mbc->readByte(r.hl); m = 2; }

void Cpu::LDHLmr_b() { _mbc->writeByte(r.hl, r.b); m = 2; }
void Cpu::LDHLmr_c() { _mbc->writeByte(r.hl, r.c); m = 2; }
void Cpu::LDHLmr_d() { _mbc->writeByte(r.hl, r.d); m = 2; }
void Cpu::LDHLmr_e() { _mbc->writeByte(r.hl, r.e); m = 2; }
void Cpu::LDHLmr_h() { _mbc->writeByte(r.hl, r.h); m = 2; }
void Cpu::LDHLmr_l() { _mbc->writeByte(r.hl, r.l); m = 2; }
void Cpu::LDHLmr_a() { _mbc->writeByte(r.hl, r.a); m = 2; }

void Cpu::LDrn_b() { r.b = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_c() { r.c = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_d() { r.d = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_e() { r.e = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_h() { r.h = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_l() { r.l = _mbc->readByte(pc++); m = 2; }
void Cpu::LDrn_a() { r.a = _mbc->readByte(pc++); m = 2; }

void Cpu::LDHLmn() { _mbc->writeByte(r.hl, _mbc->readByte(pc++)); m = 3; }

void Cpu::LDBCmA() { _mbc->writeByte(r.bc, r.a); m = 2; }
void Cpu::LDDEmA() { _mbc->writeByte(r.de, r.a); m = 2; }

void Cpu::LDmmA() { _mbc->writeByte(_mbc->readShort(pc++), r.a); pc++; m = 4; }

void Cpu::LDABCm() { r.a = _mbc->readByte(r.bc); m = 2; }
void Cpu::LDADEm() { r.a = _mbc->readByte(r.de); m = 2; }

void Cpu::LDAmm() { r.a = _mbc->readByte( _mbc->readShort(pc++)); pc++; m = 4; }

void Cpu::LDBCnn() { r.c = _mbc->readByte(pc++); r.b = _mbc->readByte(pc++); m = 3; }
void Cpu::LDDEnn() { r.e = _mbc->readByte(pc++); r.d = _mbc->readByte(pc++); m = 3; }
void Cpu::LDHLnn() { r.l = _mbc->readByte(pc++); r.h = _mbc->readByte(pc++); m = 3; }
void Cpu::LDSPnn() { sp = _mbc->readShort(pc++); pc++; m = 3; }

void Cpu::LDHLIA() { _mbc->writeByte(r.hl, r.a); r.hl++; m = 2; }
void Cpu::LDAHLI() { r.a = _mbc->readByte(r.hl); r.hl++; m = 2; }

void Cpu::LDHLDA() { _mbc->writeByte(r.hl, r.a); r.hl--; m = 2; }
void Cpu::LDAHLD() { r.a = _mbc->readByte(r.hl); r.hl--; m = 2; }

void Cpu::LDAIOn() { r.a = _mbc->readByte(0xFF00 + _mbc->readByte(pc++)); m = 3; }
void Cpu::LDIOnA() { _mbc->writeByte(0xFF00 + _mbc->readByte(pc++), r.a); m = 3; }
void Cpu::LDAIOC() { r.a = _mbc->readByte(0xFF00 + r.c); m = 2; }
void Cpu::LDIOCA() { _mbc->writeByte(0xFF00 + r.c, r.a); m = 2; }

void Cpu::LDHLSPn() { unsigned int rsp = sp; char v = (char)_mbc->readByte(pc++); rsp += v; COMPUTE_CARRY_FLAGS_16_8(sp, v, rsp); RESET_FLAG(Z_FLAG); r.hl = (uint16_t)rsp; m = 3; }

void Cpu::LDSPHL() { sp = r.hl; m = 2; }

void Cpu::LDmmSP() { auto mm = _mbc->readShort(pc++); pc++; _mbc->writeShort(mm, sp); m = 5; }

void Cpu::SWAPr_b() { auto v = r.b; r.b = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_c() { auto v = r.c; r.c = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_d() { auto v = r.d; r.d = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_e() { auto v = r.e; r.e = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_h() { auto v = r.h; r.h = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_l() { auto v = r.l; r.l = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPr_a() { auto v = r.a; r.a = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); m = 2; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }
void Cpu::SWAPHL() { auto v = _mbc->readByte(r.hl); v = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4); _mbc->writeByte(r.hl, v); m = 4; RESET_FLAG(ALL_FLAGS); if(!v) SET_FLAG(Z_FLAG); }

/* Data processing */

void Cpu::ADDr_b() { unsigned short a = r.a; a += r.b; COMPUTE_CARRY_FLAGS_8(r.a, r.b, a); RESET_FLAG(N_FLAG); r.a = (uint8_t)a; m = 1; }
void Cpu::ADDr_c() { unsigned short a = r.a; a += r.c; COMPUTE_CARRY_FLAGS_8(r.a, r.c, a); RESET_FLAG(N_FLAG); r.a = (uint8_t)a; m = 1; }
void Cpu::ADDr_d() { unsigned short a = r.a; a += r.d; COMPUTE_CARRY_FLAGS_8(r.a, r.d, a); RESET_FLAG(N_FLAG); r.a = (uint8_t)a; m = 1; }
void Cpu::ADDr_e() { unsigned short a = r.a; a += r.e; COMPUTE_CARRY_FLAGS_8(r.a, r.e, a); RESET_FLAG(N_FLAG); r.a = (uint8_t)a; m = 1; }
void Cpu::ADDr_h() { unsigned short a = r.a; a += r.h; COMPUTE_CARRY_FLAGS_8(r.a, r.h, a); RESET_FLAG(N_FLAG); r.a = (uint8_t)a; m = 1; }
void Cpu::ADDr_l() { unsigned short a = r.a; a += r.l; COMPUTE_CARRY_FLAGS_8(r.a, r.l, a); RESET_FLAG(N_FLAG); r.a = (uint8_t)a; m = 1; }
void Cpu::ADDr_a() { unsigned short a = r.a; a += r.a; COMPUTE_CARRY_FLAGS_8(r.a, r.a, a); RESET_FLAG(N_FLAG); r.a = (uint8_t)a; m = 1; }
void Cpu::ADDHL() { unsigned short a = r.a; unsigned short v = _mbc->readByte(r.hl); a += v; COMPUTE_CARRY_FLAGS_8(r.a, v, a); RESET_FLAG(N_FLAG); r.a = (uint8_t)a; m = 2; }
void Cpu::ADDn() { unsigned short a = r.a; unsigned short v = _mbc->readByte(pc++); a += v; COMPUTE_CARRY_FLAGS_8(r.a, v, a); RESET_FLAG(N_FLAG); r.a = (uint8_t)a; m = 2; }
void Cpu::ADDHLBC() { unsigned int rhl = r.hl; rhl += r.bc; COMPUTE_CARRY_FLAGS_16(r.hl, r.bc, rhl); r.hl = (uint16_t)rhl; m = 2; }
void Cpu::ADDHLDE() { unsigned int rhl = r.hl; rhl += r.de; COMPUTE_CARRY_FLAGS_16(r.hl, r.de, rhl); r.hl = (uint16_t)rhl; m = 2; }
void Cpu::ADDHLHL() { unsigned int rhl = r.hl; rhl += r.hl; COMPUTE_CARRY_FLAGS_16(r.hl, r.hl, rhl); r.hl = (uint16_t)rhl; m = 2; }
void Cpu::ADDHLSP() { unsigned int rhl = r.hl; rhl += sp; COMPUTE_CARRY_FLAGS_16(r.hl, sp, rhl); r.hl = (uint16_t)rhl; m = 2; }
void Cpu::ADDSPn() { unsigned int rsp = sp; char v = (char)_mbc->readByte(pc++); rsp += v; COMPUTE_CARRY_FLAGS_16_8(sp, v, rsp); RESET_FLAG(Z_FLAG); sp = (uint16_t)rsp; m = 4; }

void Cpu::ADCr_b() { unsigned short ca = r.a + ((r.f & 0x10) >> 4); unsigned short sa = ca + r.b; COMPUTE_CARRY_FLAGS_8(r.a, r.b, sa); RESET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::ADCr_c() { unsigned short ca = r.a + ((r.f & 0x10) >> 4); unsigned short sa = ca + r.c; COMPUTE_CARRY_FLAGS_8(r.a, r.c, sa); RESET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::ADCr_d() { unsigned short ca = r.a + ((r.f & 0x10) >> 4); unsigned short sa = ca + r.d; COMPUTE_CARRY_FLAGS_8(r.a, r.d, sa); RESET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::ADCr_e() { unsigned short ca = r.a + ((r.f & 0x10) >> 4); unsigned short sa = ca + r.e; COMPUTE_CARRY_FLAGS_8(r.a, r.e, sa); RESET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::ADCr_h() { unsigned short ca = r.a + ((r.f & 0x10) >> 4); unsigned short sa = ca + r.h; COMPUTE_CARRY_FLAGS_8(r.a, r.h, sa); RESET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::ADCr_l() { unsigned short ca = r.a + ((r.f & 0x10) >> 4); unsigned short sa = ca + r.l; COMPUTE_CARRY_FLAGS_8(r.a, r.l, sa); RESET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::ADCr_a() { unsigned short ca = r.a + ((r.f & 0x10) >> 4); unsigned short sa = ca + r.a; COMPUTE_CARRY_FLAGS_8(r.a, r.a, sa); RESET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::ADCHL() { unsigned short ca = r.a + ((r.f & 0x10) >> 4); unsigned short v = _mbc->readByte(r.hl); unsigned short sa = ca + v; COMPUTE_CARRY_FLAGS_8(r.a, v, sa); RESET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 2; }
void Cpu::ADCn() { unsigned short ca = r.a + ((r.f & 0x10) >> 4); unsigned short v = _mbc->readByte(pc++); unsigned short sa = ca + v; COMPUTE_CARRY_FLAGS_8(r.a, v, sa); RESET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 2; }

void Cpu::SUBr_b() { unsigned short a = r.a; a -= r.b; COMPUTE_CARRY_FLAGS_8(r.a, r.b, a); r.a = (uint8_t)a; SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_c() { unsigned short a = r.a; a -= r.c; COMPUTE_CARRY_FLAGS_8(r.a, r.c, a); r.a = (uint8_t)a; SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_d() { unsigned short a = r.a; a -= r.d; COMPUTE_CARRY_FLAGS_8(r.a, r.d, a); r.a = (uint8_t)a; SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_e() { unsigned short a = r.a; a -= r.e; COMPUTE_CARRY_FLAGS_8(r.a, r.e, a); r.a = (uint8_t)a; SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_h() { unsigned short a = r.a; a -= r.h; COMPUTE_CARRY_FLAGS_8(r.a, r.h, a); r.a = (uint8_t)a; SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_l() { unsigned short a = r.a; a -= r.l; COMPUTE_CARRY_FLAGS_8(r.a, r.l, a); r.a = (uint8_t)a; SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBr_a() { unsigned short a = r.a; a -= r.a; COMPUTE_CARRY_FLAGS_8(r.a, r.a, a); r.a = (uint8_t)a; SET_FLAG(N_FLAG); m = 1; }
void Cpu::SUBHL() { unsigned short a = r.a; unsigned short v = _mbc->readByte(r.hl); a -= v; COMPUTE_CARRY_FLAGS_8(r.a, v, a); r.a = (uint8_t)a; SET_FLAG(N_FLAG); m = 2; }
void Cpu::SUBn() { unsigned short a = r.a; unsigned short v = _mbc->readByte(pc++); a -= v; COMPUTE_CARRY_FLAGS_8(r.a, v, a); r.a = (uint8_t)a; SET_FLAG(N_FLAG); m = 2; }

void Cpu::SBCr_b() { unsigned short ca = r.a - ((r.f & 0x10) >> 4); unsigned short sa = ca - r.b; COMPUTE_CARRY_FLAGS_8(r.a, r.b, sa); SET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::SBCr_c() { unsigned short ca = r.a - ((r.f & 0x10) >> 4); unsigned short sa = ca - r.c; COMPUTE_CARRY_FLAGS_8(r.a, r.c, sa); SET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::SBCr_d() { unsigned short ca = r.a - ((r.f & 0x10) >> 4); unsigned short sa = ca - r.d; COMPUTE_CARRY_FLAGS_8(r.a, r.d, sa); SET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::SBCr_e() { unsigned short ca = r.a - ((r.f & 0x10) >> 4); unsigned short sa = ca - r.e; COMPUTE_CARRY_FLAGS_8(r.a, r.e, sa); SET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::SBCr_h() { unsigned short ca = r.a - ((r.f & 0x10) >> 4); unsigned short sa = ca - r.h; COMPUTE_CARRY_FLAGS_8(r.a, r.h, sa); SET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::SBCr_l() { unsigned short ca = r.a - ((r.f & 0x10) >> 4); unsigned short sa = ca - r.l; COMPUTE_CARRY_FLAGS_8(r.a, r.l, sa); SET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::SBCr_a() { unsigned short ca = r.a - ((r.f & 0x10) >> 4); unsigned short sa = ca - r.a; COMPUTE_CARRY_FLAGS_8(r.a, r.a, sa); SET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 1; }
void Cpu::SBCHL() { unsigned short ca = r.a - ((r.f & 0x10) >> 4); unsigned short v = _mbc->readByte(r.hl); unsigned short sa = ca - v; COMPUTE_CARRY_FLAGS_8(r.a, v, sa); SET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 2; }
void Cpu::SBCn() { unsigned short ca = r.a - ((r.f & 0x10) >> 4); unsigned short v = _mbc->readByte(pc++); unsigned short sa = ca - v; COMPUTE_CARRY_FLAGS_8(r.a, v, sa); SET_FLAG(N_FLAG); r.a = (uint8_t)sa; m = 2; }

void Cpu::CPr_b() { unsigned short a = r.a; unsigned short s = a - r.b; COMPUTE_CARRY_FLAGS_8(a, r.b, s); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_c() { unsigned short a = r.a; unsigned short s = a - r.c; COMPUTE_CARRY_FLAGS_8(a, r.c, s); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_d() { unsigned short a = r.a; unsigned short s = a - r.d; COMPUTE_CARRY_FLAGS_8(a, r.d, s); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_e() { unsigned short a = r.a; unsigned short s = a - r.e; COMPUTE_CARRY_FLAGS_8(a, r.e, s); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_h() { unsigned short a = r.a; unsigned short s = a - r.h; COMPUTE_CARRY_FLAGS_8(a, r.h, s); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_l() { unsigned short a = r.a; unsigned short s = a - r.l; COMPUTE_CARRY_FLAGS_8(a, r.l, s); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPr_a() { unsigned short a = r.a; unsigned short s = a - r.a; COMPUTE_CARRY_FLAGS_8(a, a, s); SET_FLAG(N_FLAG); m = 1; }
void Cpu::CPHL() { unsigned short a = r.a; unsigned short v = _mbc->readByte(r.hl); unsigned short s = a - v; COMPUTE_CARRY_FLAGS_8(a, v, s); SET_FLAG(N_FLAG); m = 2; }
void Cpu::CPn() { unsigned short a = r.a; unsigned short v = _mbc->readByte(pc++); unsigned short s = a - v; COMPUTE_CARRY_FLAGS_8(a, v, s); SET_FLAG(N_FLAG); m = 2; }

void Cpu::ANDr_b() { r.a &= r.b; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_c() { r.a &= r.c; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_d() { r.a &= r.d; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_e() { r.a &= r.e; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_h() { r.a &= r.h; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_l() { r.a &= r.l; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDr_a() { r.a &= r.a; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 1; }
void Cpu::ANDHL() { r.a &= _mbc->readByte(r.hl); RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 2; }
void Cpu::ANDn() { r.a &= _mbc->readByte(pc++); RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); SET_FLAG(H_FLAG); m = 2; }

void Cpu::ORr_b() { r.a |= r.b; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_c() { r.a |= r.c; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_d() { r.a |= r.d; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_e() { r.a |= r.e; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_h() { r.a |= r.h; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_l() { r.a |= r.l; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORr_a() { r.a |= r.a; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::ORHL() { r.a |= _mbc->readByte(r.hl); RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::ORn() { r.a |= _mbc->readByte(pc++); RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 2; }

void Cpu::XORr_b() { r.a ^= r.b; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_c() { r.a ^= r.c; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_d() { r.a ^= r.d; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_e() { r.a ^= r.e; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_h() { r.a ^= r.h; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_l() { r.a ^= r.l; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORr_a() { r.a ^= r.a; RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 1; }
void Cpu::XORHL() { r.a ^= _mbc->readByte(r.hl); RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::XORn() { r.a ^= _mbc->readByte(pc++); RESET_FLAG(ALL_FLAGS); if(!r.a) SET_FLAG(Z_FLAG); m = 2; }

void Cpu::INCr_b() { INC_REG(r.b); m = 1; }
void Cpu::INCr_c() { INC_REG(r.c); m = 1; }
void Cpu::INCr_d() { INC_REG(r.d); m = 1; }
void Cpu::INCr_e() { INC_REG(r.e); m = 1; }
void Cpu::INCr_h() { INC_REG(r.h); m = 1; }
void Cpu::INCr_l() { INC_REG(r.l); m = 1; }
void Cpu::INCr_a() { INC_REG(r.a); m = 1; }
void Cpu::INCHLm() { auto mem = _mbc->readByte(r.hl); INC_REG(mem); _mbc->writeByte(r.hl, mem); m = 3; }

void Cpu::DECr_b() { DEC_REG(r.b); m = 1; }
void Cpu::DECr_c() { DEC_REG(r.c); m = 1; }
void Cpu::DECr_d() { DEC_REG(r.d); m = 1; }
void Cpu::DECr_e() { DEC_REG(r.e); m = 1; }
void Cpu::DECr_h() { DEC_REG(r.h); m = 1; }
void Cpu::DECr_l() { DEC_REG(r.l); m = 1; }
void Cpu::DECr_a() { DEC_REG(r.a); m = 1; }
void Cpu::DECHLm() { auto mem = _mbc->readByte(r.hl); DEC_REG(mem); _mbc->writeByte(r.hl, mem); m = 3; }

void Cpu::INCBC() { r.bc++; m = 2; }
void Cpu::INCDE() { r.de++; m = 2; }
void Cpu::INCHL() { r.hl++; m = 2; }
void Cpu::INCSP() { sp++; m = 2; }

void Cpu::DECBC() { r.bc--; m = 2; }
void Cpu::DECDE() { r.de--; m = 2; }
void Cpu::DECHL() { r.hl--; m = 2; }
void Cpu::DECSP() { sp--; m = 2; }

void Cpu::DAA() {
	static int counter = 0;


	unsigned short s = r.a;

	bool n = (r.f & N_FLAG) != 0;
	bool h = (r.f & H_FLAG) != 0;
	bool c = (r.f & C_FLAG) != 0;

	if (n) {
		if (h) s = (s - 0x06) & 0xFF;
		if (c) s -= 0x60;
	}
	else
	{
		if (h || (s & 0x0F) > 9) s += 0x06;
		if (c || s > 0x9F) s += 0x60;
	}

	r.a = (uint8_t)s;
	RESET_FLAG(H_FLAG);
	if (r.a) 
		RESET_FLAG(Z_FLAG);
	else 
		SET_FLAG(Z_FLAG);

	if (s >= 0x100)
		SET_FLAG(C_FLAG);

	m = 1;
}

/* Bit manipulation */
void Cpu::BIT0b() { BIT_CHECK(r.b, 0); m = 2; }
void Cpu::BIT0c() { BIT_CHECK(r.c, 0); m = 2; }
void Cpu::BIT0d() { BIT_CHECK(r.d, 0); m = 2; }
void Cpu::BIT0e() { BIT_CHECK(r.e, 0); m = 2; }
void Cpu::BIT0h() { BIT_CHECK(r.h, 0); m = 2; }
void Cpu::BIT0l() { BIT_CHECK(r.l, 0); m = 2; }
void Cpu::BIT0a() { BIT_CHECK(r.a, 0); m = 2; }
void Cpu::BIT0m() { BIT_CHECK(_mbc->readByte(r.hl), 0); m = 3; }

void Cpu::RES0b() { r.b &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0c() { r.c &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0d() { r.d &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0e() { r.e &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0h() { r.h &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0l() { r.l &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0a() { r.a &= (0xFF ^ (1 << 0)); m = 2; }
void Cpu::RES0m() { auto mem = _mbc->readByte(r.hl); mem &= (0xFF ^ (1 << 0)); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::SET0b() { r.b |= (1 << 0); m = 2; }
void Cpu::SET0c() { r.c |= (1 << 0); m = 2; }
void Cpu::SET0d() { r.d |= (1 << 0); m = 2; }
void Cpu::SET0e() { r.e |= (1 << 0); m = 2; }
void Cpu::SET0h() { r.h |= (1 << 0); m = 2; }
void Cpu::SET0l() { r.l |= (1 << 0); m = 2; }
void Cpu::SET0a() { r.a |= (1 << 0); m = 2; }
void Cpu::SET0m() { auto mem = _mbc->readByte(r.hl); mem |= (1 << 0); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::BIT1b() { BIT_CHECK(r.b, 1); m = 2; }
void Cpu::BIT1c() { BIT_CHECK(r.c, 1); m = 2; }
void Cpu::BIT1d() { BIT_CHECK(r.d, 1); m = 2; }
void Cpu::BIT1e() { BIT_CHECK(r.e, 1); m = 2; }
void Cpu::BIT1h() { BIT_CHECK(r.h, 1); m = 2; }
void Cpu::BIT1l() { BIT_CHECK(r.l, 1); m = 2; }
void Cpu::BIT1a() { BIT_CHECK(r.a, 1); m = 2; }
void Cpu::BIT1m() { BIT_CHECK(_mbc->readByte(r.hl), 1); m = 3; }

void Cpu::RES1b() { r.b &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1c() { r.c &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1d() { r.d &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1e() { r.e &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1h() { r.h &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1l() { r.l &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1a() { r.a &= (0xFF ^ (1 << 1)); m = 2; }
void Cpu::RES1m() { auto mem = _mbc->readByte(r.hl); mem &= (0xFF ^ (1 << 1)); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::SET1b() { r.b |= (1 << 1); m = 2; }
void Cpu::SET1c() { r.c |= (1 << 1); m = 2; }
void Cpu::SET1d() { r.d |= (1 << 1); m = 2; }
void Cpu::SET1e() { r.e |= (1 << 1); m = 2; }
void Cpu::SET1h() { r.h |= (1 << 1); m = 2; }
void Cpu::SET1l() { r.l |= (1 << 1); m = 2; }
void Cpu::SET1a() { r.a |= (1 << 1); m = 2; }
void Cpu::SET1m() { auto mem = _mbc->readByte(r.hl); mem |= (1 << 1); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::BIT2b() { BIT_CHECK(r.b, 2); m = 2; }
void Cpu::BIT2c() { BIT_CHECK(r.c, 2); m = 2; }
void Cpu::BIT2d() { BIT_CHECK(r.d, 2); m = 2; }
void Cpu::BIT2e() { BIT_CHECK(r.e, 2); m = 2; }
void Cpu::BIT2h() { BIT_CHECK(r.h, 2); m = 2; }
void Cpu::BIT2l() { BIT_CHECK(r.l, 2); m = 2; }
void Cpu::BIT2a() { BIT_CHECK(r.a, 2); m = 2; }
void Cpu::BIT2m() { BIT_CHECK(_mbc->readByte(r.hl), 2); m = 3; }

void Cpu::RES2b() { r.b &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2c() { r.c &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2d() { r.d &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2e() { r.e &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2h() { r.h &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2l() { r.l &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2a() { r.a &= (0xFF ^ (1 << 2)); m = 2; }
void Cpu::RES2m() { auto mem = _mbc->readByte(r.hl); mem &= (0xFF ^ (1 << 2)); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::SET2b() { r.b |= (1 << 2); m = 2; }
void Cpu::SET2c() { r.c |= (1 << 2); m = 2; }
void Cpu::SET2d() { r.d |= (1 << 2); m = 2; }
void Cpu::SET2e() { r.e |= (1 << 2); m = 2; }
void Cpu::SET2h() { r.h |= (1 << 2); m = 2; }
void Cpu::SET2l() { r.l |= (1 << 2); m = 2; }
void Cpu::SET2a() { r.a |= (1 << 2); m = 2; }
void Cpu::SET2m() { auto mem = _mbc->readByte(r.hl); mem |= (1 << 2); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::BIT3b() { BIT_CHECK(r.b, 3); m = 2; }
void Cpu::BIT3c() { BIT_CHECK(r.c, 3); m = 2; }
void Cpu::BIT3d() { BIT_CHECK(r.d, 3); m = 2; }
void Cpu::BIT3e() { BIT_CHECK(r.e, 3); m = 2; }
void Cpu::BIT3h() { BIT_CHECK(r.h, 3); m = 2; }
void Cpu::BIT3l() { BIT_CHECK(r.l, 3); m = 2; }
void Cpu::BIT3a() { BIT_CHECK(r.a, 3); m = 2; }
void Cpu::BIT3m() { BIT_CHECK(_mbc->readByte(r.hl), 3); m = 3; }

void Cpu::RES3b() { r.b &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3c() { r.c &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3d() { r.d &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3e() { r.e &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3h() { r.h &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3l() { r.l &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3a() { r.a &= (0xFF ^ (1 << 3)); m = 2; }
void Cpu::RES3m() { auto mem = _mbc->readByte(r.hl); mem &= (0xFF ^ (1 << 3)); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::SET3b() { r.b |= (1 << 3); m = 2; }
void Cpu::SET3c() { r.c |= (1 << 3); m = 2; }
void Cpu::SET3d() { r.d |= (1 << 3); m = 2; }
void Cpu::SET3e() { r.e |= (1 << 3); m = 2; }
void Cpu::SET3h() { r.h |= (1 << 3); m = 2; }
void Cpu::SET3l() { r.l |= (1 << 3); m = 2; }
void Cpu::SET3a() { r.a |= (1 << 3); m = 2; }
void Cpu::SET3m() { auto mem = _mbc->readByte(r.hl); mem |= (1 << 3); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::BIT4b() { BIT_CHECK(r.b, 4); m = 2; }
void Cpu::BIT4c() { BIT_CHECK(r.c, 4); m = 2; }
void Cpu::BIT4d() { BIT_CHECK(r.d, 4); m = 2; }
void Cpu::BIT4e() { BIT_CHECK(r.e, 4); m = 2; }
void Cpu::BIT4h() { BIT_CHECK(r.h, 4); m = 2; }
void Cpu::BIT4l() { BIT_CHECK(r.l, 4); m = 2; }
void Cpu::BIT4a() { BIT_CHECK(r.a, 4); m = 2; }
void Cpu::BIT4m() { BIT_CHECK(_mbc->readByte(r.hl), 4); m = 3; }

void Cpu::RES4b() { r.b &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4c() { r.c &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4d() { r.d &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4e() { r.e &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4h() { r.h &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4l() { r.l &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4a() { r.a &= (0xFF ^ (1 << 4)); m = 2; }
void Cpu::RES4m() { auto mem = _mbc->readByte(r.hl); mem &= (0xFF ^ (1 << 4)); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::SET4b() { r.b |= (1 << 4); m = 2; }
void Cpu::SET4c() { r.c |= (1 << 4); m = 2; }
void Cpu::SET4d() { r.d |= (1 << 4); m = 2; }
void Cpu::SET4e() { r.e |= (1 << 4); m = 2; }
void Cpu::SET4h() { r.h |= (1 << 4); m = 2; }
void Cpu::SET4l() { r.l |= (1 << 4); m = 2; }
void Cpu::SET4a() { r.a |= (1 << 4); m = 2; }
void Cpu::SET4m() { auto mem = _mbc->readByte(r.hl); mem |= (1 << 4); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::BIT5b() { BIT_CHECK(r.b, 5); m = 2; }
void Cpu::BIT5c() { BIT_CHECK(r.c, 5); m = 2; }
void Cpu::BIT5d() { BIT_CHECK(r.d, 5); m = 2; }
void Cpu::BIT5e() { BIT_CHECK(r.e, 5); m = 2; }
void Cpu::BIT5h() { BIT_CHECK(r.h, 5); m = 2; }
void Cpu::BIT5l() { BIT_CHECK(r.l, 5); m = 2; }
void Cpu::BIT5a() { BIT_CHECK(r.a, 5); m = 2; }
void Cpu::BIT5m() { BIT_CHECK(_mbc->readByte(r.hl), 5); m = 3; }

void Cpu::RES5b() { r.b &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5c() { r.c &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5d() { r.d &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5e() { r.e &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5h() { r.h &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5l() { r.l &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5a() { r.a &= (0xFF ^ (1 << 5)); m = 2; }
void Cpu::RES5m() { auto mem = _mbc->readByte(r.hl); mem &= (0xFF ^ (1 << 5)); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::SET5b() { r.b |= (1 << 5); m = 2; }
void Cpu::SET5c() { r.c |= (1 << 5); m = 2; }
void Cpu::SET5d() { r.d |= (1 << 5); m = 2; }
void Cpu::SET5e() { r.e |= (1 << 5); m = 2; }
void Cpu::SET5h() { r.h |= (1 << 5); m = 2; }
void Cpu::SET5l() { r.l |= (1 << 5); m = 2; }
void Cpu::SET5a() { r.a |= (1 << 5); m = 2; }
void Cpu::SET5m() { auto mem = _mbc->readByte(r.hl); mem |= (1 << 5); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::BIT6b() { BIT_CHECK(r.b, 6); m = 2; }
void Cpu::BIT6c() { BIT_CHECK(r.c, 6); m = 2; }
void Cpu::BIT6d() { BIT_CHECK(r.d, 6); m = 2; }
void Cpu::BIT6e() { BIT_CHECK(r.e, 6); m = 2; }
void Cpu::BIT6h() { BIT_CHECK(r.h, 6); m = 2; }
void Cpu::BIT6l() { BIT_CHECK(r.l, 6); m = 2; }
void Cpu::BIT6a() { BIT_CHECK(r.a, 6); m = 2; }
void Cpu::BIT6m() { BIT_CHECK(_mbc->readByte(r.hl), 6); m = 3; }

void Cpu::RES6b() { r.b &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6c() { r.c &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6d() { r.d &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6e() { r.e &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6h() { r.h &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6l() { r.l &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6a() { r.a &= (0xFF ^ (1 << 6)); m = 2; }
void Cpu::RES6m() { auto mem = _mbc->readByte(r.hl); mem &= (0xFF ^ (1 << 6)); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::SET6b() { r.b |= (1 << 6); m = 2; }
void Cpu::SET6c() { r.c |= (1 << 6); m = 2; }
void Cpu::SET6d() { r.d |= (1 << 6); m = 2; }
void Cpu::SET6e() { r.e |= (1 << 6); m = 2; }
void Cpu::SET6h() { r.h |= (1 << 6); m = 2; }
void Cpu::SET6l() { r.l |= (1 << 6); m = 2; }
void Cpu::SET6a() { r.a |= (1 << 6); m = 2; }
void Cpu::SET6m() { auto mem = _mbc->readByte(r.hl); mem |= (1 << 6); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::BIT7b() { BIT_CHECK(r.b, 7); m = 2; }
void Cpu::BIT7c() { BIT_CHECK(r.c, 7); m = 2; }
void Cpu::BIT7d() { BIT_CHECK(r.d, 7); m = 2; }
void Cpu::BIT7e() { BIT_CHECK(r.e, 7); m = 2; }
void Cpu::BIT7h() { BIT_CHECK(r.h, 7); m = 2; }
void Cpu::BIT7l() { BIT_CHECK(r.l, 7); m = 2; }
void Cpu::BIT7a() { BIT_CHECK(r.a, 7); m = 2; }
void Cpu::BIT7m() { BIT_CHECK(_mbc->readByte(r.hl), 7); m = 3; }

void Cpu::RES7b() { r.b &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7c() { r.c &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7d() { r.d &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7e() { r.e &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7h() { r.h &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7l() { r.l &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7a() { r.a &= (0xFF ^ (1 << 7)); m = 2; }
void Cpu::RES7m() { auto mem = _mbc->readByte(r.hl); mem &= (0xFF ^ (1 << 7)); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::SET7b() { r.b |= (1 << 7); m = 2; }
void Cpu::SET7c() { r.c |= (1 << 7); m = 2; }
void Cpu::SET7d() { r.d |= (1 << 7); m = 2; }
void Cpu::SET7e() { r.e |= (1 << 7); m = 2; }
void Cpu::SET7h() { r.h |= (1 << 7); m = 2; }
void Cpu::SET7l() { r.l |= (1 << 7); m = 2; }
void Cpu::SET7a() { r.a |= (1 << 7); m = 2; }
void Cpu::SET7m() { auto mem = _mbc->readByte(r.hl); mem |= (1 << 7); _mbc->writeByte(r.hl, mem); m = 4; }

void Cpu::RLA() { ROTATE_LEFT_THROUGH_CARRY(r.a); m = 1; }
void Cpu::RLCA() { ROTATE_LEFT(r.a); m = 1; }
void Cpu::RRA() { ROTATE_RIGHT_THROUGH_CARRY(r.a); m = 1;  }
void Cpu::RRCA() { ROTATE_RIGHT(r.a); m = 1; }

void Cpu::RLr_b() { ROTATE_LEFT_THROUGH_CARRY(r.b); if (!r.b) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLr_c() { ROTATE_LEFT_THROUGH_CARRY(r.c); if (!r.c) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLr_d() { ROTATE_LEFT_THROUGH_CARRY(r.d); if (!r.d) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLr_e() { ROTATE_LEFT_THROUGH_CARRY(r.e); if (!r.e) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLr_h() { ROTATE_LEFT_THROUGH_CARRY(r.h); if (!r.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLr_l() { ROTATE_LEFT_THROUGH_CARRY(r.l); if (!r.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLr_a() { ROTATE_LEFT_THROUGH_CARRY(r.a); if (!r.a) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLHL() { unsigned char v = _mbc->readByte(r.hl); ROTATE_LEFT_THROUGH_CARRY(v); if (!v) SET_FLAG(Z_FLAG); _mbc->writeByte(r.hl, v); m = 4; }

void Cpu::RLCr_b() { ROTATE_LEFT(r.b); if (!r.b) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLCr_c() { ROTATE_LEFT(r.c); if (!r.c) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLCr_d() { ROTATE_LEFT(r.d); if (!r.d) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLCr_e() { ROTATE_LEFT(r.e); if (!r.e) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLCr_h() { ROTATE_LEFT(r.h); if (!r.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLCr_l() { ROTATE_LEFT(r.l); if (!r.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLCr_a() { ROTATE_LEFT(r.a); if (!r.a) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RLCHL() { unsigned char v = _mbc->readByte(r.hl); ROTATE_LEFT(v); if (!v) SET_FLAG(Z_FLAG); _mbc->writeByte(r.hl, v); m = 4; }

void Cpu::RRr_b() { ROTATE_RIGHT_THROUGH_CARRY(r.b); if (!r.b) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRr_c() { ROTATE_RIGHT_THROUGH_CARRY(r.c); if (!r.c) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRr_d() { ROTATE_RIGHT_THROUGH_CARRY(r.d); if (!r.d) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRr_e() { ROTATE_RIGHT_THROUGH_CARRY(r.e); if (!r.e) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRr_h() { ROTATE_RIGHT_THROUGH_CARRY(r.h); if (!r.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRr_l() { ROTATE_RIGHT_THROUGH_CARRY(r.l); if (!r.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRr_a() { ROTATE_RIGHT_THROUGH_CARRY(r.a); if (!r.a) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRHL() { unsigned char v = _mbc->readByte(r.hl); ROTATE_RIGHT_THROUGH_CARRY(v); if (!v) SET_FLAG(Z_FLAG); _mbc->writeByte(r.hl, v); m = 4; }

void Cpu::RRCr_b() { ROTATE_RIGHT(r.b); if (!r.b) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRCr_c() { ROTATE_RIGHT(r.c); if (!r.c) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRCr_d() { ROTATE_RIGHT(r.d); if (!r.d) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRCr_e() { ROTATE_RIGHT(r.e); if (!r.e) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRCr_h() { ROTATE_RIGHT(r.h); if (!r.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRCr_l() { ROTATE_RIGHT(r.l); if (!r.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRCr_a() { ROTATE_RIGHT(r.a); if (!r.a) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::RRCHL() { unsigned char v = _mbc->readByte(r.hl); ROTATE_RIGHT(v); if (!v) SET_FLAG(Z_FLAG); _mbc->writeByte(r.hl, v); m = 4; }

void Cpu::SLAr_b() { r.f = (r.b & 0x80) >> 3; r.b <<= 1; if(!r.b) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_c() { r.f = (r.c & 0x80) >> 3; r.c <<= 1; if(!r.c) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_d() { r.f = (r.d & 0x80) >> 3; r.d <<= 1; if(!r.d) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_e() { r.f = (r.e & 0x80) >> 3; r.e <<= 1; if(!r.e) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_h() { r.f = (r.h & 0x80) >> 3; r.h <<= 1; if(!r.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_l() { r.f = (r.l & 0x80) >> 3; r.l <<= 1; if(!r.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAr_a() { r.f = (r.a & 0x80) >> 3; r.a <<= 1; if(!r.a) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SLAHL() { auto v = _mbc->readByte(r.hl); r.f = (v & 0x80) >> 3; v <<= 1; _mbc->writeByte(r.hl, v); if(!v) SET_FLAG(Z_FLAG); m = 4; }

void Cpu::SRAr_b() { r.f = (r.b & 0x01) << 4; r.b = (r.b >> 1) | (r.b & 0x80); if(!r.b) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_c() { r.f = (r.c & 0x01) << 4; r.c = (r.c >> 1) | (r.c & 0x80); if(!r.c) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_d() { r.f = (r.d & 0x01) << 4; r.d = (r.d >> 1) | (r.d & 0x80); if(!r.d) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_e() { r.f = (r.e & 0x01) << 4; r.e = (r.e >> 1) | (r.e & 0x80); if(!r.e) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_h() { r.f = (r.h & 0x01) << 4; r.h = (r.h >> 1) | (r.h & 0x80); if(!r.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_l() { r.f = (r.l & 0x01) << 4; r.l = (r.l >> 1) | (r.l & 0x80); if(!r.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAr_a() { r.f = (r.a & 0x01) << 4; r.a = (r.a >> 1) | (r.a & 0x80); if(!r.a) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRAHL() { auto v = _mbc->readByte(r.hl); r.f = (v & 0x01) << 4; v = (v >> 1) | (v & 0x80); _mbc->writeByte(r.hl, v); if(!v) SET_FLAG(Z_FLAG); m = 4; }

void Cpu::SRLr_b() { r.f = (r.b & 0x01) << 4; r.b >>= 1; if(!r.b) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_c() { r.f = (r.c & 0x01) << 4; r.c >>= 1; if(!r.c) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_d() { r.f = (r.d & 0x01) << 4; r.d >>= 1; if(!r.d) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_e() { r.f = (r.e & 0x01) << 4; r.e >>= 1; if(!r.e) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_h() { r.f = (r.h & 0x01) << 4; r.h >>= 1; if(!r.h) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_l() { r.f = (r.l & 0x01) << 4; r.l >>= 1; if(!r.l) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLr_a() { r.f = (r.a & 0x01) << 4; r.a >>= 1; if(!r.a) SET_FLAG(Z_FLAG); m = 2; }
void Cpu::SRLHL() { auto v = _mbc->readByte(r.hl); r.f = (v & 0x01) << 4; v >>= 1; _mbc->writeByte(r.hl, v); if(!v) SET_FLAG(Z_FLAG); m = 4; }

void Cpu::CPL() { r.a = ~r.a; SET_FLAG(N_FLAG | H_FLAG); m = 1; }
void Cpu::CCF() { r.f ^= C_FLAG; RESET_FLAG(N_FLAG | H_FLAG); m = 1; }
void Cpu::SCF() { SET_FLAG(C_FLAG); RESET_FLAG(N_FLAG | H_FLAG); m = 1; }

/* Pushs and pops */

void Cpu::PUSHBC() { sp -= 2; _mbc->writeShort(sp, r.bc); m = 4;}
void Cpu::PUSHDE() { sp -= 2; _mbc->writeShort(sp, r.de); m = 4;}
void Cpu::PUSHHL() { sp -= 2; _mbc->writeShort(sp, r.hl); m = 4;}
void Cpu::PUSHAF() { sp -= 2; _mbc->writeShort(sp, r.af); m = 4;}

void Cpu::POPBC() { r.bc = _mbc->readShort(sp); sp+=2; m = 3;}
void Cpu::POPDE() { r.de = _mbc->readShort(sp); sp+=2; m = 3;}
void Cpu::POPHL() { r.hl = _mbc->readShort(sp); sp+=2; m = 3;}
void Cpu::POPAF() { r.af = 0xFFF0 & _mbc->readShort(sp); sp+=2; m = 3;}


/* Jumps */

void Cpu::JPnn() { auto nn = _mbc->readShort(pc++); pc++; pc = nn; m = 4; }
void Cpu::JPHL() { pc = r.hl; m = 1; }
void Cpu::JPNZnn() { auto nn = _mbc->readShort(pc++); pc++; m = 3; if (!(r.f & Z_FLAG)) { pc = nn; ++m; } }
void Cpu::JPZnn() { auto nn = _mbc->readShort(pc++); pc++; m = 3; if (r.f & Z_FLAG) { pc = nn; ++m; } }
void Cpu::JPNCnn() { auto nn = _mbc->readShort(pc++); pc++; m = 3; if (!(r.f & C_FLAG)) { pc = nn; ++m; } }
void Cpu::JPCnn() { auto nn = _mbc->readShort(pc++); pc++; m = 3; if (r.f & C_FLAG) { pc = nn; ++m; } }

void Cpu::JRn() { 
	auto n = (char)_mbc->readByte(pc++); 
	pc += n; 
	m = 3; 
}
void Cpu::JRNZn() { auto n = (char)_mbc->readByte(pc++); m = 2; if (!(r.f & Z_FLAG)) { pc += n; m = 3; } }
void Cpu::JRZn()  { auto n = (char)_mbc->readByte(pc++); m = 2; if (r.f & Z_FLAG)    { pc += n; m = 3; } }
void Cpu::JRNCn() { auto n = (char)_mbc->readByte(pc++); m = 2; if (!(r.f & C_FLAG)) { pc += n; m = 3; } }
void Cpu::JRCn()  { auto n = (char)_mbc->readByte(pc++); m = 2; if (r.f & C_FLAG)    { pc += n; m = 3; } }

void Cpu::CALLnn() { auto nn = _mbc->readShort(pc++); pc++; sp -= 2; _mbc->writeShort(sp, pc); pc = nn; m = 6; }
void Cpu::CALLNZnn() { auto nn = _mbc->readShort(pc++); pc++;  m = 3; if (!(r.f & Z_FLAG)) { sp -= 2; _mbc->writeShort(sp, pc); pc = nn; m = 6; } }
void Cpu::CALLZnn()  { auto nn = _mbc->readShort(pc++); pc++;  m = 3; if (r.f & Z_FLAG)    { sp -= 2; _mbc->writeShort(sp, pc); pc = nn; m = 6; } }
void Cpu::CALLNCnn() { auto nn = _mbc->readShort(pc++); pc++;  m = 3; if (!(r.f & C_FLAG)) { sp -= 2; _mbc->writeShort(sp, pc); pc = nn; m = 6; } }
void Cpu::CALLCnn()  { auto nn = _mbc->readShort(pc++); pc++;  m = 3; if (r.f & C_FLAG)    { sp -= 2; _mbc->writeShort(sp, pc); pc = nn; m = 6; } }

void Cpu::RET() { pc = _mbc->readShort(sp); sp += 2; m = 4; }
void Cpu::RETI() { _interrupts->enableInterrupts(); pc = _mbc->readShort(sp); sp += 2; m = 4; }
void Cpu::RETNZ() { m = 2; if (!(r.f & Z_FLAG)) { pc = _mbc->readShort(sp); sp += 2; m = 5; } }
void Cpu::RETZ()  { m = 2; if (r.f & Z_FLAG)    { pc = _mbc->readShort(sp); sp += 2; m = 5; } }
void Cpu::RETNC() { m = 2; if (!(r.f & C_FLAG)) { pc = _mbc->readShort(sp); sp += 2; m = 5; } }
void Cpu::RETC()  { m = 2; if (r.f & C_FLAG)    { pc = _mbc->readShort(sp); sp += 2; m = 5; } }

void Cpu::RST00() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x00; m = 4; }
void Cpu::RST08() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x08; m = 4; }
void Cpu::RST10() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x10; m = 4; }
void Cpu::RST18() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x18; m = 4; }
void Cpu::RST20() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x20; m = 4; }
void Cpu::RST28() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x28; m = 4; }
void Cpu::RST30() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x30; m = 4; }
void Cpu::RST38() { sp -= 2; _mbc->writeShort(sp, pc); pc = 0x38; m = 4; }

void Cpu::STOP() { 
	halt = true; 
	pc++;
	m = 1; 
}
void Cpu::HALT() { 
	halt = true; 
	m = 1; 
}
void Cpu::DI() { _interrupts->disableInterrupts(); m = 1; }
void Cpu::EI() { _interrupts->enableInterrupts(); m = 1; }
