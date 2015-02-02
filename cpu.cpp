#include "cpu.h"

Cpu::opinfo_t Cpu::opmap[256] = {
        // 0x00
        { &Cpu::NOP, "NOP", 1 },
        { &Cpu::LDBCnn, "LDBCnn", 3 },
        { &Cpu::LDBCmA, "LDBCmA", 2 },
        { &Cpu::NOP, "NCBC", 1 },
        { &Cpu::NOP, "NCr_b", 1 },
        { &Cpu::NOP, "ECr_b", 1 },
        { &Cpu::LDrn_b, "LDrn_b", 2 },
        { &Cpu::NOP, "RLCA", 1 },
        { &Cpu::NOP, "LDmmSP", 3 },
        { &Cpu::ADDHLBC, "ADDHLBC", 1 },
        { &Cpu::LDABCm, "LDABCm", 1 },
        { &Cpu::NOP, "DECBC", 1 },
        { &Cpu::NOP, "INCr_c", 1 },
        { &Cpu::NOP, "DECr_c", 1 },
        { &Cpu::LDrn_c, "LDrn_c", 2 },
        { &Cpu::NOP, "RRCA", 1 },

        // 0x10
        { &Cpu::NOP, "DJNZn", 2 },
        { &Cpu::LDDEnn, "LDDEnn", 3 },
        { &Cpu::LDDEmA, "LDDEmA", 2 },
        { &Cpu::NOP, "INCDE", 1 },
        { &Cpu::NOP, "INCr_d", 1 },
        { &Cpu::NOP, "DECr_d", 1 },
        { &Cpu::LDrn_d, "LDrn_d", 2 },
        { &Cpu::NOP, "RLA", 1 },
        { &Cpu::NOP, "JRn", 2 },
        { &Cpu::ADDHLDE, "ADDHLDE", 1 },
        { &Cpu::LDADEm, "LDADEm", 1 },
        { &Cpu::NOP, "DECDE", 1 },
        { &Cpu::NOP, "INCr_e", 1 },
        { &Cpu::NOP, "DECr_e", 1 },
        { &Cpu::LDrn_e, "LDrn_e", 2 },
        { &Cpu::NOP, "RRA", 1 },

        // 0x20
        { &Cpu::NOP, "JRNZn", 2 },
        { &Cpu::LDHLnn, "LDHLnn", 3 },
        { &Cpu::LDHLIA, "LDHLIA", 1 },
        { &Cpu::NOP, "INCHL", 1 },
        { &Cpu::NOP, "INCr_h", 1 },
        { &Cpu::NOP, "DECr_h", 1 },
        { &Cpu::LDrn_h, "LDrn_h", 2 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::NOP, "JRZn", 2 },
        { &Cpu::ADDHLHL, "ADDHLHL", 1 },
        { &Cpu::LDAHLI, "LDAHLI", 1 },
        { &Cpu::NOP, "DECHL", 1 },
        { &Cpu::NOP, "INCr_l", 1 },
        { &Cpu::NOP, "DECr_l", 1 },
        { &Cpu::LDrn_l, "LDrn_l", 2 },
        { &Cpu::NOP, "CPL", 1 },

        // 0x30
        { &Cpu::NOP, "JRNCn", 2 },
        { &Cpu::LDSPnn, "LDSPnn", 3 },
        { &Cpu::LDHLDA, "LDHLDA", 1 },
        { &Cpu::NOP, "INCSP", 1 },
        { &Cpu::NOP, "INCHLm", 1 },
        { &Cpu::NOP, "DECHLm", 1 },
        { &Cpu::LDHLmn, "LDHLmn", 3 },
        { &Cpu::NOP, "SCF", 1 },
        { &Cpu::NOP, "JRCn", 2 },
        { &Cpu::ADDHLSP, "ADDHLSP", 1 },
        { &Cpu::LDAHLD, "LDAHLD", 1 },
        { &Cpu::NOP, "DECSP", 1 },
        { &Cpu::NOP, "INCr_a", 1 },
        { &Cpu::NOP, "DECr_a", 1 },
        { &Cpu::LDrn_a, "LDrn_a", 2 },
        { &Cpu::NOP, "CCF", 1 },

        // 0x40
        { &Cpu::LDrr_bb, "LDrr_bb", 1 },
        { &Cpu::LDrr_bc, "LDrr_bc", 1 },
        { &Cpu::LDrr_bd, "LDrr_bd", 1 },
        { &Cpu::LDrr_be, "LDrr_be", 1 },
        { &Cpu::LDrr_bh, "LDrr_bh", 1 },
        { &Cpu::LDrr_bl, "LDrr_bl", 1 },
        { &Cpu::LDrHLm_b, "LDrHLm_b", 1 },
        { &Cpu::LDrr_ba, "LDrr_ba", 1 },
        { &Cpu::LDrr_cb, "LDrr_cb", 1 },
        { &Cpu::LDrr_cc, "LDrr_cc", 1 },
        { &Cpu::LDrr_cd, "LDrr_cd", 1 },
        { &Cpu::LDrr_ce, "LDrr_ce", 1 },
        { &Cpu::LDrr_ch, "LDrr_ch", 1 },
        { &Cpu::LDrr_cl, "LDrr_cl", 1 },
        { &Cpu::LDrHLm_c, "LDrHLm_c", 1 },
        { &Cpu::LDrr_ca, "LDrr_ca", 1 },

        // 0x50
        { &Cpu::LDrr_db, "LDrr_db", 1 },
        { &Cpu::LDrr_dc, "LDrr_dc", 1 },
        { &Cpu::LDrr_dd, "LDrr_dd", 1 },
        { &Cpu::LDrr_de, "LDrr_de", 1 },
        { &Cpu::LDrr_dh, "LDrr_dh", 1 },
        { &Cpu::LDrr_dl, "LDrr_dl", 1 },
        { &Cpu::LDrHLm_d, "LDrHLm_d", 1 },
        { &Cpu::LDrr_da, "LDrr_da", 1 },
        { &Cpu::LDrr_eb, "LDrr_eb", 1 },
        { &Cpu::LDrr_ec, "LDrr_ec", 1 },
        { &Cpu::LDrr_ed, "LDrr_ed", 1 },
        { &Cpu::LDrr_ee, "LDrr_ee", 1 },
        { &Cpu::LDrr_eh, "LDrr_eh", 1 },
        { &Cpu::LDrr_el, "LDrr_el", 1 },
        { &Cpu::LDrHLm_e, "LDrHLm_e", 1 },
        { &Cpu::LDrr_ea, "LDrr_ea", 1 },

        // 0x60
        { &Cpu::LDrr_hb, "LDrr_hb", 1 },
        { &Cpu::LDrr_hc, "LDrr_hc", 1 },
        { &Cpu::LDrr_hd, "LDrr_hd", 1 },
        { &Cpu::LDrr_he, "LDrr_he", 1 },
        { &Cpu::LDrr_hh, "LDrr_hh", 1 },
        { &Cpu::LDrr_hl, "LDrr_hl", 1 },
        { &Cpu::LDrHLm_h, "LDrHLm_h", 1 },
        { &Cpu::LDrr_ha, "LDrr_ha", 1 },
        { &Cpu::LDrr_lb, "LDrr_lb", 1 },
        { &Cpu::LDrr_lc, "LDrr_lc", 1 },
        { &Cpu::LDrr_ld, "LDrr_ld", 1 },
        { &Cpu::LDrr_le, "LDrr_le", 1 },
        { &Cpu::LDrr_lh, "LDrr_lh", 1 },
        { &Cpu::LDrr_ll, "LDrr_ll", 1 },
        { &Cpu::LDrHLm_l, "LDrHLm_l", 1 },
        { &Cpu::LDrr_la, "LDrr_la", 1 },

        // 0x70
        { &Cpu::LDHLmr_b, "LDHLmr_b", 1 },
        { &Cpu::LDHLmr_c, "LDHLmr_c", 1 },
        { &Cpu::LDHLmr_d, "LDHLmr_d", 1 },
        { &Cpu::LDHLmr_e, "LDHLmr_e", 1 },
        { &Cpu::LDHLmr_h, "LDHLmr_h", 1 },
        { &Cpu::LDHLmr_l, "LDHLmr_l", 1 },
        { &Cpu::NOP, "HALT", 1 },
        { &Cpu::LDHLmr_a, "LDHLmr_a", 1 },
        { &Cpu::LDrr_ab, "LDrr_ab", 1 },
        { &Cpu::LDrr_ac, "LDrr_ac", 1 },
        { &Cpu::LDrr_ad, "LDrr_ad", 1 },
        { &Cpu::LDrr_ae, "LDrr_ae", 1 },
        { &Cpu::LDrr_ah, "LDrr_ah", 1 },
        { &Cpu::LDrr_al, "LDrr_al", 1 },
        { &Cpu::LDrHLm_a, "LDrHLm_a", 1 },
        { &Cpu::LDrr_aa, "LDrr_aa", 1 },

        // 0x80
        { &Cpu::ADDr_b, "ADDr_b", 1 },
        { &Cpu::ADDr_c, "ADDr_c", 1 },
        { &Cpu::ADDr_d, "ADDr_d", 1 },
        { &Cpu::ADDr_e, "ADDr_e", 1 },
        { &Cpu::ADDr_h, "ADDr_h", 1 },
        { &Cpu::ADDr_l, "ADDr_l", 1 },
        { &Cpu::ADDHL, "ADDHL", 1 },
        { &Cpu::ADDr_a, "ADDr_a", 1 },
        { &Cpu::ADCr_b, "ADCr_b", 1 },
        { &Cpu::ADCr_c, "ADCr_c", 1 },
        { &Cpu::ADCr_d, "ADCr_d", 1 },
        { &Cpu::ADCr_e, "ADCr_e", 1 },
        { &Cpu::ADCr_h, "ADCr_h", 1 },
        { &Cpu::ADCr_l, "ADCr_l", 1 },
        { &Cpu::ADCHL, "ADCHL", 1 },
        { &Cpu::ADCr_a, "ADCr_a", 1 },

        // 0x90
        { &Cpu::SUBr_b, "SUBr_b", 1 },
        { &Cpu::SUBr_c, "SUBr_c", 1 },
        { &Cpu::SUBr_d, "SUBr_d", 1 },
        { &Cpu::SUBr_e, "SUBr_e", 1 },
        { &Cpu::SUBr_h, "SUBr_h", 1 },
        { &Cpu::SUBr_l, "SUBr_l", 1 },
        { &Cpu::SUBHL, "SUBHL", 1 },
        { &Cpu::SUBr_a, "SUBr_a", 1 },
        { &Cpu::SBCr_b, "SBCr_b", 1 },
        { &Cpu::SBCr_c, "SBCr_c", 1 },
        { &Cpu::SBCr_d, "SBCr_d", 1 },
        { &Cpu::SBCr_e, "SBCr_e", 1 },
        { &Cpu::SBCr_h, "SBCr_h", 1 },
        { &Cpu::SBCr_l, "SBCr_l", 1 },
        { &Cpu::SBCHL, "SBCHL", 1 },
        { &Cpu::SBCr_a, "SBCr_a", 1 },

        // 0xA0
        { &Cpu::ANDr_b, "ANDr_b", 1 },
        { &Cpu::ANDr_c, "ANDr_c", 1 },
        { &Cpu::ANDr_d, "ANDr_d", 1 },
        { &Cpu::ANDr_e, "ANDr_e", 1 },
        { &Cpu::ANDr_h, "ANDr_h", 1 },
        { &Cpu::ANDr_l, "ANDr_l", 1 },
        { &Cpu::ANDHL, "ANDHL", 1 },
        { &Cpu::ANDr_a, "ANDr_a", 1 },
        { &Cpu::XORr_b, "XORr_b", 1 },
        { &Cpu::XORr_c, "XORr_c", 1 },
        { &Cpu::XORr_d, "XORr_d", 1 },
        { &Cpu::XORr_e, "XORr_e", 1 },
        { &Cpu::XORr_h, "XORr_h", 1 },
        { &Cpu::XORr_l, "XORr_l", 1 },
        { &Cpu::XORHL, "XORHL", 1 },
        { &Cpu::XORr_a, "XORr_a", 1 },

        // 0xB0
        { &Cpu::ORr_b, "ORr_b", 1 },
        { &Cpu::ORr_c, "ORr_c", 1 },
        { &Cpu::ORr_d, "ORr_d", 1 },
        { &Cpu::ORr_e, "ORr_e", 1 },
        { &Cpu::ORr_h, "ORr_h", 1 },
        { &Cpu::ORr_l, "ORr_l", 1 },
        { &Cpu::ORHL, "ORHL", 1 },
        { &Cpu::ORr_a, "ORr_a", 1 },
        { &Cpu::CPr_b, "CPr_b", 1 },
        { &Cpu::CPr_c, "CPr_c", 1 },
        { &Cpu::CPr_d, "CPr_d", 1 },
        { &Cpu::CPr_e, "CPr_e", 1 },
        { &Cpu::CPr_h, "CPr_h", 1 },
        { &Cpu::CPr_l, "CPr_l", 1 },
        { &Cpu::CPHL, "CPHL", 1 },
        { &Cpu::CPr_a, "CPr_a", 1 },

        // 0xC0
        { &Cpu::NOP, "RETNZ", 1 },
        { &Cpu::NOP, "POPBC", 1 },
        { &Cpu::NOP, "JPNZnn", 3 },
        { &Cpu::NOP, "JPnn", 3 },
        { &Cpu::NOP, "CALLNZnn", 3 },
        { &Cpu::NOP, "PUSHBC", 1 },
        { &Cpu::ADDn, "ADDn", 2 },
        { &Cpu::NOP, "RST00", 1 },
        { &Cpu::NOP, "RETZ", 1 },
        { &Cpu::NOP, "RET", 1 },
        { &Cpu::NOP, "JPZnn", 3 },
        { &Cpu::NOP, "MAPcb", 1 },
        { &Cpu::NOP, "CALLZnn", 3 },
        { &Cpu::NOP, "CALLnn", 3 },
        { &Cpu::ADCn, "ADCn", 2 },
        { &Cpu::NOP, "RST08", 1 },

        // 0xD0
        { &Cpu::NOP, "RETNC", 1 },
        { &Cpu::NOP, "POPDE", 1 },
        { &Cpu::NOP, "JPNCnn", 3 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::NOP, "CALLNCnn", 3 },
        { &Cpu::NOP, "PUSHDE", 1 },
        { &Cpu::SUBn, "SUBn", 2 },
        { &Cpu::NOP, "RST10", 1 },
        { &Cpu::NOP, "RETC", 1 },
        { &Cpu::NOP, "RETI", 1 },
        { &Cpu::NOP, "JPCnn", 3 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::NOP, "CALLCnn", 3 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::SBCn, "SBCn", 2 },
        { &Cpu::NOP, "RST18", 1 },

        // 0xE0
        { &Cpu::LDIOnA, "LDIOnA", 2 },
        { &Cpu::NOP, "POPHL", 1 },
        { &Cpu::LDIOCA, "LDIOCA", 1 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::NOP, "PUSHHL", 1 },
        { &Cpu::ANDn, "ANDn", 2 },
        { &Cpu::NOP, "RST20", 1 },
        { &Cpu::ADDSPn, "ADDSPn", 2 },
        { &Cpu::NOP, "JPHL", 1 },
        { &Cpu::LDmmA, "LDmmA", 3 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::ORn, "ORn", 2 },
        { &Cpu::NOP, "RST28", 1 },

        // 0xF0
        { &Cpu::LDAIOn, "LDAIOn", 2 },
        { &Cpu::NOP, "POPAF", 1 },
        { &Cpu::LDAIOC, "LDAIOC", 1 },
        { &Cpu::NOP, "DI", 1 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::NOP, "PUSHAF", 1 },
        { &Cpu::XORn, "XORn", 2 },
        { &Cpu::NOP, "RST30", 1 },
        { &Cpu::LDHLSPn, "LDHLSPn", 2 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::LDAmm, "LDAmm", 3 },
        { &Cpu::NOP, "EI", 1 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::NOP, "XX", 1 },
        { &Cpu::CPn, "CPn", 2 },
        { &Cpu::NOP, "RST38", 1 },
};

