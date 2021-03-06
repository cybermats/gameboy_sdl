#pragma once

#include "interrupts.h"

#include <stdint.h>

class GbuTimer
{
public:
	GbuTimer(Interrupts* interrupts)
		: _interrupts(interrupts)
		, counter(0x00)
//		, counter(0xab00)
		, tima(0)
		, tma(0)
		, tac(0)
		, tac_on(false)
	{}

	void tick(uint16_t m)
	{
		auto oldCounter = counter;
		counter += 4*m;
		div = counter >> 8;

		if (!tac_on)
			return;

		auto mod = 512;
		switch (tac & 0x03)
		{
		case 0:
			mod = 1024; break;
		case 1:
			mod = 16; break;
		case 2:
			mod = 64; break;
		case 3:
			mod = 256; break;
		}
		auto newCounter = oldCounter + 4 * m; // handle overflows
		auto tima_ticks = (newCounter / mod) - (oldCounter / mod);

		for (int i = 0; i < tima_ticks; ++i)
		{
			++tima;
			if (!tima)
			{
				tima = tma;
				_interrupts->flagInterrupt(Interrupts::Timer);
			}
		}
	}


	unsigned char read(unsigned short addr);
	void write(unsigned short addr, unsigned char value);

	uint8_t div;
	uint8_t tima;
	uint8_t tma;
	uint8_t tac;
	uint16_t counter;
private:
	Interrupts* _interrupts;


	bool tac_on;

};