#include "gbu-timer.h"

unsigned char GbuTimer::read(unsigned short addr)
{
	switch (addr)
	{
	case 0xFF04:
		return div;
	case 0xFF05:
		return tima;
	case 0xFF06:
		return tma;
	case 0xFF07:
		return tac;
	}
	return 0;
}

void GbuTimer::write(unsigned short addr, unsigned char value)
{
	switch (addr)
	{
	case 0xFF04:
		counter = 0;
		return;
	case 0xFF05:
		tima = value;
		return;
	case 0xFF06:
		tma = value;
		return;
	case 0xFF07:
		tac = value;
		tac_on = (value & 0x04) > 0;
		return;
	}
	return;

}


