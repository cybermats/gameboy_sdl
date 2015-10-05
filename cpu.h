#pragma once

#include "mmu.h"
#include "interrupts.h"

#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <exception>


#define Z_FLAG 0x80 // Zero
#define N_FLAG 0x40 // Subtract flag
#define H_FLAG 0x20 // Half-carry flag
#define C_FLAG 0x10 // Carry flag
#define ALL_FLAGS 0xF0

#define SET_FLAG(flag) r.f |= (flag)
#define RESET_FLAG(flag) r.f &= (0xF0 ^ (flag))


class Cpu {
private:
	struct Registers {
		Registers()
			: f(0), a(0)
			, c(0), b(0)
			, e(0), d(0)
			, l(0), h(0)
		{}

		uint8_t f;
		uint8_t a;
		uint8_t c;
		uint8_t b;
		uint8_t e;
		uint8_t d;
		uint8_t l;
		uint8_t h;

		uint16_t af()
		{
			return (a << 8) | f;
		}

		void af(uint16_t r)
		{
			a = r >> 8;
			f = r & 0xff;
		}

		uint16_t bc()
		{
			return (b << 8) | c;
		}

		void bc(uint16_t r)
		{
			b = r >> 8;
			c = r & 0xff;
		}

		uint16_t de()
		{
			return (d << 8) | e;
		}

		void de(uint16_t r)
		{
			d = r >> 8;
			e = r & 0xff;
		}

		uint16_t hl()
		{
			return (h << 8) | l;
		}

		void hl(uint16_t r)
		{
			h = r >> 8;
			l = r & 0xff;
		}
	};

public:

	Cpu(MMU* mbc, Interrupts* interrupts, GbuTimer* timer, bool useBios)
        : _mbc(mbc)
		, _interrupts(interrupts)
		, _timer(timer)
        , pc(useBios ? 0x0 : 0x0100)
        , sp(0)
    {}

    void execute()
    {
		if (!halt)
		{
			// Fetch
			uint8_t opcode = _mbc->readByte(pc++);

			// Decode
			_timer->tick(opmap[opcode].cycles);

			// Execute
			cycles = 0;
			(this->*opmap[opcode].func)();
			if (opcode != 0xcb)
				_timer->tick(cycles);
			cycles += opmap[opcode].cycles;

		}
		else
			_timer->tick(cycles); // fix for HALT

		checkInterrupts();
    }

    void MAPcb()
    {
        // Fetch
        uint8_t opcode = _mbc->readByte(pc++);
		// Decode
		_timer->tick(cbopmap[opcode].cycles);
		// Execute
		(this->*cbopmap[opcode].func)();
		cycles += cbopmap[opcode].cycles;
	}

    void printCode(size_t numCmds);

	void printStep(std::ostream& os);


    uint16_t getPc() const { return pc; }

//private:

    void decode();

	inline void checkInterrupts()
	{
		uint16_t irq = _interrupts->getInterrupts();
		if (irq)
		{
			sp -= 2; 
			_mbc->writeShort(sp, pc);
			pc = irq;
			_timer->tick(3); // tick ahead
			cycles += 3;
			halt = false;
		}
		else
		{
			if (halt)
			{
				if (_interrupts->readIF())
					halt = false;
			}
		}
	}


    MMU* _mbc;
	Interrupts* _interrupts;
	GbuTimer* _timer;

	// Clock
	uint8_t cycles;
	
	uint16_t pc;
    uint16_t sp;
	
	Registers r;

    bool halt;


    typedef void(Cpu::*opfunc_t)();

    struct opinfo_t {
        opfunc_t func;
        const char* name;
        unsigned char size;
		unsigned char cycles;
    };

    void NOP() { }

    // Load/Store

