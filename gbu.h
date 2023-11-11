#pragma once

#include "cpu.h"
#include "gpu.h"
#include "mmu.h"
#include "mbc.h"
#include "cartridge.h"
#include "interrupts.h"
#include "gbu-timer.h"
#include "joypad.h"
#include "gbu-serial.h"
#include "gb-audio.h"

#include <memory>
#include <iostream>
#include <iomanip>
#include <deque>
#include <iostream>
#include <fstream>
#include <cstdio>

class Gbu
{
public:
    Gbu(const char* filename, bool useBios, bool print_callstack)
		: _print_callstack(print_callstack)
    {
        Cartridge cartridge(filename);
        _mbc = cartridge.getMBC();
		_interrupts = std::make_unique<Interrupts>();
		_timer = std::make_unique<GbuTimer>(_interrupts.get());
		_joypad = std::make_unique<Joypad>(_interrupts.get());
		_serial = std::make_unique<Serial>();
		_audio = std::make_unique<Audio>();


        _mmu = std::unique_ptr<MMU>(new MMU(_mbc.get(), _interrupts.get(), _timer.get(), _joypad.get(), _serial.get(), _audio.get(), useBios));
        _gpu = std::unique_ptr<Gpu>(new Gpu(_mmu.get(), _interrupts.get()));
        _mmu->setGpu(_gpu.get());
        _cpu = std::unique_ptr<Cpu>(new Cpu(_mmu.get(), _interrupts.get(), _timer.get(), useBios));
		


		if (_print_callstack)
			_callstack_stream.open("callstack.txt");

    }

	~Gbu()
	{
		if (_print_callstack)
			_callstack_stream.close();
	}

    void runFrame()
    {
        while(!_gpu->hasImage())
        {
#if 1
			static volatile int a = 0xc2b4;
			if (_cpu->pc == a)
			{
				a = 0;
			}
#endif

			if (_print_callstack && !_mmu->isBios() && !_cpu->halt)
				printStep();
			_cpu->execute();
            _gpu->checkline(_cpu->cycles);
			_audio->tick(_cpu->cycles);
//			_timer->tick(_cpu->m);
        }
		_audio->end_frame();
    }

    std::vector<uint8_t>& getBuffer()
    {
        return _gpu->getFrame();
    }

	bool getAudio(std::vector<short>& buffer)
	{
		if (buffer.size() > _audio->samples_available())
			return false;

		_audio->read_samples(&buffer[0], buffer.size());
		return true;
	}

	void printStep()
	{
		auto& r = _cpu->r;
		auto& pc = _cpu->pc;
		auto& sp = _cpu->sp;
		auto& opmap = _cpu->opmap;
		auto& cbopmap = _cpu->cbopmap;

		static int counter = 0;
		if ((counter++)%16 == 0)
		{
			_callstack_stream << " AF  ZNHC  BC   DE   HL   SP   PC   Op  IME  IE   IF  DIV  TIMA TMA  TAC Counter Mne" << std::endl;
		}

		_callstack_stream << std::hex
			<< std::setfill('0') << std::setw(4)
			<< (int)r.af()
			<< " "
			<< ((r.f & Z_FLAG) ? "1" : "0")
			<< ((r.f & N_FLAG) ? "1" : "0")
			<< ((r.f & H_FLAG) ? "1" : "0")
			<< ((r.f & C_FLAG) ? "1" : "0")
			<< " "
			<< std::setfill('0') << std::setw(4)
			<< r.bc() << " "
			<< std::setfill('0') << std::setw(4)
			<< r.de() << " "
			<< std::setfill('0') << std::setw(4)
			<< r.hl() << " "
			<< std::setfill('0') << std::setw(4)
			<< sp << " "
			<< std::setfill('0') << std::setw(4)
			<< pc << " ";

		auto opcode = _mmu->readByte(pc);
		auto opitem = opmap[opcode];
		bool cb = opcode == 0xcb;
		if (cb)
		{
			opcode = _mmu->readByte(pc + 1);
			opitem = cbopmap[opcode];
		}
		_callstack_stream << std::hex;
		_callstack_stream
			<< "[" << std::setfill('0') << std::setw(2) << (int)opcode << "] "
			<< "[" << ((_interrupts->readIME()) ? "E" : " ") << "] "
			<< "[" << std::setfill('0') << std::setw(2) << (int)_interrupts->readIE() << "] "
			<< "[" << std::setfill('0') << std::setw(2) << (int)_interrupts->readIF() << "] "
			<< "[" << std::setfill('0') << std::setw(2) << (int)_timer->div << "] "
			<< "[" << std::setfill('0') << std::setw(2) << (int)_timer->tima << "] "
			<< "[" << std::setfill('0') << std::setw(2) << (int)_timer->tma << "] "
			<< "[" << std::setfill('0') << std::setw(2) << (int)_timer->tac << "] "
			<< "[" << std::setfill('0') << std::setw(4) << (int)_timer->counter << "]  "

			<< opitem.name << " ";

		for (uint8_t j = 1; j < opitem.size; ++j)
			_callstack_stream << std::setfill('0') << std::setw(2) << (int)_mmu->readByte(pc + j) << " ";
		_callstack_stream << std::endl;

		if (_interrupts->readIME() && _interrupts->readIF())
			_callstack_stream << "IRQ: " << std::hex << (int)_interrupts->readIF() << std::endl;

		_callstack_stream.flush();

	}

	Joypad* joypad()
	{
		return _joypad.get();
	}

private:
    std::unique_ptr<IMBC> _mbc;
    std::unique_ptr<MMU> _mmu;
    std::unique_ptr<Gpu> _gpu;
    std::unique_ptr<Cpu> _cpu;
	std::unique_ptr<Interrupts> _interrupts;
	std::unique_ptr<GbuTimer> _timer;
	std::unique_ptr<Joypad> _joypad;
	std::unique_ptr<Serial> _serial;
	std::unique_ptr<Audio> _audio;
	std::ofstream _callstack_stream;
	bool _print_callstack;
};