Cpu::opinfo_t Cpu::cbopmap[256] = {
        // CB 0x00
        {&Cpu::NOP, "RLCr_b", 1},
        {&Cpu::NOP, "RLCr_c", 1},
        {&Cpu::NOP, "RLCr_d", 1},
        {&Cpu::NOP, "RLCr_e", 1},
        {&Cpu::NOP, "RLCr_h", 1},
        {&Cpu::NOP, "RLCr_l", 1},
        {&Cpu::NOP, "RLCHL", 1},
        {&Cpu::NOP, "RLCr_a", 1},
        {&Cpu::NOP, "RRCr_b", 1},
        {&Cpu::NOP, "RRCr_c", 1},
        {&Cpu::NOP, "RRCr_d", 1},
        {&Cpu::NOP, "RRCr_e", 1},
        {&Cpu::NOP, "RRCr_h", 1},
        {&Cpu::NOP, "RRCr_l", 1},
        {&Cpu::NOP, "RRCHL", 1},
        {&Cpu::NOP, "RRCr_a", 1},

        // CB 0x10
        {&Cpu::NOP, "RLr_b", 1},
        {&Cpu::NOP, "RLr_c", 1},
        {&Cpu::NOP, "RLr_d", 1},
        {&Cpu::NOP, "RLr_e", 1},
        {&Cpu::NOP, "RLr_h", 1},
        {&Cpu::NOP, "RLr_l", 1},
        {&Cpu::NOP, "RLHL", 1},
        {&Cpu::NOP, "RLr_a", 1},
        {&Cpu::NOP, "RRr_b", 1},
        {&Cpu::NOP, "RRr_c", 1},
        {&Cpu::NOP, "RRr_d", 1},
        {&Cpu::NOP, "RRr_e", 1},
        {&Cpu::NOP, "RRr_h", 1},
        {&Cpu::NOP, "RRr_l", 1},
        {&Cpu::NOP, "RRHL", 1},
        {&Cpu::NOP, "RRr_a", 1},

        // CB 0x20
        {&Cpu::NOP, "SLAr_b", 1},
        {&Cpu::NOP, "SLAr_c", 1},
        {&Cpu::NOP, "SLAr_d", 1},
        {&Cpu::NOP, "SLAr_e", 1},
        {&Cpu::NOP, "SLAr_h", 1},
        {&Cpu::NOP, "SLAr_l", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "SLAr_a", 1},
        {&Cpu::NOP, "SRAr_b", 1},
        {&Cpu::NOP, "SRAr_c", 1},
        {&Cpu::NOP, "SRAr_d", 1},
        {&Cpu::NOP, "SRAr_e", 1},
        {&Cpu::NOP, "SRAr_h", 1},
        {&Cpu::NOP, "SRAr_l", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "SRAr_a", 1},

        // CB 0x30
        {&Cpu::SWAPr_b, "SWAPr_b", 1},
        {&Cpu::SWAPr_c, "SWAPr_c", 1},
        {&Cpu::SWAPr_d, "SWAPr_d", 1},
        {&Cpu::SWAPr_e, "SWAPr_e", 1},
        {&Cpu::SWAPr_h, "SWAPr_h", 1},
        {&Cpu::SWAPr_l, "SWAPr_l", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::SWAPr_a, "SWAPr_a", 1},
        {&Cpu::NOP, "SRLr_b", 1},
        {&Cpu::NOP, "SRLr_c", 1},
        {&Cpu::NOP, "SRLr_d", 1},
        {&Cpu::NOP, "SRLr_e", 1},
        {&Cpu::NOP, "SRLr_h", 1},
        {&Cpu::NOP, "SRLr_l", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "SRLr_a", 1},

        // CB 0x40
        {&Cpu::NOP, "BIT0b", 1},
        {&Cpu::NOP, "BIT0c", 1},
        {&Cpu::NOP, "BIT0d", 1},
        {&Cpu::NOP, "BIT0e", 1},
        {&Cpu::NOP, "BIT0h", 1},
        {&Cpu::NOP, "BIT0l", 1},
        {&Cpu::NOP, "BIT0m", 1},
        {&Cpu::NOP, "BIT0a", 1},
        {&Cpu::NOP, "BIT1b", 1},
        {&Cpu::NOP, "BIT1c", 1},
        {&Cpu::NOP, "BIT1d", 1},
        {&Cpu::NOP, "BIT1e", 1},
        {&Cpu::NOP, "BIT1h", 1},
        {&Cpu::NOP, "BIT1l", 1},
        {&Cpu::NOP, "BIT1m", 1},
        {&Cpu::NOP, "BIT1a", 1},

        // CB 0x50
        {&Cpu::NOP, "BIT2b", 1},
        {&Cpu::NOP, "BIT2c", 1},
        {&Cpu::NOP, "BIT2d", 1},
        {&Cpu::NOP, "BIT2e", 1},
        {&Cpu::NOP, "BIT2h", 1},
        {&Cpu::NOP, "BIT2l", 1},
        {&Cpu::NOP, "BIT2m", 1},
        {&Cpu::NOP, "BIT2a", 1},
        {&Cpu::NOP, "BIT3b", 1},
        {&Cpu::NOP, "BIT3c", 1},
        {&Cpu::NOP, "BIT3d", 1},
        {&Cpu::NOP, "BIT3e", 1},
        {&Cpu::NOP, "BIT3h", 1},
        {&Cpu::NOP, "BIT3l", 1},
        {&Cpu::NOP, "BIT3m", 1},
        {&Cpu::NOP, "BIT3a", 1},

        // CB 0x60
        {&Cpu::NOP, "BIT4b", 1},
        {&Cpu::NOP, "BIT4c", 1},
        {&Cpu::NOP, "BIT4d", 1},
        {&Cpu::NOP, "BIT4e", 1},
        {&Cpu::NOP, "BIT4h", 1},
        {&Cpu::NOP, "BIT4l", 1},
        {&Cpu::NOP, "BIT4m", 1},
        {&Cpu::NOP, "BIT4a", 1},
        {&Cpu::NOP, "BIT5b", 1},
        {&Cpu::NOP, "BIT5c", 1},
        {&Cpu::NOP, "BIT5d", 1},
        {&Cpu::NOP, "BIT5e", 1},
        {&Cpu::NOP, "BIT5h", 1},
        {&Cpu::NOP, "BIT5l", 1},
        {&Cpu::NOP, "BIT5m", 1},
        {&Cpu::NOP, "BIT5a", 1},

        // CB 0x70
        {&Cpu::NOP, "BIT6b", 1},
        {&Cpu::NOP, "BIT6c", 1},
        {&Cpu::NOP, "BIT6d", 1},
        {&Cpu::NOP, "BIT6e", 1},
        {&Cpu::NOP, "BIT6h", 1},
        {&Cpu::NOP, "BIT6l", 1},
        {&Cpu::NOP, "BIT6m", 1},
        {&Cpu::NOP, "BIT6a", 1},
        {&Cpu::NOP, "BIT7b", 1},
        {&Cpu::NOP, "BIT7c", 1},
        {&Cpu::NOP, "BIT7d", 1},
        {&Cpu::NOP, "BIT7e", 1},
        {&Cpu::NOP, "BIT7h", 1},
        {&Cpu::NOP, "BIT7l", 1},
        {&Cpu::NOP, "BIT7m", 1},
        {&Cpu::NOP, "BIT7a", 1},

        // CB 0x80
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},

        // CB 0x90
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},

        // CB 0xA0
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},

        // CB 0xB0
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},

        // CB 0xC0
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},

        // CB 0xD0
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},

        // CB 0xE0
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},

        // CB 0xF0
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},
        {&Cpu::NOP, "XX", 1},

};