    void LDrr_bb();
    void LDrr_bc();
    void LDrr_bd();
    void LDrr_be();
    void LDrr_bh();
    void LDrr_bl();
    void LDrr_ba();
    void LDrr_cb();
    void LDrr_cc();
    void LDrr_cd();
    void LDrr_ce();
    void LDrr_ch();
    void LDrr_cl();
    void LDrr_ca();
    void LDrr_db();
    void LDrr_dc();
    void LDrr_dd();
    void LDrr_de();
    void LDrr_dh();
    void LDrr_dl();
    void LDrr_da();
    void LDrr_eb();
    void LDrr_ec();
    void LDrr_ed();
    void LDrr_ee();
    void LDrr_eh();
    void LDrr_el();
    void LDrr_ea();
    void LDrr_hb();
    void LDrr_hc();
    void LDrr_hd();
    void LDrr_he();
    void LDrr_hh();
    void LDrr_hl();
    void LDrr_ha();
    void LDrr_lb();
    void LDrr_lc();
    void LDrr_ld();
    void LDrr_le();
    void LDrr_lh();
    void LDrr_ll();
    void LDrr_la();
    void LDrr_ab();
    void LDrr_ac();
    void LDrr_ad();
    void LDrr_ae();
    void LDrr_ah();
    void LDrr_al();
    void LDrr_aa();

    void LDrHLm_b();
    void LDrHLm_c();
    void LDrHLm_d();
    void LDrHLm_e();
    void LDrHLm_h();
    void LDrHLm_l();
    void LDrHLm_a();

    void LDHLmr_b();
    void LDHLmr_c();
    void LDHLmr_d();
    void LDHLmr_e();
    void LDHLmr_h();
    void LDHLmr_l();
    void LDHLmr_a();

    void LDrn_b();
    void LDrn_c();
    void LDrn_d();
    void LDrn_e();
    void LDrn_h();
    void LDrn_l();
    void LDrn_a();

    void LDHLmn();

    void LDBCmA();
    void LDDEmA();

    void LDmmA();

    void LDABCm();
    void LDADEm();

    void LDAmm();

    void LDBCnn();
    void LDDEnn();
    void LDHLnn();
    void LDSPnn();

    void LDHLIA();
    void LDAHLI();

    void LDHLDA();
    void LDAHLD();

    void LDAIOn();
    void LDIOnA();
    void LDAIOC();
    void LDIOCA();

	void LDHLSPn();
	void LDSPHL();

    void LDmmSP();

    void SWAPr_b();
    void SWAPr_c();
    void SWAPr_d();
    void SWAPr_e();
    void SWAPr_h();
    void SWAPr_l();
    void SWAPr_a();
    void SWAPHL();

    // Data processing

    void ADDr_b();
    void ADDr_c();
    void ADDr_d();
    void ADDr_e();
    void ADDr_h();
    void ADDr_l();
    void ADDr_a();
    void ADDHL();
    void ADDn();
    void ADDHLBC();
    void ADDHLDE();
    void ADDHLHL();
    void ADDHLSP();
    void ADDSPn();

    void ADCr_b();
    void ADCr_c();
    void ADCr_d();
    void ADCr_e();
    void ADCr_h();
    void ADCr_l();
    void ADCr_a();
    void ADCHL();
    void ADCn();

    void SUBr_b();
    void SUBr_c();
    void SUBr_d();
    void SUBr_e();
    void SUBr_h();
    void SUBr_l();
    void SUBr_a();
    void SUBHL();
    void SUBn();

    void SBCr_b();
    void SBCr_c();
    void SBCr_d();
    void SBCr_e();
    void SBCr_h();
    void SBCr_l();
    void SBCr_a();
    void SBCHL();
    void SBCn();

    void CPr_b();
    void CPr_c();
    void CPr_d();
    void CPr_e();
    void CPr_h();
    void CPr_l();
    void CPr_a();
    void CPHL();
    void CPn();

