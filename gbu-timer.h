#pragma once

#include "interrupts.h"

#include <stdint.h>

class GbuTimer
{
public:
	GbuTimer(Interrupts* interrupts)
		: _interrupts(interrupts)
		, tima(0)
		, tma(0)
		, tac(0)
		, tac_on(false)
	{}

	void tick(uint32_t m)
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
		bool tick_tima = false;
		if ((counter % mod) < (oldCounter % mod))
			tick_tima = true;

		if (tick_tima)
		{
			tima++;
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
private:
	Interrupts* _interrupts;


	uint16_t counter;
	bool tac_on;

};