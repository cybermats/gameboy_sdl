#pragma once

#include <stdint.h>

class Interrupts
{
public:
	enum InterruptFlag {
		None = 0,
		Vblank = (1 << 0),
		LcdStat = (1 << 1),
		Timer = (1 << 2),
		Serial = (1 << 3),
		Joypad = (1 << 4),
	};

	Interrupts()
		: interrupt_master_enable(true)
		, interrupt_enabled(None)
		, interrupt_flagged(None)
	{}

	uint16_t getInterrupts()
	{
		if (interrupt_master_enable && interrupt_enabled && interrupt_flagged)
		{
			auto fire = interrupt_enabled & interrupt_flagged;
			interrupt_master_enable = false;
			if (fire & Vblank)
			{
				interrupt_flagged = (InterruptFlag)(interrupt_flagged & ~Vblank);
				return 0x40;
			}
			if (fire & LcdStat)
			{
				interrupt_flagged = (InterruptFlag)(interrupt_flagged & ~LcdStat);
				return 0x48;
			}
			if (fire & Timer)
			{
				interrupt_flagged = (InterruptFlag)(interrupt_flagged & ~Timer);
				return 0x50;
			}
			if (fire & Serial)
			{
				interrupt_flagged = (InterruptFlag)(interrupt_flagged & ~Serial);
				return 0x58;
			}
			if (fire & Joypad)
			{
				interrupt_flagged = (InterruptFlag)(interrupt_flagged & ~Joypad);
				return 0x60;
			}
		}
		return 0;
	}

	void flagInterrupt(InterruptFlag flag)
	{
		if (interrupt_enabled & flag)
		{
			interrupt_flagged = (InterruptFlag)(interrupt_flagged | flag);
		}
	}

	uint8_t readIF()
	{
		return interrupt_flagged;
	}

	void writeIF(uint8_t value)
	{
		interrupt_flagged = (InterruptFlag)value;
	}

	uint8_t readIE()
	{
		return (uint8_t)interrupt_enabled;
	}

	void writeIE(uint8_t value)
	{
		interrupt_enabled = (InterruptFlag)value;
	}

	bool readIME()
	{
		return interrupt_master_enable;
	}

	void enableInterrupts()
	{
		interrupt_master_enable = true;
	}

	void disableInterrupts()
	{
		interrupt_master_enable = false;
	}

private:
	bool interrupt_master_enable;
	InterruptFlag interrupt_enabled;
	InterruptFlag interrupt_flagged;
};