    void ANDr_b();
    void ANDr_c();
    void ANDr_d();
    void ANDr_e();
    void ANDr_h();
    void ANDr_l();
    void ANDr_a();
    void ANDHL();
    void ANDn();

    void ORr_b();
    void ORr_c();
    void ORr_d();
    void ORr_e();
    void ORr_h();
    void ORr_l();
    void ORr_a();
    void ORHL();
    void ORn();

    void XORr_b();
    void XORr_c();
    void XORr_d();
    void XORr_e();
    void XORr_h();
    void XORr_l();
    void XORr_a();
    void XORHL();
    void XORn();

    void INCr_b();
    void INCr_c();
    void INCr_d();
    void INCr_e();
    void INCr_h();
    void INCr_l();
    void INCr_a();
    void INCHLm();

    void DECr_b();
    void DECr_c();
    void DECr_d();
    void DECr_e();
    void DECr_h();
    void DECr_l();
    void DECr_a();
    void DECHLm();

    void INCBC();
    void INCDE();
    void INCHL();
    void INCSP();

    void DECBC();
    void DECDE();
    void DECHL();
    void DECSP();
	void DAA();

    /* Bit manipulation */
    void BIT0b();
    void BIT0c();
    void BIT0d();
    void BIT0e();
    void BIT0h();
    void BIT0l();
    void BIT0a();
    void BIT0m();

    void RES0b();
    void RES0c();
    void RES0d();
    void RES0e();
    void RES0h();
    void RES0l();
    void RES0a();
    void RES0m();

    void SET0b();
    void SET0c();
    void SET0d();
    void SET0e();
    void SET0h();
    void SET0l();
    void SET0a();
    void SET0m();

    void BIT1b();
    void BIT1c();
    void BIT1d();
    void BIT1e();
    void BIT1h();
    void BIT1l();
    void BIT1a();
    void BIT1m();

    void RES1b();
    void RES1c();
    void RES1d();
    void RES1e();
    void RES1h();
    void RES1l();
    void RES1a();
    void RES1m();

    void SET1b();
    void SET1c();
    void SET1d();
    void SET1e();
    void SET1h();
    void SET1l();
    void SET1a();
    void SET1m();

    void BIT2b();
    void BIT2c();
    void BIT2d();
    void BIT2e();
    void BIT2h();
    void BIT2l();
    void BIT2a();
    void BIT2m();

    void RES2b();
    void RES2c();
    void RES2d();
    void RES2e();
    void RES2h();
    void RES2l();
    void RES2a();
    void RES2m();

    void SET2b();
    void SET2c();
    void SET2d();
    void SET2e();
    void SET2h();
    void SET2l();
    void SET2a();
    void SET2m();

    void BIT3b();
    void BIT3c();
    void BIT3d();
    void BIT3e();
    void BIT3h();
    void BIT3l();
    void BIT3a();
    void BIT3m();

    void RES3b();
    void RES3c();
    void RES3d();
    void RES3e();
    void RES3h();
    void RES3l();
    void RES3a();
    void RES3m();

    void SET3b();
    void SET3c();
    void SET3d();
    void SET3e();
    void SET3h();
    void SET3l();
    void SET3a();
    void SET3m();

    void BIT4b();
    void BIT4c();
    void BIT4d();
    void BIT4e();
    void BIT4h();
    void BIT4l();
    void BIT4a();
    void BIT4m();

    void RES4b();
    void RES4c();
    void RES4d();
    void RES4e();
    void RES4h();
    void RES4l();
    void RES4a();
    void RES4m();

    void SET4b();
    void SET4c();
    void SET4d();
    void SET4e();
    void SET4h();
    void SET4l();
    void SET4a();
    void SET4m();

    void BIT5b();
    void BIT5c();
    void BIT5d();
    void BIT5e();
    void BIT5h();
    void BIT5l();
    void BIT5a();
    void BIT5m();

    void RES5b();
    void RES5c();
    void RES5d();
    void RES5e();
    void RES5h();
    void RES5l();
    void RES5a();
    void RES5m();

    void SET5b();
    void SET5c();
    void SET5d();
    void SET5e();
    void SET5h();
    void SET5l();
    void SET5a();
    void SET5m();

    void BIT6b();
    void BIT6c();
    void BIT6d();
    void BIT6e();
    void BIT6h();
    void BIT6l();
    void BIT6a();
    void BIT6m();

    void RES6b();
    void RES6c();
    void RES6d();
    void RES6e();
    void RES6h();
    void RES6l();
    void RES6a();
    void RES6m();

    void SET6b();
    void SET6c();
    void SET6d();
    void SET6e();
    void SET6h();
    void SET6l();
    void SET6a();
    void SET6m();

    void BIT7b();
    void BIT7c();
    void BIT7d();
    void BIT7e();
    void BIT7h();
    void BIT7l();
    void BIT7a();
    void BIT7m();

    void RES7b();
    void RES7c();
    void RES7d();
    void RES7e();
    void RES7h();
    void RES7l();
    void RES7a();
    void RES7m();

    void SET7b();
    void SET7c();
    void SET7d();
    void SET7e();
    void SET7h();
    void SET7l();
    void SET7a();
    void SET7m();

    void RLA();
    void RLCA();
    void RRA();
    void RRCA();

    void RLr_b();
    void RLr_c();
    void RLr_d();
    void RLr_e();
    void RLr_h();
    void RLr_l();
    void RLr_a();
    void RLHL();

    void RLCr_b();
    void RLCr_c();
    void RLCr_d();
    void RLCr_e();
    void RLCr_h();
    void RLCr_l();
    void RLCr_a();
    void RLCHL();

    void RRr_b();
    void RRr_c();
    void RRr_d();
    void RRr_e();
    void RRr_h();
    void RRr_l();
    void RRr_a();
    void RRHL();

    void RRCr_b();
    void RRCr_c();
    void RRCr_d();
    void RRCr_e();
    void RRCr_h();
    void RRCr_l();
    void RRCr_a();
    void RRCHL();

    void SLAr_b();
    void SLAr_c();
    void SLAr_d();
    void SLAr_e();
    void SLAr_h();
    void SLAr_l();
    void SLAr_a();
    void SLAHL();

    void SRAr_b();
    void SRAr_c();
    void SRAr_d();
    void SRAr_e();
    void SRAr_h();
    void SRAr_l();
    void SRAr_a();
    void SRAHL();

    void SRLr_b();
    void SRLr_c();
    void SRLr_d();
    void SRLr_e();
    void SRLr_h();
    void SRLr_l();
    void SRLr_a();
    void SRLHL();

    void CPL();
    void CCF();
    void SCF();

    /* Pushs and pops */
    void PUSHBC();
    void PUSHDE();
    void PUSHHL();
    void PUSHAF();

    void POPBC();
    void POPDE();
    void POPHL();
    void POPAF();


    /* Jumps */

    void JPnn();
    void JPHL();
    void JPNZnn();
    void JPZnn();
    void JPNCnn();
    void JPCnn();

    void JRn();
    void JRNZn();
    void JRZn();
    void JRNCn();
    void JRCn();

    void CALLnn();
    void CALLNZnn();
    void CALLZnn();
    void CALLNCnn();
    void CALLCnn();

    void RET();
    void RETI();
    void RETNZ();
    void RETZ();
    void RETNC();
    void RETC();

    void RST00();
    void RST08();
    void RST10();
    void RST18();
    void RST20();
    void RST28();
    void RST30();
    void RST38();

    void STOP();
    void HALT();

    void DI();
    void EI();

    void Unimpl() {
        std::cout << "Unimplemented function" << std::endl;
        throw std::exception();
    }

    static opinfo_t opmap[];
    static opinfo_t cbopmap[];

    static unsigned char _add_flags[];
};